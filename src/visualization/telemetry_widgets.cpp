#include "visualization/telemetry_widgets.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <limits>
#include <vector>

#include "imgui.h"
#include "implot.h"

static void FormatLapTime(char* buffer, size_t size, float seconds) {
  const float clamped = std::max(0.0f, seconds);
  const int minutes = static_cast<int>(clamped) / 60;
  const float remainder = clamped - minutes * 60.0f;
  std::snprintf(buffer, size, "%d:%06.3f", minutes, remainder);
}

static void RenderLeaderboard(const TelemetrySample* s) {
  if (!s) return;
  static const std::array<const char*, 10> drivers = {"VER", "PER", "LEC", "SAI", "HAM",
                                                      "RUS", "NOR", "PIA", "ALO", "GAS"};
  const ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit;
  if (ImGui::BeginTable("leaderboard", 4, flags)) {
    ImGui::TableSetupColumn("Pos");
    ImGui::TableSetupColumn("Driver");
    ImGui::TableSetupColumn("Gap");
    ImGui::TableSetupColumn("Lap");
    ImGui::TableHeadersRow();
    
    char lap_time[32];
    for (size_t i = 0; i < drivers.size(); ++i) {
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("%zu", i + 1);
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%s", drivers[i]);
      ImGui::TableSetColumnIndex(2);
      if (i == 0) ImGui::Text("Interval");
      else ImGui::Text("+%.2fs", 0.5f * i);
      ImGui::TableSetColumnIndex(3);
      FormatLapTime(lap_time, sizeof(lap_time), s->lap_time + 0.1f * i);
      ImGui::Text("%s", lap_time);
    }
    ImGui::EndTable();
  }
}

static void RenderTimingTower(TelemetryStream& stream, float width) {
  const bool expanded = stream.State().timing_tower_expanded;
  // 折叠模式增加宽度以容纳多列：Interval, Tyre, Pit
  ImGui::BeginChild("TimingTower", ImVec2(expanded ? width : 200, 0), true);
  
  // 顶部折叠/展开按钮
  if (ImGui::Button(expanded ? "<< " : " >>")) {
    const_cast<SessionState&>(stream.State()).timing_tower_expanded = !expanded;
  }
  
  if (!expanded) {
    // 折叠状态：多列大格子
    ImGui::Separator();
    static const std::array<const char*, 10> short_drivers = {"VER", "PER", "LEC", "SAI", "HAM", "RUS", "NOR", "PIA", "ALO", "GAS"};
    static const std::array<const char*, 10> intervals = {"-", "+1.2s", "+5.4s", "+0.3s", "+2.1s", "+0.8s", "+1.5s", "+3.2s", "+0.9s", "+4.0s"};
    
    // 模拟轮胎历史数据：每套轮胎的 (配方, 圈数)
    struct TyreSet { char comp; int age; };
    static const std::array<std::vector<TyreSet>, 10> tyre_histories = {{
      {{'M', 12}, {'S', 10}}, // VER: 第一套黄12, 第二套红10
      {{'M', 15}, {'S', 8}},  // PER
      {{'S', 18}},           // LEC
      {{'M', 10}, {'H', 12}}, // SAI
      {{'H', 25}},           // HAM
      {{'S', 10}, {'M', 12}}, // RUS
      {{'S', 5}, {'M', 18}},  // NOR
      {{'M', 15}},           // PIA
      {{'H', 30}},           // ALO
      {{'S', 8}, {'S', 12}}   // GAS
    }};

    static const std::array<ImVec4, 10> team_colors = {
      ImVec4(0, 0.23f, 0.47f, 1), // RBR
      ImVec4(0, 0.23f, 0.47f, 1), // RBR
      ImVec4(0.9f, 0, 0, 1),      // Ferrari
      ImVec4(0.9f, 0, 0, 1),      // Ferrari
      ImVec4(0, 0.83f, 0.75f, 1), // Mercedes
      ImVec4(0, 0.83f, 0.75f, 1), // Mercedes
      ImVec4(1, 0.5f, 0, 1),      // McLaren
      ImVec4(1, 0.5f, 0, 1),      // McLaren
      ImVec4(0, 0.56f, 0.45f, 1), // Aston
      ImVec4(0, 0.47f, 0.84f, 1)  // Alpine
    };

    for (int i = 0; i < short_drivers.size(); ++i) {
      ImGui::PushID(i);
      ImGui::BeginGroup();
      
      // 1. 车队颜色边条
      ImVec2 pos = ImGui::GetCursorScreenPos();
      ImGui::GetWindowDrawList()->AddRectFilled(
        pos,
        ImVec2(pos.x + 4, pos.y + 42),
        ImGui::ColorConvertFloat4ToU32(team_colors[i])
      );
      
      ImGui::Indent(10);
      
      // 第一行：排名、ID 和 Interval
      ImGui::SetWindowFontScale(1.3f);
      ImVec2 rank_pos = ImGui::GetCursorScreenPos();
      ImGui::Text("%d", i + 1);

      // 在排名正下方显示 P (圆圈包裹 + 闭合动画)
      bool is_pitting = (i == 2); 
      bool is_in_pit = (i == 5);  
      
      if (is_pitting || is_in_pit) {
        ImGui::SetWindowFontScale(0.85f);
        float p_width = ImGui::CalcTextSize("P").x;
        float rank_width = ImGui::CalcTextSize(std::to_string(i+1).c_str()).x;
        
        // 向右微调 4 像素，避免与左侧车队颜色条重叠
        ImVec2 center = ImVec2(rank_pos.x + rank_width * 0.5f + 4.0f, rank_pos.y + 30);
        float radius = 8.0f;
        ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 0, 1));
        
        auto draw_list = ImGui::GetWindowDrawList();
        const float PI = 3.1415926535f;
        if (is_pitting) {
          // Pitting: 圆圈从开放到闭合动画 (减慢速度，从 1.5f 降至 0.8f)
          float progress = fmodf((float)ImGui::GetTime() * 0.8f, 1.0f);
          draw_list->PathArcTo(center, radius, -PI * 0.5f, -PI * 0.5f + progress * PI * 2.0f, 20);
          draw_list->PathStroke(color, false, 2.0f);
        } else {
          // In Pit: 闭合圆圈
          draw_list->AddCircle(center, radius, color, 20, 2.0f);
        }
        
        // 渲染字母 P，居中于圆圈
        ImGui::SetCursorScreenPos(ImVec2(center.x - p_width * 0.5f, center.y - 6));
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "P");
        ImGui::SetWindowFontScale(1.3f);
      }

      ImGui::SetCursorScreenPos(ImVec2(rank_pos.x + 35, rank_pos.y));
      ImGui::Text("%s", short_drivers[i]);
      ImGui::SetWindowFontScale(1.0f);
      
      ImGui::SameLine(90);
      ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", intervals[i]);
      
      // 第二行：轮胎历史 (垂直对齐：上方配方，下方圈数)
      ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5);
      ImGui::Indent(25);
      
      // 记录起始位置以便对齐
      ImVec2 start_pos = ImGui::GetCursorScreenPos();
      const float stint_width = 28.0f; // 进一步压缩固定宽度，使间距更紧凑
      
      // 渲染配方行 (S)/(M)/(H) - 使用较小字体
      ImGui::SetWindowFontScale(0.85f);
      for (size_t j = 0; j < tyre_histories[i].size(); ++j) {
        const auto& set = tyre_histories[i][j];
        ImVec4 t_color = (set.comp == 'S') ? ImVec4(1, 0, 0, 1) : 
                         (set.comp == 'M') ? ImVec4(1, 1, 0, 1) : ImVec4(1, 1, 1, 1);
        
        // 显式设置每套轮胎的起始 X 位置
        ImVec2 text_pos = ImVec2(start_pos.x + j * stint_width, start_pos.y);
        ImGui::SetCursorScreenPos(text_pos);
        
        // 渲染加粗效果：通过在微小偏移位置多次绘制来实现模拟加粗
        ImGui::TextColored(t_color, "(%c)", set.comp);
        ImGui::SetCursorScreenPos(ImVec2(text_pos.x + 0.5f, text_pos.y));
        ImGui::TextColored(t_color, "(%c)", set.comp);
        ImGui::SetCursorScreenPos(ImVec2(text_pos.x, text_pos.y + 0.2f));
        ImGui::TextColored(t_color, "(%c)", set.comp);
      }
      
      // 渲染圈数行 (垂直对齐在下方，带 L 后缀)
      for (size_t j = 0; j < tyre_histories[i].size(); ++j) {
        const auto& set = tyre_histories[i][j];
        std::string age_str = std::to_string(set.age) + "L";
        
        // 计算居中偏移：(S) 括号宽度约 22px
        float text_width = ImGui::CalcTextSize(age_str.c_str()).x;
        float center_offset = (22.0f - text_width) * 0.5f; 
        
        ImGui::SetCursorScreenPos(ImVec2(start_pos.x + j * stint_width + center_offset, start_pos.y + 11)); // 稍微拉近垂直距离
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "%s", age_str.c_str());
      }
      ImGui::SetWindowFontScale(1.0f); // 恢复正常缩放
      
      // 状态点 - 相应左移以适应更紧凑的轮胎区
      ImGui::SetCursorScreenPos(ImVec2(start_pos.x + 110, start_pos.y));
      if (i == 0) { 
        ImGui::TextColored(ImVec4(1, 0, 1, 1), "FASTEST"); 
      }
      
      // 底部微型进度条 - 调整垂直位置以匹配双行文本
      ImGui::SetCursorScreenPos(ImVec2(start_pos.x, start_pos.y + 32));
      ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1, 1, 0, 1));
      ImGui::ProgressBar(0.6f, ImVec2(160, 2), "");
      ImGui::PopStyleColor();
      
      ImGui::Unindent(25);
      ImGui::Unindent(10);
      ImGui::EndGroup();
      
      ImGui::Spacing();
      ImGui::Separator();
      ImGui::Spacing();
      
      ImGui::PopID();
    }
  } else {
    // 展开状态：显示完整标题和 Tab 系统
    ImGui::SameLine();
    ImGui::Text("TIMING TOWER");
    ImGui::Separator();

    if (ImGui::BeginTabBar("TimingTabs", ImGuiTabBarFlags_None)) {
      if (ImGui::BeginTabItem("Timing")) {
        const auto& samples = stream.Samples(0);
        if (!samples.empty()) RenderLeaderboard(&samples.back());
        ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("Tyres")) {
        if (ImGui::BeginTable("tyres_table", 3, ImGuiTableFlags_RowBg)) {
          ImGui::TableSetupColumn("Driver");
          ImGui::TableSetupColumn("Comp");
          ImGui::TableSetupColumn("Age");
          ImGui::TableHeadersRow();
          for (int i = 0; i < 2; ++i) {
            const auto& s = stream.Samples(i);
            if (s.empty()) continue;
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("%s", s.back().driver_name.c_str());
            ImGui::TableSetColumnIndex(1); ImGui::Text("%s", s.back().tire_compound.c_str());
            ImGui::TableSetColumnIndex(2); ImGui::Text("%d", s.back().tire_age);
          }
          ImGui::EndTable();
        }
        ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("Status")) {
        if (ImGui::BeginTable("status_table", 3, ImGuiTableFlags_RowBg)) {
          ImGui::TableSetupColumn("Driver");
          ImGui::TableSetupColumn("ERS");
          ImGui::TableSetupColumn("Fuel");
          ImGui::TableHeadersRow();
          for (int i = 0; i < 2; ++i) {
            const auto& s = stream.Samples(i);
            if (s.empty()) continue;
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("%s", s.back().driver_name.c_str());
            ImGui::TableSetColumnIndex(1); ImGui::Text("%.0f%%", s.back().ers_charge * 100.0f);
            ImGui::TableSetColumnIndex(2); ImGui::Text("%.1fkg", s.back().fuel_remaining);
          }
          ImGui::EndTable();
        }
        ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
    }
  }
  ImGui::EndChild();
}

static void RenderDashboard(const TelemetrySample* a, const TelemetrySample* b) {
  if (ImGui::BeginTable("dashboard_comparison", 2, ImGuiTableFlags_BordersInnerV)) {
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    if (a) {
      ImGui::TextColored(ImVec4(1, 0.84f, 0, 1), "DRIVER A: %s", a->driver_name.c_str());
      ImGui::Text("Speed: %.1f km/h", a->speed);
      ImGui::Text("Gear: %d  RPM: %.0f", a->gear, a->rpm);
      ImGui::ProgressBar(a->throttle, ImVec2(-1, 0), "THR");
      ImGui::ProgressBar(a->brake, ImVec2(-1, 0), "BRK");
      ImGui::Text("DRS: %s  ERS: %.0f%%", a->drs_active ? "ON" : (a->drs_available ? "AVAL" : "OFF"), a->ers_charge * 100.0f);
    }
    ImGui::TableSetColumnIndex(1);
    if (b) {
      ImGui::TextColored(ImVec4(0, 0.75f, 1, 1), "DRIVER B: %s", b->driver_name.c_str());
      ImGui::Text("Speed: %.1f km/h", b->speed);
      ImGui::Text("Gear: %d  RPM: %.0f", b->gear, b->rpm);
      ImGui::ProgressBar(b->throttle, ImVec2(-1, 0), "THR");
      ImGui::ProgressBar(b->brake, ImVec2(-1, 0), "BRK");
      ImGui::Text("DRS: %s  ERS: %.0f%%", b->drs_active ? "ON" : (b->drs_available ? "AVAL" : "OFF"), b->ers_charge * 100.0f);
    }
    ImGui::EndTable();
  }
}

static void RenderTelemetryPlots(const std::deque<TelemetrySample>& a_samples, const std::deque<TelemetrySample>& b_samples) {
  const float plot_height = ImGui::GetContentRegionAvail().y * 0.45f;
  
  // 1. 速度对比图
  if (ImPlot::BeginPlot("##SpeedPlot", ImVec2(-1, plot_height), ImPlotFlags_None)) {
    ImPlot::SetupAxes("Distance (m)", "Speed (km/h)", ImPlotAxisFlags_None, ImPlotAxisFlags_None);
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 350, ImGuiCond_Always);
    
    if (!a_samples.empty()) {
      static std::vector<float> dist_a, speed_a;
      dist_a.clear(); speed_a.clear();
      for (const auto& s : a_samples) {
        dist_a.push_back(s.distance);
        speed_a.push_back(s.speed);
      }
      ImPlot::SetNextLineStyle(ImVec4(1, 0.84f, 0, 1));
      ImPlot::PlotLine("VER", dist_a.data(), speed_a.data(), static_cast<int>(dist_a.size()));
    }
    
    if (!b_samples.empty()) {
      static std::vector<float> dist_b, speed_b;
      dist_b.clear(); speed_b.clear();
      for (const auto& s : b_samples) {
        dist_b.push_back(s.distance);
        speed_b.push_back(s.speed);
      }
      ImPlot::SetNextLineStyle(ImVec4(0, 0.75f, 1, 1));
      ImPlot::PlotLine("PER", dist_b.data(), speed_b.data(), static_cast<int>(dist_b.size()));
    }
    ImPlot::EndPlot();
  }

  // 2. Delta-Time 深度对比图
  if (ImPlot::BeginPlot("##DeltaPlot", ImVec2(-1, plot_height), ImPlotFlags_None)) {
    ImPlot::SetupAxes("Distance (m)", "Delta (s)", ImPlotAxisFlags_None, ImPlotAxisFlags_None);
    ImPlot::SetupAxisLimits(ImAxis_Y1, -1.0f, 1.0f, ImGuiCond_Always);
    
    if (!b_samples.empty()) {
      static std::vector<float> dist_d, delta_d;
      dist_d.clear(); delta_d.clear();
      for (const auto& s : b_samples) {
        dist_d.push_back(s.distance);
        delta_d.push_back(s.delta_to_ref);
      }
      ImPlot::SetNextLineStyle(ImVec4(1, 1, 1, 1));
      ImPlot::PlotLine("Delta (PER vs VER)", dist_d.data(), delta_d.data(), static_cast<int>(dist_d.size()));
      ImPlot::SetNextFillStyle(ImVec4(1, 1, 1, 0.1f));
      ImPlot::PlotShaded("##DeltaShade", dist_d.data(), delta_d.data(), static_cast<int>(dist_d.size()), 0);
    }
    ImPlot::EndPlot();
  }
}

static void RenderMainContent(const TelemetryStream& stream) {
  const auto& a_samples = stream.Samples(0);
  const auto& b_samples = stream.Samples(1);
  const TelemetrySample* a = a_samples.empty() ? nullptr : &a_samples.back();
  const TelemetrySample* b = b_samples.empty() ? nullptr : &b_samples.back();

  ImGui::BeginChild("MainContent", ImVec2(0, 0), false);
  const float avail_y = ImGui::GetContentRegionAvail().y;
  
  // 上半部：状态 Tab (Dashboard/Tactical Map)
  ImGui::BeginChild("StatusArea", ImVec2(0, avail_y * 0.4f), true);
  if (ImGui::BeginTabBar("MainTabs", ImGuiTabBarFlags_FittingPolicyScroll)) {
    if (ImGui::BeginTabItem("Dual Dashboards")) {
      RenderDashboard(a, b);
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Tactical Map")) {
      // 简化版全场战术地图
      if (ImPlot::BeginPlot("##TacticalMap", ImVec2(-1, -1), ImPlotFlags_Equal)) {
        ImPlot::SetupAxes("X", "Y", ImPlotAxisFlags_None, ImPlotAxisFlags_None);
        ImPlot::SetupAxisLimits(ImAxis_X1, -1500, 1500, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, -1500, 1500, ImGuiCond_Always);
        if (a) ImPlot::PlotScatter("VER", &a->x, &a->y, 1);
        if (b) ImPlot::PlotScatter("PER", &b->x, &b->y, 1);
        ImPlot::EndPlot();
      }
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
  ImGui::EndChild();
  
  // 下半部：遥测图表区
  ImGui::BeginChild("TelemetryArea", ImVec2(0, 0), true);
  RenderTelemetryPlots(a_samples, b_samples);
  ImGui::EndChild();
  
  ImGui::EndChild();
}

static void RenderStatusSidebar(const TelemetryStream& stream, float width) {
  ImGui::BeginChild("StatusSidebar", ImVec2(width, 0), true);
  
  // 1. 天气面板
  ImGui::Text("ENVIRONMENT");
  ImGui::Separator();
  if (ImGui::BeginTable("weather_table", 2)) {
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0); ImGui::Text("Track Temp:");
    ImGui::TableSetColumnIndex(1); ImGui::Text("%.1f C", stream.State().track_temp);
    
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0); ImGui::Text("Air Temp:");
    ImGui::TableSetColumnIndex(1); ImGui::Text("%.1f C", stream.State().air_temp);
    
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0); ImGui::Text("Rain Prob:");
    ImGui::TableSetColumnIndex(1); ImGui::Text("%.0f%%", stream.State().rain_probability * 100.0f);
    ImGui::EndTable();
  }
  
  ImGui::Spacing();
  ImGui::Separator();
  
  // 2. 事件日志面板
  ImGui::Text("SESSION EVENTS");
  ImGui::Separator();
  ImGui::BeginChild("EventLogScroll", ImVec2(0, 0), false);
  for (const auto& event : stream.State().events) {
    ImGui::TextColored(ImVec4(event.color[0], event.color[1], event.color[2], event.color[3]), 
                       "[%.1fs] %s", event.timestamp, event.message.c_str());
  }
  ImGui::EndChild();
  
  ImGui::EndChild();
}

static void RenderBottomBar(const TelemetryStream& stream) {
  if (stream.State().mode == SessionMode::REPLAY) {
    ImGui::BeginChild("BottomBar", ImVec2(0, 40), true);
    if (ImGui::Button(stream.State().is_playing ? "PAUSE" : "PLAY")) {
      // Toggle handled in model
    }
    ImGui::SameLine();
    ImGui::SliderFloat("Seek", (float*)&stream.State().time_remaining, 0.0f, 3600.0f);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    
    static const char* speeds[] = {"1x", "2x", "5x", "10x"};
    static int current_speed_idx = 0;
    // 使用简单的 Combo 调用，避免潜在的指针或索引越界问题
    if (ImGui::BeginCombo("Speed", speeds[current_speed_idx])) {
      for (int n = 0; n < IM_ARRAYSIZE(speeds); n++) {
        const bool is_selected = (current_speed_idx == n);
        if (ImGui::Selectable(speeds[n], is_selected)) {
          current_speed_idx = n;
        }
        if (is_selected) {
          ImGui::SetItemDefaultFocus();
        }
      }
      ImGui::EndCombo();
    }
    ImGui::EndChild();
  }
}

void RenderTelemetryDashboard(TelemetryStream& stream) {
  ImGuiIO& io = ImGui::GetIO();
  if (io.DisplaySize.x <= 0 || io.DisplaySize.y <= 0) return;

  ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
  ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
  ImGui::Begin("F1 Professional Telemetry", nullptr,
               ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);

  if (ImGui::BeginMenuBar()) {
    ImGui::Text("MODE: %s", stream.State().mode == SessionMode::LIVE ? "LIVE" : "REPLAY");
    ImGui::Separator();
    ImGui::Text("%s - %s", stream.State().track_name.c_str(), stream.State().session_type.c_str());
    ImGui::EndMenuBar();
  }

  const float timing_tower_width = stream.State().timing_tower_expanded ? io.DisplaySize.x * 0.25f : 40.0f;
  const float sidebar_width = io.DisplaySize.x * 0.20f;
  const float spacing = ImGui::GetStyle().ItemSpacing.x;
  const float main_content_width = io.DisplaySize.x - timing_tower_width - sidebar_width - (spacing * 2.0f);

  RenderTimingTower(stream, timing_tower_width);
  ImGui::SameLine();
  
  ImGui::BeginChild("MainContentGroup", ImVec2(main_content_width, 0), false);
  RenderMainContent(stream);
  ImGui::EndChild();
  
  ImGui::SameLine();
  RenderStatusSidebar(stream, sidebar_width);
  
  RenderBottomBar(stream);

  ImGui::End();
}
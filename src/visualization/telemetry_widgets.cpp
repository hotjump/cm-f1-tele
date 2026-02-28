#include "visualization/telemetry_widgets.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <limits>
#include <vector>

#include "imgui.h"
#include "implot.h"

/* 暂时注释掉未使用的格式化函数，待接入真实圈速数据时启用
static void FormatLapTime(char* buffer, size_t size, float seconds) {
  const float clamped = std::max(0.0f, seconds);
  const int minutes = static_cast<int>(clamped) / 60;
  const float remainder = clamped - minutes * 60.0f;
  std::snprintf(buffer, size, "%d:%06.3f", minutes, remainder);
}
*/

// 移除未使用的 RenderLeaderboard 函数

static void RenderTimingTower(TelemetryStream& stream, float width) {
  const bool expanded = stream.State().timing_tower_expanded;
  
  // 模拟轮胎历史数据：每套轮胎的 (配方, 圈数)
  struct TyreSet { char comp; int age; };
  static const std::array<std::vector<TyreSet>, 10> tyre_histories = {{
    {{'M', 12}, {'S', 10}}, // VER: 第一套黄12, 第二套红10
    {{'M', 15}, {'S', 8}},  // PER
    {{'S', 18}},
    {{'M', 10}, {'H', 12}}, // SAI
    {{'H', 25}},
    {{'S', 10}, {'M', 12}}, // RUS
    {{'S', 5}, {'M', 18}},  // NOR
    {{'M', 15}},
    {{'H', 30}},
    {{'S', 8}, {'S', 12}}   // GAS
  }};

  // 计算动态宽度
  float collapsed_width = 320.0f; 
  float expanded_width = 500.0f; // 展开后的总宽度
  float current_sidebar_width = expanded ? expanded_width : collapsed_width;
  
  if (!expanded) {
    size_t max_stints = 0;
    for (const auto& history : tyre_histories) {
      max_stints = std::max(max_stints, history.size());
    }
    collapsed_width = 230.0f + max_stints * 28.0f + 10.0f; 
    collapsed_width = std::max(collapsed_width, 320.0f);
    current_sidebar_width = collapsed_width;
  }

  ImGui::BeginChild("TimingTower", ImVec2(current_sidebar_width, 0), true);
  
  // 顶部折叠/展开按钮
  if (ImGui::Button(expanded ? "<< " : " >>")) {
    const_cast<SessionState&>(stream.State()).timing_tower_expanded = !expanded;
  }
  
  if (true) { // 始终显示 Timing Tower 列表，展开模式下内联显示更多信息
    ImGui::Separator();
    static const std::array<const char*, 10> short_drivers = {"VER", "PER", "LEC", "SAI", "HAM", "RUS", "NOR", "PIA", "ALO", "GAS"};
    static const std::array<const char*, 10> intervals = {"-", "+1.2s", "+5.4s", "+0.3s", "+2.1s", "+0.8s", "+1.5s", "+3.2s", "+0.9s", "+4.0s"};
    static const std::array<float, 10> mock_track_progress = {0.1f, 0.45f, 0.66f, 0.22f, 0.88f, 0.33f, 0.75f, 0.52f, 0.15f, 0.95f};

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

    for (size_t i = 0; i < short_drivers.size(); ++i) {
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
      ImGui::Text("%zu", i + 1);

      // 在排名正下方显示复合状态标识 (优先级：Flag > Pit > Track Status > DRS)
      bool is_yellow_flag = (i == 3);
      bool is_blue_flag = (i == 6);
      bool is_pitting = (i == 2); 
      bool is_in_pit = (i == 5);  
      bool is_drs_active = (i == 0); 
      bool in_drs_zone = (i == 1);   
      
      static const std::array<int, 10> mock_driver_status = {4, 1, 2, 3, 1, 4, 1, 4, 2, 0};
      int status = mock_driver_status[i];
      
      if (is_yellow_flag || is_blue_flag || is_pitting || is_in_pit || is_drs_active || in_drs_zone || (status >= 1 && status <= 3)) {
        ImGui::SetWindowFontScale(0.85f);
        
        // 确定标签与颜色 (优先级逻辑: Flag > Pit > Track Status > DRS)
        const char* label = ""; // 默认为空，防止误画
        ImVec4 color_vec = ImVec4(1, 1, 1, 1); 
        
        if (is_yellow_flag) {
          label = "Y"; color_vec = ImVec4(1, 1, 0, 1);
        } else if (is_blue_flag) {
          label = "B"; color_vec = ImVec4(0.2f, 0.4f, 1, 1);
        } else if (is_pitting || is_in_pit) {
          label = "P"; color_vec = ImVec4(1, 0.5f, 0, 1);
        } else if (is_drs_active || in_drs_zone) {
          label = "D"; color_vec = ImVec4(0, 1, 0, 1);
        } else if (status == 1) { // FLYING LAP
          label = "FLY"; color_vec = ImVec4(1, 0, 1, 1);
        } else if (status == 2) { // IN LAP
          label = ">>"; color_vec = ImVec4(1, 1, 1, 1); 
        } else if (status == 3) { // OUT LAP
          label = "<<"; color_vec = ImVec4(1, 1, 1, 1); 
        }
        
        float label_width = ImGui::CalcTextSize(label).x;
        float rank_width = ImGui::CalcTextSize(std::to_string(i+1).c_str()).x;
        ImVec2 center = ImVec2(rank_pos.x + rank_width * 0.5f + 4.0f, rank_pos.y + 30);
        float radius = 8.0f;
        auto draw_list = ImGui::GetWindowDrawList();
        const float PI = 3.1415926535f;

        if (is_yellow_flag || is_blue_flag) {
          // 渲染旗帜形状 (Flag Shape)
          float flag_w = 14.0f;
          float flag_h = 10.0f;
          ImVec2 flag_top_left = ImVec2(center.x - flag_w * 0.5f, center.y - flag_h * 0.5f);
          
          // 计算闪烁 (黄旗和蓝旗现在都高频闪烁)
          float alpha = (sinf((float)ImGui::GetTime() * 15.0f) * 0.5f) + 0.5f;
          ImU32 flag_color = ImGui::ColorConvertFloat4ToU32(ImVec4(color_vec.x, color_vec.y, color_vec.z, alpha));
          
          // 绘制旗杆
          draw_list->AddLine(ImVec2(flag_top_left.x, flag_top_left.y - 2), ImVec2(flag_top_left.x, flag_top_left.y + flag_h + 2), flag_color, 1.5f);
          // 绘制旗面 (带一点弧度模拟飘动感)
          draw_list->AddRectFilled(flag_top_left, ImVec2(flag_top_left.x + flag_w, flag_top_left.y + flag_h), flag_color, 1.0f);
          
          // 渲染字母标签 (居中于旗面)
          ImGui::SetCursorScreenPos(ImVec2(flag_top_left.x + (flag_w - label_width) * 0.5f, flag_top_left.y + (flag_h - 12) * 0.5f));
          ImGui::TextColored(ImVec4(0, 0, 0, alpha), "%s", label); 
        } else {
          // 渲染圆圈标识 (Pit / Track Status / DRS)
          float alpha = 1.0f;
          if (is_pitting || status == 1) {
            // Pit 和 Flying Lap 增加闪烁效果
            alpha = (sinf((float)ImGui::GetTime() * 10.0f) * 0.5f) + 0.5f;
          }
          ImU32 circle_color = ImGui::ColorConvertFloat4ToU32(ImVec4(color_vec.x, color_vec.y, color_vec.z, alpha));

          if (is_pitting || is_in_pit || is_drs_active || in_drs_zone) {
            // 仅对 Pit 和 DRS 状态绘制圆圈/进度条 (FLY/IN/OUT 保持纯文本/符号)
            if (is_pitting || is_in_pit) {
              draw_list->AddCircle(center, radius, circle_color, 20, 2.0f);
            } else if (in_drs_zone) {
              float drs_progress = 0.3f + 0.6f * (0.5f + 0.5f * sinf((float)ImGui::GetTime() * 2.0f));
              draw_list->PathArcTo(center, radius, -PI * 0.5f, -PI * 0.5f + drs_progress * PI * 2.0f, 20);
              draw_list->PathStroke(circle_color, false, 2.0f);
            } else if (is_drs_active) {
              draw_list->AddCircleFilled(center, radius, circle_color, 20);
            }
          }

          // 渲染字母标签
          float current_font_scale = 0.85f;
          if (status == 2 || status == 3) {
            current_font_scale = 1.1f; 
          }
          ImGui::SetWindowFontScale(current_font_scale);
          
          float current_label_width = ImGui::CalcTextSize(label).x;
          // 根据缩放调整 Y 偏移，确保文字始终居中于 10px 的图标高度内
          float y_offset = (current_font_scale > 1.0f) ? 8.0f : 6.0f;
          ImGui::SetCursorScreenPos(ImVec2(center.x - current_label_width * 0.5f, center.y - y_offset));
          
          bool use_black_text = is_drs_active;
          ImU32 text_color = use_black_text ? ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, alpha)) : circle_color;
          ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(text_color), "%s", label);
          
          ImGui::SetWindowFontScale(0.85f); // 恢复 0.85f 比例供后续可能的文本使用
        }
        ImGui::SetWindowFontScale(1.3f);
      }



      // 渲染车手名字 (扩充区域以支持全名)
      float name_max_width = 110.0f; 
      ImGui::SetCursorScreenPos(ImVec2(pos.x + 38, pos.y + 13));
      
      // 使用 ClipRect 限制名字渲染区域 (支持全名显示)
      ImGui::PushClipRect(ImVec2(pos.x + 38, pos.y + 10), ImVec2(pos.x + 38 + name_max_width, pos.y + 30), true);
      ImGui::Text("%s", short_drivers[i]);
      ImGui::PopClipRect();
      
      // 绘制垂直分割线 (进一步右移，靠近时间/轮胎区)
      auto draw_list = ImGui::GetWindowDrawList();
      ImVec2 line_start = ImVec2(pos.x + 155, pos.y + 10);
      ImVec2 line_end = ImVec2(pos.x + 155, pos.y + 32);
      draw_list->AddLine(line_start, line_end, ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 1.0f)), 1.0f);

      // 1. 间隔时间列 (Gap/Interval) - 随分割线右移
      ImGui::SetWindowFontScale(0.95f);
      static const std::array<const char*, 10> leader_gaps = {"LDR", "+1.2s", "+6.6s", "+6.9s", "+9.0s", "+9.8s", "+11.3s", "+14.5s", "+15.4s", "+19.4s"};
      
      const float gap_x = 162.0f;
      ImGui::SetCursorScreenPos(ImVec2(pos.x + gap_x, pos.y + 8)); 
      ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", intervals[i]);
      ImGui::SetCursorScreenPos(ImVec2(pos.x + gap_x, pos.y + 21));
      ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", leader_gaps[i]);
      
      // 2. 轮胎历史列 (图形化显示，位置进一步右移)
      const float tyre_start_x = pos.x + 235;
      const float stint_width = 28.0f; 
      
      for (size_t j = 0; j < tyre_histories[i].size(); ++j) {
        const auto& set = tyre_histories[i][j];
        ImVec4 t_color = (set.comp == 'S') ? ImVec4(1, 0, 0, 1) : 
                         (set.comp == 'M') ? ImVec4(1, 1, 0, 1) : ImVec4(1, 1, 1, 1);
        
        ImVec2 icon_center = ImVec2(tyre_start_x + j * stint_width + 10, pos.y + 16);
        float tyre_radius = 8.0f;
        
        draw_list->AddCircle(icon_center, tyre_radius, ImGui::ColorConvertFloat4ToU32(t_color), 20, 2.0f);
        char comp_char[2] = {set.comp, '\0'};
        float char_width = ImGui::CalcTextSize(comp_char).x;
        ImGui::SetCursorScreenPos(ImVec2(icon_center.x - char_width * 0.5f, icon_center.y - 6));
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", comp_char);
        
        std::string age_str = std::to_string(set.age) + "L";
        float text_width = ImGui::CalcTextSize(age_str.c_str()).x;
        float center_offset = (20.0f - text_width) * 0.5f; 
        ImGui::SetCursorScreenPos(ImVec2(tyre_start_x + j * stint_width + center_offset, pos.y + 25));
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "%s", age_str.c_str());
      }

      // 3. 进度条 (置于该行的底部) - 实现分段颜色 (S1/S2/S3)
      float bar_y = pos.y + 36;
      float bar_x = pos.x + 38;
      float total_width = current_sidebar_width - 50.0f;
      float seg_gap = 2.0f;
      float seg_width = (total_width - 2 * seg_gap) / 3.0f;
      float progress = mock_track_progress[i];

      // 定义扇区颜色
      ImU32 s1_color = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.8f, 1.0f, 1.0f)); // S1: 青蓝
      ImU32 s2_color = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.9f, 0.0f, 1.0f)); // S2: 黄色
      ImU32 s3_color = ImGui::ColorConvertFloat4ToU32(ImVec4(0.9f, 0.0f, 0.9f, 1.0f)); // S3: 紫色
      ImU32 bg_col = ImGui::ColorConvertFloat4ToU32(ImVec4(0.15f, 0.15f, 0.15f, 1.0f));

      for (int s = 0; s < 3; ++s) {
        float s_start_x = bar_x + s * (seg_width + seg_gap);
        float s_end_x = s_start_x + seg_width;
        
        // 计算当前扇区的填充比例
        float s_progress = 0.0f;
        float s_threshold_start = s * 0.333f;
        float s_threshold_end = (s + 1) * 0.333f;
        
        if (progress > s_threshold_end) {
          s_progress = 1.0f;
        } else if (progress > s_threshold_start) {
          s_progress = (progress - s_threshold_start) / 0.333f;
        }

        // 绘制背景
        draw_list->AddRectFilled(ImVec2(s_start_x, bar_y), ImVec2(s_end_x, bar_y + 4), bg_col, 1.0f);
        
        // 绘制进度
        if (s_progress > 0) {
          ImU32 current_col = (s == 0) ? s1_color : (s == 1) ? s2_color : s3_color;
          draw_list->AddRectFilled(ImVec2(s_start_x, bar_y), ImVec2(s_start_x + seg_width * s_progress, bar_y + 4), current_col, 1.0f);
        }
      }

      // --- 展开模式下的额外组件 (扇区条、ERS、遥测) ---
      if (expanded) {
        float x_offset = collapsed_width + 5.0f;
        float y_top = pos.y + 4.0f;
        float component_width = width - x_offset - 10.0f;

        // A. 扇区计时条 (Sector Timeline) - 更精细的样式
        float sector_y = y_top + 4.0f;
        float s_w = (component_width * 0.45f) / 3.0f - 2.0f;
        ImVec4 s_colors[3] = {ImVec4(0.8f, 0, 1, 1), ImVec4(0, 1, 0, 1), ImVec4(1, 1, 0, 1)};
        
        for (int s = 0; s < 3; ++s) {
          ImVec2 s_pos = ImVec2(x_offset + s * (s_w + 2), sector_y + 12);
          draw_list->AddRectFilled(s_pos, ImVec2(s_pos.x + s_w, s_pos.y + 5), 
                                   ImGui::ColorConvertFloat4ToU32(s_colors[s]), 1.0f);
          
          // 扇区标签
          ImGui::SetCursorScreenPos(ImVec2(s_pos.x, s_pos.y - 12));
          ImGui::SetWindowFontScale(0.7f);
          ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "S%d", s + 1);
        }

        // B. ERS & Telemetry 数据卡片
        float data_x = x_offset + component_width * 0.5f;
        ImGui::SetWindowFontScale(0.85f);
        
        // ERS 
        float ers_val = 0.65f + 0.2f * sinf((float)ImGui::GetTime() + i);
        ImGui::SetCursorScreenPos(ImVec2(data_x, y_top + 2));
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "ERS");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d%%", (int)(ers_val * 100));
        
        // 进度条背景
        draw_list->AddRectFilled(ImVec2(data_x, y_top + 16), ImVec2(data_x + 80, y_top + 20), 
                                 ImGui::ColorConvertFloat4ToU32(ImVec4(0.2f, 0.2f, 0.2f, 1.0f)));
        draw_list->AddRectFilled(ImVec2(data_x, y_top + 16), ImVec2(data_x + 80 * ers_val, y_top + 20), 
                                 ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 0, 1)));

        // Speed / RPM
        float tel_x = data_x + 95.0f;
        int speed = 280 + (int)(20 * sinf((float)ImGui::GetTime() * 2.0f + i));
        int rpm = 11500 + (int)(500 * cosf((float)ImGui::GetTime() * 1.5f + i));
        
        ImGui::SetCursorScreenPos(ImVec2(tel_x, y_top + 2));
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "%d", speed);
        ImGui::SameLine();
        ImGui::SetWindowFontScale(0.6f);
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "KMH");
        
        ImGui::SetWindowFontScale(0.85f);
        ImGui::SetCursorScreenPos(ImVec2(tel_x, y_top + 14));
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "%d", rpm);
        ImGui::SameLine();
        ImGui::SetWindowFontScale(0.6f);
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "RPM");

        ImGui::SetWindowFontScale(1.3f);
      }

      ImGui::SetWindowFontScale(1.0f);
      ImGui::Unindent(10);
      ImGui::EndGroup();
      
      ImGui::Spacing();
      ImGui::Separator();
      ImGui::Spacing();
      
      ImGui::PopID();
    }
  }
  ImGui::EndChild();
}

// 移除 RenderDashboard, RenderTacticalMap 等冗余代码，仅保留侧边栏和遥测对比图

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

  ImGui::BeginChild("MainContent", ImVec2(0, 0), false);
  
  // 遥测图表区 (全占模式)
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

  const float timing_tower_width = stream.State().timing_tower_expanded ? 550.0f : 300.0f;
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
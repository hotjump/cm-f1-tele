#include "visualization/telemetry_widgets.h"

#include "imgui.h"
#include "implot.h"

#include <algorithm>
#include <limits>
#include <cmath>
#include <vector>

static const TelemetrySample* Latest(const std::deque<TelemetrySample>& samples) {
  if (samples.empty()) {
    return nullptr;
  }
  return &samples.back();
}

static void RenderRaceSummary(const TelemetrySample* s) {
  ImGui::Text("Lap %d / %d", s->lap, s->total_laps);
  ImGui::Text("Position %d", s->position);
  ImGui::Text("Gap +%.2fs  -%.2fs", s->gap_ahead, s->gap_behind);
  ImGui::Text("Lap %.2fs  Best %.2fs", s->lap_time, s->best_lap);
  ImGui::Text("Sector %d  %.2fs", s->sector, s->sector_time);
  ImGui::Text("Delta %.2fs", s->delta_to_best);
  ImGui::ProgressBar(s->track_progress, ImVec2(-1, 0), "Track");
}

static void RenderVehicleStatus(const TelemetrySample* s) {
  ImGui::Text("Speed %.1f km/h", s->speed);
  ImGui::Text("RPM %.0f", s->rpm);
  ImGui::Text("Gear %d", s->gear);
  ImGui::Text("Throttle %.2f", s->throttle);
  ImGui::Text("Brake %.2f", s->brake);
  ImGui::Text("DRS %s", s->drs_active ? "On" : (s->drs_available ? "Available" : "Off"));
}

static void RenderEnergyPanel(const TelemetrySample* s) {
  ImGui::Text("ERS Charge");
  ImGui::ProgressBar(s->ers_charge, ImVec2(-1, 0));
  ImGui::Text("ERS Deploy");
  ImGui::ProgressBar(s->ers_deploy, ImVec2(-1, 0));
  ImGui::Text("Fuel %.1f kg", s->fuel_remaining);
  ImGui::Text("Fuel/Lap %.2f kg", s->fuel_per_lap);
}

static void RenderStrategyPanel(const TelemetrySample* s) {
  ImGui::Text("Compound %s", s->tire_compound.c_str());
  ImGui::Text("Tire Age %d", s->tire_age);
  ImGui::Text("Tire Temp %.1f C", s->tire_temp);
  ImGui::Text("Tire Pressure %.2f psi", s->tire_pressure);
  ImGui::Text("Tire Wear");
  ImGui::ProgressBar(s->tire_wear, ImVec2(-1, 0));
}

static void RenderTrendsPlot(const std::deque<TelemetrySample>& samples) {
  if (samples.empty()) {
    ImGui::Text("No data");
    return;
  }
  std::vector<double> t;
  std::vector<double> speed;
  std::vector<double> throttle;
  std::vector<double> brake;
  std::vector<double> rpm;
  t.reserve(samples.size());
  speed.reserve(samples.size());
  throttle.reserve(samples.size());
  brake.reserve(samples.size());
  rpm.reserve(samples.size());

  const double t_end = samples.back().t;
  const double window = 20.0;
  const double t_start = std::max(samples.front().t, t_end - window);
  double min_y = std::numeric_limits<double>::infinity();
  double max_y = -std::numeric_limits<double>::infinity();

  for (const auto& s : samples) {
    const double rpm_k = s.rpm / 1000.0;
    t.push_back(s.t);
    speed.push_back(s.speed);
    throttle.push_back(s.throttle);
    brake.push_back(s.brake);
    rpm.push_back(rpm_k);
    if (s.t >= t_start) {
      min_y = std::min(min_y, std::min({static_cast<double>(s.speed), static_cast<double>(s.throttle),
                                       static_cast<double>(s.brake), rpm_k}));
      max_y = std::max(max_y, std::max({static_cast<double>(s.speed), static_cast<double>(s.throttle),
                                       static_cast<double>(s.brake), rpm_k}));
    }
  }

  if (!std::isfinite(min_y) || !std::isfinite(max_y)) {
    min_y = 0.0;
    max_y = 1.0;
  }
  if (max_y - min_y < 1e-6) {
    min_y -= 1.0;
    max_y += 1.0;
  } else {
    const double pad = (max_y - min_y) * 0.08;
    min_y -= pad;
    max_y += pad;
  }

  if (ImPlot::BeginPlot("Telemetry Trends", ImVec2(-1, 0))) {
    ImPlot::SetupAxes("t (s)", "value");
    ImPlot::SetupAxisLimits(ImAxis_X1, t_start, t_end, ImGuiCond_Always);
    ImPlot::SetupAxisLimits(ImAxis_Y1, min_y, max_y, ImGuiCond_Always);
    ImPlot::PlotLine("Speed", t.data(), speed.data(), static_cast<int>(t.size()));
    ImPlot::PlotLine("Throttle", t.data(), throttle.data(), static_cast<int>(t.size()));
    ImPlot::PlotLine("Brake", t.data(), brake.data(), static_cast<int>(t.size()));
    ImPlot::PlotLine("RPM x1000", t.data(), rpm.data(), static_cast<int>(t.size()));
    ImPlot::EndPlot();
  }
}

static void RenderTrackMap(const std::deque<TelemetrySample>& samples) {
  if (samples.empty()) {
    ImGui::Text("No data");
    return;
  }
  std::vector<double> xs;
  std::vector<double> ys;
  xs.reserve(samples.size());
  ys.reserve(samples.size());
  double min_x = samples.front().x;
  double max_x = samples.front().x;
  double min_y = samples.front().y;
  double max_y = samples.front().y;
  for (const auto& s : samples) {
    xs.push_back(s.x);
    ys.push_back(s.y);
    min_x = std::min(min_x, static_cast<double>(s.x));
    max_x = std::max(max_x, static_cast<double>(s.x));
    min_y = std::min(min_y, static_cast<double>(s.y));
    max_y = std::max(max_y, static_cast<double>(s.y));
  }
  if (ImPlot::BeginPlot("Track", ImVec2(-1, 0), ImPlotFlags_Equal)) {
    ImPlot::SetupAxes("X", "Y");
    ImPlot::SetupAxisLimits(ImAxis_X1, min_x - 10.0, max_x + 10.0, ImGuiCond_Always);
    ImPlot::SetupAxisLimits(ImAxis_Y1, min_y - 10.0, max_y + 10.0, ImGuiCond_Always);
    ImPlot::PlotLine("Trajectory", xs.data(), ys.data(), static_cast<int>(xs.size()));
    ImPlot::EndPlot();
  }
}

static void RenderTelemetryTable(const std::deque<TelemetrySample>& samples) {
  const ImGuiTableFlags flags =
      ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY;
  const ImVec2 size = ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 16.0f);
  if (ImGui::BeginTable("telemetry_table", 10, flags, size)) {
    ImGui::TableSetupColumn("t");
    ImGui::TableSetupColumn("Lap");
    ImGui::TableSetupColumn("Pos");
    ImGui::TableSetupColumn("Speed");
    ImGui::TableSetupColumn("Throttle");
    ImGui::TableSetupColumn("Brake");
    ImGui::TableSetupColumn("RPM");
    ImGui::TableSetupColumn("Gear");
    ImGui::TableSetupColumn("Fuel");
    ImGui::TableSetupColumn("Tire");
    ImGui::TableHeadersRow();

    const int max_rows = 120;
    const int start_index = samples.size() > static_cast<size_t>(max_rows)
                                ? static_cast<int>(samples.size() - max_rows)
                                : 0;
    for (int i = start_index; i < static_cast<int>(samples.size()); ++i) {
      const auto& s = samples[i];
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("%.2f", s.t);
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%d/%d", s.lap, s.total_laps);
      ImGui::TableSetColumnIndex(2);
      ImGui::Text("%d", s.position);
      ImGui::TableSetColumnIndex(3);
      ImGui::Text("%.1f", s.speed);
      ImGui::TableSetColumnIndex(4);
      ImGui::Text("%.2f", s.throttle);
      ImGui::TableSetColumnIndex(5);
      ImGui::Text("%.2f", s.brake);
      ImGui::TableSetColumnIndex(6);
      ImGui::Text("%.0f", s.rpm);
      ImGui::TableSetColumnIndex(7);
      ImGui::Text("%d", s.gear);
      ImGui::TableSetColumnIndex(8);
      ImGui::Text("%.1f", s.fuel_remaining);
      ImGui::TableSetColumnIndex(9);
      ImGui::Text("%s", s.tire_compound.c_str());
    }
    ImGui::EndTable();
  }
}

void RenderTelemetryDashboard(const std::deque<TelemetrySample>& samples) {
  ImGuiIO& io = ImGui::GetIO();
  ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
  ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
  ImGui::Begin("F1 Telemetry", nullptr,
               ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoTitleBar);
  if (samples.empty()) {
    ImGui::Text("No data");
    ImGui::End();
    return;
  }
  const auto* s = Latest(samples);
  const ImVec2 spacing = ImGui::GetStyle().ItemSpacing;
  const ImVec2 avail = ImGui::GetContentRegionAvail();
  const float top_height = avail.y * 0.55f;
  const float right_width = avail.x * 0.32f;
  const float left_width = avail.x - right_width - spacing.x;

  ImGui::BeginChild("TopRow", ImVec2(0, top_height), false);
  ImGui::BeginChild("LeftTop", ImVec2(left_width, 0), true);
  ImGui::Text("Race Summary");
  ImGui::Separator();
  RenderRaceSummary(s);
  ImGui::Separator();
  ImGui::Text("Vehicle Status");
  ImGui::Separator();
  RenderVehicleStatus(s);
  ImGui::Separator();
  ImGui::Text("Energy & Fuel");
  ImGui::Separator();
  RenderEnergyPanel(s);
  ImGui::EndChild();

  ImGui::SameLine();
  ImGui::BeginChild("RightTop", ImVec2(0, 0), true);
  ImGui::Text("Tire & Strategy");
  ImGui::Separator();
  RenderStrategyPanel(s);
  ImGui::EndChild();
  ImGui::EndChild();

  ImGui::BeginChild("BottomRow", ImVec2(0, 0), true);
  if (ImGui::BeginTabBar("TelemetryTabs")) {
    if (ImGui::BeginTabItem("Trends")) {
      RenderTrendsPlot(samples);
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Track Map")) {
      RenderTrackMap(samples);
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Telemetry Table")) {
      RenderTelemetryTable(samples);
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
  ImGui::EndChild();
  ImGui::End();
}

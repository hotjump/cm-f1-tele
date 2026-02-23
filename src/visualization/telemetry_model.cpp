#include "visualization/telemetry_model.h"

#include <algorithm>
#include <cmath>

void TelemetryStream::Update(double now) {
  if (start_time_ == 0.0) {
    start_time_ = now;
    last_time_ = now;
  }
  const double t = now - start_time_;
  const double dt = now - last_time_;
  if (dt < 0.016) {
    return;
  }
  last_time_ = now;
  phase_ = t;

  const float speed = 280.0f + 40.0f * std::sin(phase_ * 1.5);
  const float throttle = 0.5f + 0.5f * std::sin(phase_ * 2.0);
  const float brake = std::max(0.0f, 0.6f - throttle);
  const float rpm = 9000.0f + 3000.0f * throttle;
  const int gear = 1 + static_cast<int>(speed / 50.0f);
  const float angle = static_cast<float>(phase_ * 0.3);
  const float radius = 120.0f + 20.0f * std::sin(phase_ * 0.7);
  const float x = radius * std::cos(angle);
  const float y = radius * std::sin(angle);
  const int total_laps = 58;
  const float lap_duration = 92.0f;
  const int lap = std::max(1, static_cast<int>(t / lap_duration) + 1);
  const int position = 1 + (lap % 6);
  const float gap_ahead = 0.4f + 0.2f * std::sin(phase_ * 0.8f);
  const float gap_behind = 0.6f + 0.25f * std::cos(phase_ * 0.6f);
  const float lap_time = static_cast<float>(std::fmod(t, lap_duration));
  const float best_lap = 89.3f;
  const int sector = static_cast<int>(std::fmod(lap_time / (lap_duration / 3.0f), 3.0f)) + 1;
  const float sector_time = lap_time - (sector - 1) * (lap_duration / 3.0f);
  const float delta_to_best = lap_time - best_lap;
  const bool drs_available = std::fmod(t, 12.0) < 8.0;
  const bool drs_active = drs_available && throttle > 0.7f;
  const float ers_charge = 0.55f + 0.35f * std::sin(phase_ * 0.35f);
  const float ers_deploy = 0.4f + 0.4f * std::cos(phase_ * 0.45f);
  const float fuel_remaining = std::max(0.0f, 110.0f - 1.8f * static_cast<float>(lap));
  const float fuel_per_lap = 1.75f + 0.1f * std::sin(phase_ * 0.2f);
  const int tire_age = lap;
  const float tire_wear = std::min(1.0f, tire_age / 35.0f);
  const float tire_temp = 85.0f + 15.0f * std::sin(phase_ * 0.5f);
  const float tire_pressure = 21.0f + 0.6f * std::cos(phase_ * 0.4f);
  const std::string tire_compound = (lap % 3 == 0) ? "Soft" : ((lap % 3 == 1) ? "Medium" : "Hard");
  const float track_progress = static_cast<float>(std::fmod(lap_time / lap_duration, 1.0f));

  samples_.push_back({t,
                      speed,
                      throttle,
                      brake,
                      rpm,
                      gear,
                      x,
                      y,
                      lap,
                      total_laps,
                      position,
                      gap_ahead,
                      gap_behind,
                      lap_time,
                      best_lap,
                      sector,
                      sector_time,
                      delta_to_best,
                      drs_available,
                      drs_active,
                      ers_charge,
                      ers_deploy,
                      fuel_remaining,
                      fuel_per_lap,
                      tire_compound,
                      tire_wear,
                      tire_temp,
                      tire_pressure,
                      tire_age,
                      track_progress});
  if (samples_.size() > max_samples_) {
    samples_.pop_front();
  }
}

const std::deque<TelemetrySample>& TelemetryStream::Samples() const {
  return samples_;
}

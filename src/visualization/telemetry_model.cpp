#include "visualization/telemetry_model.h"

#include <algorithm>
#include <cmath>
#include <cstring>

void TelemetryStream::Update(double now) {
  if (start_time_ == 0.0) {
    start_time_ = now;
    last_time_ = now;
    // 初始化模拟事件
    state_.events.push_back({0.0f, "SESSION STARTED", {0.0f, 1.0f, 0.0f, 1.0f}});
    state_.events.push_back({10.5f, "TRACK CLEAR", {0.0f, 1.0f, 0.0f, 1.0f}});
    state_.events.push_back({45.2f, "SECTOR 2: YELLOW FLAG", {1.0f, 1.0f, 0.0f, 1.0f}});
  }
  
  if (state_.mode == SessionMode::REPLAY && !state_.is_playing) {
    last_time_ = now;
    return;
  }

  const double dt = (now - last_time_) * state_.playback_speed;
  if (dt < 0.016) {
    return;
  }
  last_time_ = now;
  const double elapsed = now - start_time_;

  auto generate_driver_data = [&](int driver_id, const std::string& name, float offset_phase) {
    const float t = static_cast<float>(elapsed) + offset_phase;
    TelemetrySample s;
    s.timestamp = elapsed;
    s.driver_id = driver_id;
    s.driver_name = name;
    
    // 模拟赛道位置 (0-5000m)
    s.distance = std::fmod(t * 70.0f, 5000.0f);
    s.track_progress = s.distance / 5000.0f;
    s.lap = static_cast<int>((t * 70.0f) / 5000.0f) + 1;
    s.total_laps = 57;
    
    // 模拟遥测数据
    s.speed = 200.0f + 120.0f * std::sin(t * 0.5f);
    s.throttle = 0.5f + 0.5f * std::sin(t * 1.2f);
    s.brake = std::max(0.0f, -0.8f * std::sin(t * 1.2f));
    s.rpm = 8000.0f + 4000.0f * std::sin(t * 0.8f);
    s.gear = 1 + (static_cast<int>(s.speed) / 40) % 8;
    
    // 模拟 X, Y (简单圆圈)
    const float angle = s.track_progress * 2.0f * 3.14159f;
    s.x = 1000.0f * std::cos(angle);
    s.y = 1000.0f * std::sin(angle);
    
    // 模拟排名与差距
    s.position = (driver_id == 0) ? 1 : 2;
    s.gap_ahead = (driver_id == 0) ? 0.0f : 1.2f + 0.1f * std::sin(t * 0.2f);
    s.gap_behind = (driver_id == 0) ? 1.2f + 0.1f * std::sin(t * 0.2f) : 3.5f;
    
    s.lap_time = std::fmod(t, 90.0f);
    s.best_lap = 88.421f;
    s.sector = 1 + (static_cast<int>(s.track_progress * 3.0f) % 3);
    s.delta_to_best = -0.15f + 0.05f * std::sin(t * 0.4f);
    s.delta_to_ref = (driver_id == 0) ? 0.0f : 0.45f + 0.15f * std::sin(t * 0.3f);
    
    s.drs_available = (s.speed > 250.0f);
    s.drs_active = s.drs_available && (s.throttle > 0.95f);
    
    s.ers_charge = 0.8f - 0.2f * std::sin(t * 0.1f);
    s.ers_deploy = 0.4f + 0.4f * std::sin(t * 0.15f);
    
    s.fuel_remaining = 100.0f - (t * 0.01f);
    s.fuel_per_lap = 1.85f;
    
    s.tire_compound = (driver_id == 0) ? "SOFT" : "MEDIUM";
    s.tire_wear = std::fmod(t * 0.001f, 1.0f);
    s.tire_temp = 95.0f + 15.0f * std::sin(t * 0.6f);
    s.tire_pressure = 22.5f + 1.2f * std::sin(t * 0.6f);
    s.tire_age = s.lap - 1;

    auto& q = driver_samples_[driver_id];
    q.push_back(s);
    if (q.size() > max_samples_) {
      q.pop_front();
    }
  };

  generate_driver_data(0, "VER", 0.0f);
  generate_driver_data(1, "PER", -0.5f); // 模拟佩雷兹稍慢
}

const std::deque<TelemetrySample>& TelemetryStream::Samples(int driver_id) const {
  static const std::deque<TelemetrySample> empty;
  auto it = driver_samples_.find(driver_id);
  if (it != driver_samples_.end()) {
    return it->second;
  }
  return empty;
}

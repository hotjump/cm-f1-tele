#pragma once

#include <cstddef>
#include <deque>
#include <map>
#include <string>
#include <vector>

struct TelemetrySample {
  double timestamp;
  float distance;  // 赛道距离 (X轴基准)
  float speed;
  float throttle;
  float brake;
  float rpm;
  int gear;
  float x, y;
  int lap;
  int total_laps;
  int position;
  float gap_ahead;
  float gap_behind;
  float lap_time;
  float best_lap;
  int sector;
  float sector_time;
  float delta_to_best;
  float delta_to_ref; // 与参考车手的差距
  bool drs_available;
  bool drs_active;
  float ers_charge;
  float ers_deploy;
  float fuel_remaining;
  float fuel_per_lap;
  std::string tire_compound;
  float tire_wear;
  float tire_temp;
  float tire_pressure;
  int tire_age;
  float track_progress;
  std::string driver_name;
  int driver_id;
};

enum class SessionMode { LIVE, REPLAY };

struct EventLog {
  float timestamp;
  std::string message;
  float color[4]; // RGBA
};

struct SessionState {
  SessionMode mode = SessionMode::LIVE;
  std::string track_name = "Bahrain";
  std::string session_type = "Practice 1";
  float time_remaining = 3600.0f;
  bool is_playing = true;
  float playback_speed = 1.0f;
  int active_timing_tab = 0;
  int active_main_tab = 0;
  bool timing_tower_expanded = false; // 左侧栏展开/折叠状态
  float track_temp = 32.5f;
  float air_temp = 24.0f;
  float rain_probability = 0.05f;
  std::vector<EventLog> events;
};

class TelemetryStream {
 public:
  void Update(double now);
  const std::deque<TelemetrySample>& Samples(int driver_id) const;
  const SessionState& State() const { return state_; }
  void SetMode(SessionMode mode) { state_.mode = mode; }
  void TogglePlayback() { state_.is_playing = !state_.is_playing; }

 private:
  std::map<int, std::deque<TelemetrySample>> driver_samples_;
  SessionState state_;
  double start_time_ = 0.0;
  double last_time_ = 0.0;
  size_t max_samples_ = 1000;
};
#pragma once

#include <cstddef>
#include <deque>
#include <string>

struct TelemetrySample {
  double t;
  float speed;
  float throttle;
  float brake;
  float rpm;
  int gear;
  float x;
  float y;
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
};

class TelemetryStream {
 public:
  void Update(double now);
  const std::deque<TelemetrySample>& Samples() const;

 private:
  double start_time_ = 0.0;
  double last_time_ = 0.0;
  double phase_ = 0.0;
  size_t max_samples_ = 900;
  std::deque<TelemetrySample> samples_;
};

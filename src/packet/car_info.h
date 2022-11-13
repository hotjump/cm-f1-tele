#pragma once

#include "packet/packet.h"

#include <algorithm>
#include <array>
#include <ctime>
#include <random>

const int NUMSLICE = 200;
const int MAX_CAR_NUM = 22;

struct sliceTime {
  uint32 laptimeInMS;
  float sessionTime;

  void init() {
    laptimeInMS = 0;
    sessionTime = 0.0f;
  }
};

struct SingleLapDetail {
  uint8 lapNum;
  uint8 carPosition;
  uint32 totalTimeInMS;
  sliceTime slice[NUMSLICE];

  SingleLapDetail() { init(); }

  void init() {
    lapNum = 0;
    carPosition = 0;
    totalTimeInMS = 0;
    for (int i = 0; i < NUMSLICE; i++) {
      slice[i].init();
    }
  }
};

class carInfo {
 public:
  int carIndex;
  uint8 currentLapNum;
  uint8 carPosition;
  int sliceIndex;
  float lapDistance;
  uint8 bestLapNum;        // Lap number best time achieved on
  uint32 bestLapTimeInMS;  // Best lap time of the session in seconds
  uint32 lastLapTimeInMS;
  // car status
  uint8 drsAllowed;              // 0 = not allowed, 1 = allowed
  uint16 drsActivationDistance;  // 0 = DRS not available, non-zero - DRS will be available
                                 // in [X] metres
  uint8 tyresAgeLaps;            // Age in laps of the current set of tyres
  int8 vehicleFiaFlags;          // -1 = invalid/unknown, 0 = none, 1 = green
                                 // 2 = blue, 3 = yellow, 4 = red
  // car tele
  uint8 drs;

  // lap data
  uint8 currentLapInvalid;       // Current lap invalid - 0 = valid, 1 = invalid
  uint8 pitStatus;               // 0 = none, 1 = pitting, 2 = in pit area
  uint8 resultStatus;            // Result status - 0 = invalid, 1 = inactive, 2 = active
                                 // 3 = finished, 4 = didnotfinish, 5 = disqualified
                                 // 6 = not classified, 7 = retired
  uint8 driverStatus;            // Status of driver - 0 = in garage, 1 = flying lap
                                 // 2 = in lap, 3 = out lap, 4 = on track
  uint8 pitLaneTimerActive;      // Pit lane timing, 0 = inactive, 1 = active
  uint16 pitLaneTimeInLaneInMS;  // If active, the current time spent in the pit lane in ms

  // car damage
  uint8 frontWingDamage;

  SingleLapDetail lap[100];
  PacketSessionHistoryData history;

  // 在排位赛时，有两个diff：1. 有效成绩的对比. 2.刷圈时即时对比。JIT为just in time。
  float diffBetweenLeader;
  float diffBetweenFront;
  float diffBetweenLeaderJIT;
  float diffBetweenFrontJIT;
  float diffBetweenLastlapJIT;
  float diffBetweenBestlapJIT;

  carInfo() { init(); }

  void resetDiff() {
    diffBetweenLeader = 0.0f;
    diffBetweenFront = 0.0f;
    diffBetweenLeaderJIT = 0.0f;
    diffBetweenFrontJIT = 0.0f;
    diffBetweenLastlapJIT = 0.0f;
    diffBetweenBestlapJIT = 0.0f;
  }

  void init() {
    carIndex = -1;
    currentLapNum = 0;
    carPosition = 0;
    sliceIndex = -1;
    lapDistance = 0.0f;
    bestLapNum = 0;
    bestLapTimeInMS = 0;
    lastLapTimeInMS = 0;

    drsAllowed = 0;
    drsActivationDistance = 0;
    tyresAgeLaps = 0;
    vehicleFiaFlags = 0;

    drs = 0;

    currentLapInvalid = 0;
    pitStatus = 0;
    resultStatus = 0;
    driverStatus = 0;
    pitLaneTimerActive = 0;
    pitLaneTimeInLaneInMS = 0;

    frontWingDamage = 0;

    resetDiff();

    for (int i = 0; i < 100; i++) {
      lap[i].init();
    }
  }
};

ENUM(Scenes, in_garage, fly_lap, not_fly_lap, out_lap, in_lap, on_track, formation_lap, standing_start, battle_in_0_2s,
     battle_in_0_5s, battle_in_1s, battle_in_2s, battle_in_3s, yellow_flag, green_flag, blue_flag, read_flag,
     drs_approving, drs_activing, box, box_is_in, box_is_out, random, last_lap, chequered_flag, active, last);

#define ScenesObj(type) scenes_[static_cast<size_t>(type)]

struct FocusCar {
  const carInfo* cur_ = nullptr;
  Scenes scenes_;
  std::time_t end_unix_time_ = 0;
  FocusCar() { Init(); }
  void Init() {
    cur_ = nullptr;
    scenes_ = Scenes::last;
    end_unix_time_ = 0;
  }
  void SwitchCar(const carInfo* car, Scenes scenes, int extra_time = 1) {
    scenes_ = scenes;
    if (car) {
      cur_ = car;
    }
    end_unix_time_ = std::time(0) + extra_time;
  };

  bool CanSwitch() const { return !cur_ || std::time(0) >= end_unix_time_; }
  bool IsFlyLapSwitching(const carInfo* car) const { return cur_ && scenes_ == Scenes::fly_lap && cur_ != car; }
};

struct ScenesAndTime {
  Scenes scenes_;
  uint8 secs_;
};

class AllCarInfo {
 public:
  carInfo car_[MAX_CAR_NUM];
  PacketSessionData session_;

  int rank_num_ = 0;
  carInfo* rankList_[MAX_CAR_NUM];

  FocusCar focus_car_;
  std::array<std::vector<carInfo*>, static_cast<size_t>(Scenes::last)> scenes_;

  AllCarInfo() { init(); }

  void clearScenes() {
    for (auto& s : scenes_) {
      s.clear();
    }
  }

  void ScenesPush(Scenes type, carInfo* car) {
    if (car) {
      scenes_[static_cast<size_t>(type)].push_back(car);
    }
  }
  template <typename DISTRIBUTION>
  carInfo* Random(Scenes type, DISTRIBUTION distribution) {
    std::random_device rd;
    std::mt19937 gen(rd());
    auto size = ScenesObj(type).size();
    return size > 0 ? ScenesObj(type)[static_cast<size_t>(distribution(gen)) % size] : nullptr;
  }
  void ScenesSort(Scenes type, bool cmpFunc(const carInfo* value1, const carInfo* value2)) {
    std::sort(scenes_[static_cast<size_t>(type)].begin(), scenes_[static_cast<size_t>(type)].end(), cmpFunc);
  }
  bool ScenesIsExist(Scenes type) { return scenes_[static_cast<size_t>(type)].size() > 0; }
  void clearForLap() { clearScenes(); }
  void clearForRace() { clearScenes(); }

  void init() {
    rank_num_ = 0;
    focus_car_.Init();

    clearForLap();
    clearForRace();

    for (int i = 0; i < MAX_CAR_NUM; i++) {
      car_[i].init();
    }
  }

  void FillLapData(const PacketLapData& packet);
  void FillSession(const PacketSessionData& session) { session_ = session; }
  void FillLapHistroy(const PacketSessionHistoryData& packet);
  void FillCarStatus(const PacketCarStatusData& packet);
  void FillCarTelemetry(const PacketCarTelemetryData& packet);
  void FillCarDamage(const PacketCarDamageData& packet);
  void reCalcuteLapDiff();
  void reCalcuteRaceDiff();
  void reCalcuteRaceFocus();
  void reCalcuteLapFocus();
  void PickForLap();
  void PickForRace();

  std::string ToSQL(FuntionCommonArg, ParticipantDataArg);
};

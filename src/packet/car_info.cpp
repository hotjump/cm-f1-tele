#include "car_info.h"

#include <algorithm>
#include <chrono>
#include <random>

std::string AllCarInfo::ToSQL(FuntionCommonArg, ParticipantDataArg, TTArg) {
  if (session_.IsRace()) {
    reCalcuteRaceDiff();
    PickForRace();
  } else {
    reCalcuteLapDiff();
    PickForLap();
  }
  std::string sql;
  sql += "INSERT INTO CarDiff Values\n";
  const char* fmt = "(%u,%u,%u,NOW(),%u,'%s',%u,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f),\n";
  char stmt[512] = {0};
  carInfo* p = car_;
  for (uint8 i = 0; i < dirver_num; i++) {     
    SkipTTNonexistedParticipant;
    snprintf(stmt, sizeof(stmt), fmt, PrimaryKeyCommonPart, i + 1, driver_name[i].name().c_str(), p[i].carPosition,
             p[i].diffBetweenLeader, p[i].diffBetweenFront, p[i].diffBetweenLeaderJIT, p[i].diffBetweenFrontJIT,
             p[i].diffBetweenLastlapJIT, p[i].diffBetweenBestlapJIT);
    sql += stmt;
  }
  sql[sql.size() - 2] = ';';

  if (focus_car_.cur_) {
    const char* foucs_fmt = "INSERT INTO CarFocus Values(%u,%u,%u,NOW(),%u,'%s',%u,'%s');\n";
    int idx = focus_car_.cur_->carIndex;
    snprintf(stmt, sizeof(stmt), foucs_fmt, PrimaryKeyCommonPart, idx + 1, driver_name[idx].name().c_str(),
             focus_car_.cur_->carPosition, EnumToCStr(Scenes, focus_car_.scenes_));
    sql += stmt;
  }

  std::string detail_sql;
  const char* detail_fmt = "(%u,%u,%u,NOW(),%u,'%s',%u,%u,%u,%.2f,%.2f,%.2f,%u,%i,%u,%u,%.2f,%.2f,%.2f,%u,'%s',%u),\n";

  for (uint8 i = 0; i < dirver_num; i++) {
    for (const auto& t: p[i].tele_snapshot_buffer) {
      TimeFormat currentLapTimeInMS(t.currentLapTimeInMS);
      snprintf(stmt, sizeof(stmt) ,detail_fmt, PrimaryKeyCommonPart, i + 1, driver_name[i].name().c_str(),
              t.currentLapNum, t.lapDistance, t.speed, t.throttle, t.steer, t.brake, t.clutch, t.gear, t.engineRPM, t.drs,
              t.worldPositionX, t.worldPositionY, t.worldPositionZ, t.currentLapTimeInMS, currentLapTimeInMS.c_str(), t.ersDeployMode);
      detail_sql += stmt;
    }
    p[i].tele_snapshot_buffer.clear();
  }

  if (detail_sql.size() > 0) {
    detail_sql = "REPLACE INTO LapDetails VALUES\n" + detail_sql;
    detail_sql[detail_sql.size() - 2] = ';';
    sql += detail_sql;
  }

  return sql;
}

bool cmpLapDistanceDes(const carInfo* value1, const carInfo* value2) {
  return value1->lapDistance > value2->lapDistance;
}

bool cmpDiffBetweenFront(const carInfo* value1, const carInfo* value2) {
  return value1->diffBetweenFront < value2->diffBetweenFront;
}

bool cmpPitLaneTimeInLaneInMS(const carInfo* value1, const carInfo* value2) {
  return value1->pitLaneTimeInLaneInMS > value2->pitLaneTimeInLaneInMS;
}

bool cmpDrsActivationDistance(const carInfo* value1, const carInfo* value2) {
  return value1->drsActivationDistance < value2->drsActivationDistance;
}

void AllCarInfo::FillCarStatus(const PacketCarStatusData& packet) {
  auto p = packet.m_carStatusData;
  for (int i = 0; i < MAX_CAR_NUM; i++) {
    auto& car = car_[i];
    car.drsAllowed = p[i].m_drsAllowed;
    car.drsActivationDistance = p[i].m_drsActivationDistance;
    car.tyresAgeLaps = p[i].m_tyresAgeLaps;
    car.vehicleFiaFlags = p[i].m_vehicleFiaFlags;
    car.cur_tele_snapshot.ersDeployMode = p[i].m_ersDeployMode;
  }
}

void AllCarInfo::FillCarMotion(const PacketMotionData& packet) {
  auto p = packet.m_carMotionData;
  for (int i = 0; i < MAX_CAR_NUM; i++) {
    auto& car = car_[i];
    car.cur_tele_snapshot.worldPositionX = p[i].m_worldPositionX;
    car.cur_tele_snapshot.worldPositionY = p[i].m_worldPositionY;
    car.cur_tele_snapshot.worldPositionZ = p[i].m_worldPositionZ;
  }
}

void AllCarInfo::FillCarDamage(const PacketCarDamageData& packet) {
  auto p = packet.m_carDamageData;
  for (int i = 0; i < MAX_CAR_NUM; i++) {
    auto& car = car_[i];
    car.frontWingDamage = std::max(p[i].m_frontLeftWingDamage, p[i].m_frontRightWingDamage);
  }
}

void AllCarInfo::FillCarTelemetry(const PacketCarTelemetryData& packet) {
  auto p = packet.m_carTelemetryData;
  for (int i = 0; i < MAX_CAR_NUM; i++) {
    auto& car = car_[i];
    car.drs = p[i].m_drs;
    car.cur_tele_snapshot.speed = p[i].m_speed;
    car.cur_tele_snapshot.throttle = p[i].m_throttle;
    car.cur_tele_snapshot.steer = p[i].m_steer;
    car.cur_tele_snapshot.brake = p[i].m_brake;
    car.cur_tele_snapshot.clutch = p[i].m_clutch;
    car.cur_tele_snapshot.gear = p[i].m_gear;
    car.cur_tele_snapshot.engineRPM = p[i].m_engineRPM;
    car.cur_tele_snapshot.drs = p[i].m_drs;
  }
}

void AllCarInfo::FillLapData(const PacketLapData& packet) {
  auto p = packet.m_lapData;
  rank_num_ = 0;

  for (int i = 0; i < MAX_CAR_NUM; i++) {
    if (session_.IsTTMode() && !(i == 0 || i == packet.m_timeTrialPBCarIdx || i == packet.m_timeTrialRivalCarIdx)) {
      continue;
    }
    auto& car = car_[i];
    uint8 carPos = p[i].m_carPosition;
    if (carPos == 0) continue;

    car.cur_tele_snapshot.currentLapNum = p[i].m_currentLapNum;
    car.cur_tele_snapshot.currentLapTimeInMS = p[i].m_currentLapTimeInMS;

    auto last_lapDistance = car.cur_tele_snapshot.lapDistance;
    auto cur_lapDistance = int((p[i].m_lapDistance >= 0 ? p[i].m_lapDistance : p[i].m_lapDistance + session_.m_trackLength)/LAP_PREC) * LAP_PREC;

    if (car.cur_tele_snapshot.currentLapNum == 0 || cur_lapDistance < last_lapDistance) {
      car.cur_tele_snapshot.currentLapNum = p[i].m_currentLapNum;
    }

    if (cur_lapDistance != last_lapDistance && !is_flash_back_ &&
        car.cur_tele_snapshot.worldPositionX != 0.0f &&
        car.cur_tele_snapshot.worldPositionY != 0.0f &&
        car.cur_tele_snapshot.worldPositionZ != 0.0f) {
      car.cur_tele_snapshot.lapDistance = cur_lapDistance;
      if (session_.IsRace()) {
        if (static_cast<SafetyCarStatus>(session_.m_safetyCarStatus) != SafetyCarStatus::FORMATION_LAP) {
          car.tele_snapshot_buffer.push_back(car.cur_tele_snapshot);
        }
      } else {
        if (car.cur_tele_snapshot.currentLapTimeInMS > 0 && p[i].m_lapDistance > 0.0f &&
          (static_cast<DriversStatus>(car.driverStatus) == DriversStatus::flying_lap || 
          static_cast<ErsDeployMode>(car.cur_tele_snapshot.ersDeployMode) == ErsDeployMode::hotlap)) {
          car.tele_snapshot_buffer.push_back(car.cur_tele_snapshot);
        }
      }
    }

    is_flash_back_ = false;
  
    car.carIndex = i;
    car.carPosition = carPos;
    car.lapDistance = p[i].m_lapDistance;
    car.lastLapTimeInMS = p[i].m_lastLapTimeInMS;

    car.currentLapInvalid = p[i].m_currentLapInvalid;
    car.pitStatus = p[i].m_pitStatus;
    car.resultStatus = p[i].m_resultStatus;
    car.driverStatus = p[i].m_driverStatus;
    car.pitLaneTimerActive = p[i].m_pitLaneTimerActive;
    car.pitLaneTimeInLaneInMS = p[i].m_pitLaneTimeInLaneInMS;

    int sliceIndex = int((p[i].m_lapDistance / float(session_.m_trackLength)) * NUMSLICE) - 1;
    car.sliceIndex = sliceIndex;

    uint8 currentLapNum = p[i].m_currentLapNum;
    car.lap[currentLapNum].lapNum = currentLapNum;
    car.lap[currentLapNum].carPosition = p[i].m_carPosition;
    car.lap[currentLapNum].totalTimeInMS = p[i].m_currentLapTimeInMS;

    // TODO: lapNum始终是1，导致这里清零，导致结果不对
    if (static_cast<DriversStatus>(p[i].m_driverStatus) == DriversStatus::flying_lap && sliceIndex < 0) {
      car.lap[currentLapNum].init();
    }

    // if lap num change, fill last lap
    if (car.currentLapNum != currentLapNum && currentLapNum > 1) {
      uint8 lastLapNum = currentLapNum - 1;
      car.lap[lastLapNum].totalTimeInMS = p[i].m_lastLapTimeInMS;
      car.lap[lastLapNum].slice[NUMSLICE - 1].laptimeInMS = p[i].m_lastLapTimeInMS;
      car.lap[lastLapNum].slice[NUMSLICE - 1].sessionTime = packet.m_header.m_sessionTime;
    }

    car.currentLapNum = currentLapNum;

    // fill this lap
    if (car.lap[currentLapNum].slice[sliceIndex].laptimeInMS == 0 && sliceIndex >= 0) {
      car.lap[currentLapNum].slice[sliceIndex].laptimeInMS = p[i].m_currentLapTimeInMS;
      car.lap[currentLapNum].slice[sliceIndex].sessionTime = packet.m_header.m_sessionTime;
    }

    rankList_[carPos - 1] = &car_[i];
    rank_num_++;
  }
}

void AllCarInfo::FillLapHistroy(const PacketSessionHistoryData& packet) {
  auto carIdx = packet.m_carIdx;
  car_[carIdx].history = packet;
  const LapHistoryData* p = packet.m_lapHistoryData;
  car_[carIdx].bestLapNum = packet.m_bestLapTimeLapNum;
  if (packet.m_bestLapTimeLapNum) {
    car_[carIdx].bestLapTimeInMS = p[packet.m_bestLapTimeLapNum - 1].m_lapTimeInMS;
  }
}

float diffBetween2Car(const carInfo* car1, const carInfo* car2, uint8 lapNum, int sliceIndex) {
  return car1->lap[lapNum].slice[sliceIndex].sessionTime - car2->lap[lapNum].slice[sliceIndex].sessionTime;
}

float diffBetween2CarJIT(const carInfo* car1, uint8 lapNum1, const carInfo* car2, uint8 lapNum2, int sliceIndex) {
  float diff = 0.0f;
  if (car2->lap[lapNum2].slice[NUMSLICE - 1].laptimeInMS) {
    diff = (static_cast<int32_t>(car1->lap[lapNum1].slice[sliceIndex].laptimeInMS) -
            static_cast<int32_t>(car2->lap[lapNum2].slice[sliceIndex].laptimeInMS)) *
           0.001f;
  } else {
    auto car1_lap_num = car1->history.m_numLaps;
    auto car1_s1 = car1->history.m_lapHistoryData[car1_lap_num - 1].m_sector1TimeInMS;
    auto car1_s2 = car1->history.m_lapHistoryData[car1_lap_num - 1].m_sector2TimeInMS;
    auto car2_best_num = car2->history.m_bestLapTimeLapNum;
    auto car2_s1 = car2->history.m_lapHistoryData[car2_best_num - 1].m_sector1TimeInMS;
    auto car2_s2 = car2->history.m_lapHistoryData[car2_best_num - 1].m_sector2TimeInMS;

    if (car1_s2) {
      diff = (static_cast<int32_t>(car1_s1 + car1_s2) - static_cast<int32_t>(car2_s1 + car2_s2)) * 0.001f;
    } else if (car1_s1) {
      diff = (static_cast<int32_t>(car1_s1) - static_cast<int32_t>(car2_s1)) * 0.001f;
    }
  }
  return diff;
}

void AllCarInfo::reCalcuteLapDiff() {
  for (int i = 0; i < rank_num_; i++) {
    auto p = rankList_[i];
    if (!p) continue;

    auto currentLapNum = p->currentLapNum;
    auto sliceIndex = p->sliceIndex;
    auto bestLapNum = p->bestLapNum;

    p->resetDiff();

    if (currentLapNum > 1 && sliceIndex >= 0) {
      uint8 lastLapNum = currentLapNum - 1;
      p->diffBetweenBestlapJIT = diffBetween2CarJIT(p, currentLapNum, p, bestLapNum, sliceIndex);
      p->diffBetweenLastlapJIT = diffBetween2CarJIT(p, currentLapNum, p, lastLapNum, sliceIndex);
    }

    auto leaderCar = rankList_[0];
    auto frontCar = rankList_[i - 1];

    if (i > 0) {
      if (p->bestLapTimeInMS > 0 && leaderCar->bestLapTimeInMS > 0) {
        p->diffBetweenLeader =
            (static_cast<int32_t>(p->bestLapTimeInMS) - static_cast<int32_t>(leaderCar->bestLapTimeInMS)) * 0.001f;
      }
      if (p->bestLapTimeInMS > 0 && frontCar->bestLapTimeInMS > 0) {
        p->diffBetweenFront =
            (static_cast<int32_t>(p->bestLapTimeInMS) - static_cast<int32_t>(frontCar->bestLapTimeInMS)) * 0.001f;
      }

      if (frontCar->bestLapNum > 0 && sliceIndex >= 0) {
        p->diffBetweenFrontJIT = diffBetween2CarJIT(p, currentLapNum, frontCar, frontCar->bestLapNum, sliceIndex);
      }
    }

    if (leaderCar->bestLapNum > 0 && sliceIndex >= 0) {
      p->diffBetweenLeaderJIT = diffBetween2CarJIT(p, currentLapNum, leaderCar, leaderCar->bestLapNum, sliceIndex);
    }
  }
}

void AllCarInfo::reCalcuteRaceDiff() {
  for (int i = 0; i < rank_num_; i++) {
    auto p = rankList_[i];
    auto currentLapNum = p->currentLapNum;
    auto sliceIndex = p->sliceIndex;
    auto bestLapNum = p->bestLapNum;

    p->resetDiff();

    if (currentLapNum > 1 && sliceIndex >= 0) {
      uint8 lastLapNum = currentLapNum - 1;
      p->diffBetweenBestlapJIT = diffBetween2CarJIT(p, currentLapNum, p, bestLapNum, sliceIndex);
      p->diffBetweenLastlapJIT = diffBetween2CarJIT(p, currentLapNum, p, lastLapNum, sliceIndex);
    }

    if (i > 0) {
      auto leaderCar = rankList_[0];
      auto frontCar = rankList_[i - 1];
      uint8 lapNumForCmp = currentLapNum;
      int sliceIndexForCmp = sliceIndex;
      /*
      1. 在第一圈时，在小于NUMSLICE / 20的计时点处都使用NUMSLICE / 20的计时，在未通过前，计时差都是0.0
      2. 在未通过第一个计时点时，使用前一圈最后一个计时点
      */
      if (currentLapNum == 1) {
        sliceIndexForCmp = std::max(NUMSLICE / 20, sliceIndex);
      } else if (sliceIndex < 0) {
        lapNumForCmp = currentLapNum - 1;
        sliceIndexForCmp = NUMSLICE - 1;
      }
      auto diffBetweenLeader = diffBetween2Car(p, leaderCar, lapNumForCmp, sliceIndexForCmp);
      auto diffBetweenFront = diffBetween2Car(p, frontCar, lapNumForCmp, sliceIndexForCmp);
      p->diffBetweenLeaderJIT = p->diffBetweenLeader = std::max(0.0f, diffBetweenLeader);
      p->diffBetweenFrontJIT = p->diffBetweenFront = std::max(0.0f, diffBetweenFront);
    }
  }
}

void AllCarInfo::PickForLap() {
  clearForLap();

  for (int i = 0; i < rank_num_; i++) {
    auto car = rankList_[i];
    if (!car) continue;

    auto driver_status = car->driverStatus;

    switch (static_cast<DriversStatus>(driver_status)) {
      case DriversStatus::in_garage:
        ScenesPush(Scenes::in_garage, car);
        break;
      case DriversStatus::flying_lap:
        if (car->currentLapInvalid == 0 && car->diffBetweenLeaderJIT < 2.5f && car->diffBetweenBestlapJIT < 0.3f &&
            car->frontWingDamage < 10) {
          ScenesPush(Scenes::fly_lap, car);
        } else {
          ScenesPush(Scenes::not_fly_lap, car);
        }
        break;
      case DriversStatus::in_lap:
        ScenesPush(Scenes::in_lap, car);
        break;
      case DriversStatus::out_lap:
        ScenesPush(Scenes::out_lap, car);
        break;
      case DriversStatus::on_track:
        ScenesPush(Scenes::on_track, car);
        break;
      default:
        break;
    }
  }

  ScenesSort(Scenes::in_garage, cmpLapDistanceDes);
  ScenesSort(Scenes::fly_lap, cmpLapDistanceDes);
  ScenesSort(Scenes::in_lap, cmpLapDistanceDes);
  ScenesSort(Scenes::out_lap, cmpLapDistanceDes);
  ScenesSort(Scenes::on_track, cmpLapDistanceDes);

  reCalcuteLapFocus();
}

void AllCarInfo::reCalcuteLapFocus() {
  std::array<Scenes, 5> scenes = {Scenes::fly_lap, Scenes::out_lap, Scenes::not_fly_lap, Scenes::in_lap,
                                  Scenes::in_garage};
  if (!focus_car_.CanSwitch()) {
    return;
  }

  for (auto& s : scenes) {
    if (ScenesIsExist(s)) {
      if (focus_car_.IsFlyLapSwitching(ScenesObj(s)[0])) {
        focus_car_.SwitchCar(nullptr, Scenes::not_fly_lap, 2);
      } else {
        focus_car_.SwitchCar(ScenesObj(s)[0], s, 1);
      }
      return;
    }
  }
}

void AllCarInfo::PickForRace() {
  clearForRace();

  auto leaderCar = rankList_[0];
  auto cur_lap_num = rankList_[0]->currentLapNum;
  auto last_lap_num = session_.m_totalLaps;

  for (int i = 0; i < rank_num_; i++) {
    auto car = rankList_[i];
    if (static_cast<ResultStatus>(car->resultStatus) != ResultStatus::active) {
      continue;
    }

    ScenesPush(Scenes::active, car);

    // TODO: 考虑罚时后的实际位置
    // 如果进站时可能会出现很小的秒差，并不是在追逐
    if (i > 0 && car->pitStatus == 0) {
      if (car->diffBetweenFront <= 0.2f) {
        ScenesPush(Scenes::battle_in_0_2s, car);
      } else if (car->diffBetweenFront <= 0.5f) {
        ScenesPush(Scenes::battle_in_0_5s, car);
      } else if (car->diffBetweenFront <= 1.0f) {
        ScenesPush(Scenes::battle_in_1s, car);
      } else if (car->diffBetweenFront <= 2.0f) {
        ScenesPush(Scenes::battle_in_2s, car);
      } else if (car->diffBetweenFront <= 3.0f) {
        ScenesPush(Scenes::battle_in_3s, car);
      }
    }

    if (static_cast<VehicleFiaFlags>(car->vehicleFiaFlags) == VehicleFiaFlags::yellow) {
      ScenesPush(Scenes::yellow_flag, car);
    } else if (static_cast<VehicleFiaFlags>(car->vehicleFiaFlags) == VehicleFiaFlags::green) {
      ScenesPush(Scenes::green_flag, car);
    } else if (static_cast<VehicleFiaFlags>(car->vehicleFiaFlags) == VehicleFiaFlags::blue) {
      ScenesPush(Scenes::blue_flag, car);
    } else if (static_cast<VehicleFiaFlags>(car->vehicleFiaFlags) == VehicleFiaFlags::red) {
      ScenesPush(Scenes::read_flag, car);
    }

    // 雨天时也会有显示距离，可能还需要额外判断allow_drs
    if (car->drsActivationDistance) {
      ScenesPush(Scenes::drs_approving, car);
    }

    if (car->drs) {
      ScenesPush(Scenes::drs_activing, car);
    }

    if (car->pitStatus && car->pitLaneTimeInLaneInMS) {
      ScenesPush(Scenes::box, car);
      if (car->pitLaneTimeInLaneInMS < 3 * 1000) {
        ScenesPush(Scenes::box_is_in, car);
      } else if (car->pitLaneTimeInLaneInMS > 22 * 1000) {
        ScenesPush(Scenes::box_is_out, car);
      }
    }

    if (cur_lap_num >= last_lap_num) {
      ScenesPush(Scenes::last_lap, car);
      if (car->carPosition == ScenesObj(Scenes::active)[0]->carPosition &&
          car->sliceIndex > (NUMSLICE - NUMSLICE / 3)) {
        ScenesPush(Scenes::chequered_flag, car);
      }
    }
  }

  if (cur_lap_num == 1) {
    if (static_cast<SafetyCarStatus>(session_.m_safetyCarStatus) == SafetyCarStatus::FORMATION_LAP) {
      /*
        geometric_distribution(0.16)
        0: *********************************
        1: ****************************
        2: ***********************
        3: *******************
        4: ****************
        5: *************
        6: ***********
        7: *********
        8: ********
        9: *******
        10: *****
        11: ****
        12: ****
        13: ***
        14: **
        15: **
        16: **
        17: *
        18: *
        19: *
      */
      ScenesPush(Scenes::formation_lap, Random(Scenes::active, std::geometric_distribution<>(0.16)));
    } else if (leaderCar->sliceIndex < (NUMSLICE / 3)) {
      /* 起步时前三分之一圈，主要关注前几位，给的镜头依排名下降，使用chi_squared_distribution(2.0)，分布如下*/
      /*
        0: ******************************************************************************
        1: ************************************************
        2: *****************************
        3: *****************
        4: ***********
        5: ******
        6: ***
        7: **
        8: *
        9:
      */
      ScenesPush(Scenes::standing_start, Random(Scenes::active, std::chi_squared_distribution<double>(2.0)));
    }
  }
  /*
    std::negative_binomial_distribution<int>(4, 0.5)
    0: ************
    1: ************************
    2: *******************************
    3: *******************************
    4: ***************************
    5: *********************
    6: ****************
    7: ***********
    8: ********
    9: *****
    10: ***
    11: **
    12: *
    13:
    14:
    15:
    16:
    17:
    18:
    19:
  */
  ScenesPush(Scenes::random, Random(Scenes::active, std::negative_binomial_distribution<int>(4, 0.5)));

  ScenesSort(Scenes::drs_activing, cmpDiffBetweenFront);
  ScenesSort(Scenes::drs_approving, cmpDrsActivationDistance);
  ScenesSort(Scenes::box, cmpPitLaneTimeInLaneInMS);
  ScenesSort(Scenes::box_is_in, cmpPitLaneTimeInLaneInMS);
  ScenesSort(Scenes::box_is_out, cmpPitLaneTimeInLaneInMS);
  // 有可能存在套圈车先撞线，所以这里按圈内距离去排序，
  // 但是也可能出现第一名还没撞线，所以只在第一名撞线后才进行排序
  // 头车刚过last lap时，可能处在上一圈的车会误成为chequered_flag
  // if (ScenesIsExist(Scenes::chequered_flag) && ScenesObj(Scenes::chequered_flag)[0]->carPosition > 1) {
  //  ScenesSort(Scenes::chequered_flag, cmpLapDistanceDes);
  //}
  reCalcuteRaceFocus();
}

void AllCarInfo::reCalcuteRaceFocus() {
  if (!focus_car_.CanSwitch()) {
    return;
  }

  std::array<ScenesAndTime, 18> scenes = {{
      //暖胎圈
      {Scenes::formation_lap, 8},
      //起步
      {Scenes::standing_start, 6},
      // 冲线
      {Scenes::chequered_flag, 2},
      // 进站镜头(将出站、刚进站)
      {Scenes::box_is_out, 8},
      {Scenes::box_is_in, 3},
      // 超车镜头
      {Scenes::battle_in_0_2s, 8},
      // 各种旗语
      {Scenes::read_flag, 6},
      {Scenes::yellow_flag, 6},
      {Scenes::green_flag, 3},
      {Scenes::blue_flag, 6},
      // 开DRS
      {Scenes::drs_activing, 4},
      // 两车半秒内
      // Scenes::battle_in_0_5s,
      // 靠近DRS区
      {Scenes::drs_approving, 4},
      // 两车1S内
      {Scenes::battle_in_1s, 5},
      // 两车两秒内
      {Scenes::battle_in_2s, 4},
      // 有车进站中
      {Scenes::box, 5},
      // 冲线圈
      {Scenes::last_lap, 4},
      // 随机
      {Scenes::random, 5},
  }};

  for (auto& s : scenes) {
    if (ScenesIsExist(s.scenes_)) {
      focus_car_.SwitchCar(ScenesObj(s.scenes_)[0], s.scenes_, s.secs_);
      return;
    }
  }
}

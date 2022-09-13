#include "packet_house.h"

#define PacketData(type) packet_[static_cast<size_t>(PacketID::type)].type
#define BeginAndLastTime begin_unix_time_, last_unix_time_
#define Drivers driver_num, participants
#define TimeAndDrivers BeginAndLastTime, Drivers
#define PacketDataToSQL(type) PacketData(type).ToSQL(TimeAndDrivers);

void PacketHouse::Reset(bool include_bit) {
  if (include_bit) {
    recv_bit_.reset();
    cur_recv_bit_.reset();
  }

  event_vec_.clear();
  all_session_history_.clear();
  car_info_.init();

  session_uid_ = 0;
  begin_unix_time_ = std::time(0);
  last_unix_time_ = begin_unix_time_;
}

void PacketHouse::Store(const Packet& packet) {
  auto packet_id = packet.header.m_packetId;
  if (static_cast<PacketID>(packet_id) == PacketID::SessionData) {
    last_session_ = PacketData(SessionData);
  }

  packet_[packet_id] = packet;

  switch (static_cast<PacketID>(packet_id)) {
    case PacketID::LapData: {
      car_info_.FillLapData(PacketData(LapData));
      break;
    }
    case PacketID::EventData: {
      if (strncmp(packet.EventData.m_eventStringCode, "BUTN", 4)) {
        event_vec_.push_back(packet.EventData);
      } else {
        return;
      }
      break;
    }
    case PacketID::SessionHistory: {
      all_session_history_[packet.SessionHistory.m_carIdx] = packet.SessionHistory;
      car_info_.FillLapHistroy(packet.SessionHistory);
      break;
    }
    case PacketID::SessionData: {
      car_info_.FillSession(packet.SessionData);
      break;
    }
    default:
      break;
  }
  recv_bit_.set(packet_id, true);
  cur_recv_bit_.set(packet_id, true);
}

bool PacketHouse::IsNewSession() {
  if (!TestCurRecvPacketExist(PacketID::SessionData, true)) {
    return false;
  }

  return PacketData(SessionData).m_header.m_sessionUID != last_session_.m_header.m_sessionUID ||
         PacketData(SessionData).m_sessionType != last_session_.m_sessionType ||
         PacketData(SessionData).m_trackId != last_session_.m_trackId;
}

void PacketHouse::ClearRecvPacketExist(PacketID packet_id) { cur_recv_bit_.set(static_cast<size_t>(packet_id), false); }

bool PacketHouse::TestCurRecvPacketExist(PacketID packet_id, bool allow_session_different) const {
  return cur_recv_bit_.test(static_cast<size_t>(packet_id)) &&
         (allow_session_different || packet_[static_cast<size_t>(packet_id)].header.m_sessionUID == session_uid_);
}

bool PacketHouse::TestCurRecvPacketExistAndClear(PacketID packet_id, bool allow_session_different) {
  bool is_exist = TestCurRecvPacketExist(packet_id, allow_session_different);
  ClearRecvPacketExist(packet_id);
  return is_exist;
}

bool PacketHouse::TestRecvPacketExist(PacketID packet_id, bool allow_session_different) const {
  return recv_bit_.test(static_cast<size_t>(packet_id)) &&
         (allow_session_different || packet_[static_cast<size_t>(packet_id)].header.m_sessionUID == session_uid_);
}

bool PacketHouse::CheckPacketIsEnough() const {
  return TestRecvPacketExist(PacketID::Participants) && TestCurRecvPacketExist(PacketID::SessionData) &&
         TestCurRecvPacketExist(PacketID::CarMotion) && TestCurRecvPacketExist(PacketID::LapData) &&
         TestCurRecvPacketExist(PacketID::CarSetup) && TestCurRecvPacketExist(PacketID::CarTelemetry) &&
         TestCurRecvPacketExist(PacketID::CarStatus) && TestCurRecvPacketExist(PacketID::CarDamage);
}

bool PacketHouse::ToSQL(std::string& sql) {
  // CarMotion, SessionData, LapData, EventData, Participants, CarSetup,
  // CarTelemetry, CarStatus, FinalClassification,
  // LobbyInfo, CarDamage
  // SessionHistory
  // 其中 car_motion,SessionData,LapData,Participants,CarSetup,CarTelemetry,CarStatus,CarDamage
  // 要同频记录每一秒，并同步更新lastUnixTime在SessionList
  // EventData与beginUnixTime挂钩，并接每一秒去细分，但是不需要与上述同频
  // LobbyInfo 只记录curUnixTime，不需要与 beginUnixTime 挂钩
  // FinalClassification 和 SessionHistory与beginUnixTime挂钩

  if (cur_recv_bit_.count() == 0) {
    return false;
  }

  // 优先处理大厅消息
  if (TestCurRecvPacketExistAndClear(PacketID::LobbyInfo, true)) {
    sql = std::move(PacketData(LobbyInfo).ToSQL(std::time(0)));
    return true;
  }

  // 新session
  if (IsNewSession()) {
    Reset(false);
    auto& session = PacketData(SessionData);
    sql = session.m_header.ToSQL(BeginAndLastTime);
    session_uid_ = session.m_header.m_sessionUID;
    car_info_.FillSession(session);
    ClearRecvPacketExist(PacketID::SessionData);
    return true;
  }

  // 存在并且session id且与当前的SessionData相同
  if (!(TestRecvPacketExist(PacketID::Participants) && TestRecvPacketExist(PacketID::SessionData))) {
    return false;
  }

  auto participants = PacketData(Participants).m_participants;
  auto& session = PacketData(SessionData);

  if (TestCurRecvPacketExistAndClear(PacketID::FinalClassification)) {
    sql = PacketData(FinalClassification).ToSQL(BeginAndLastTime, participants, &session);
    return true;
  }

  if (!(std::time(0) > last_unix_time_)) {
    return false;
  }

  last_unix_time_ = std::time(0);

  if (TestCurRecvPacketExistAndClear(PacketID::SessionHistory)) {
    for (auto iter = all_session_history_.begin(); iter != all_session_history_.end(); iter++) {
      sql += iter->second.ToSQL(BeginAndLastTime, participants);
    }
  }

  if (TestCurRecvPacketExistAndClear(PacketID::EventData)) {
    for (size_t i = 0; i < event_vec_.size(); i++) {
      sql += event_vec_[i].ToSQL(BeginAndLastTime, i, participants);
    }
    event_vec_.clear();
  }

  if (CheckPacketIsEnough()) {
    auto driver_num = PacketData(Participants).DriverNum(&session);
    sql += session.ToSQL(BeginAndLastTime);
    sql += PacketData(Participants).ToSQL(BeginAndLastTime);
    sql += PacketData(CarMotion).ToSQL(TimeAndDrivers, session.m_trackId);
    sql += PacketDataToSQL(LapData);
    sql += PacketDataToSQL(CarSetup);
    sql += PacketDataToSQL(CarTelemetry);
    sql += PacketDataToSQL(CarStatus);
    sql += PacketDataToSQL(CarDamage);
    sql += car_info_.ToSQL(TimeAndDrivers);
    sql += session.m_header.AddUpdate(BeginAndLastTime);
    cur_recv_bit_.reset();
  }

  return true;
}

// 缠斗
// 追近
// 事故
// 进站和出站
// 刷新圈速
// 最快圈
// 第一圈和冲线圈
std::string PacketHouse::EventGenerator() const {
  // 缠斗，记录的是后车
  std::vector<int> battle_car;
  auto car_num = car_info_.rank_num_;
  for (int i = 1; i < car_info_.rank_num_; i++) {
    if (car_info_.car_[i].diffBetweenFront < 1.0f) {
      battle_car.push_back(car_info_.car_[i].carIndex);
    }
  }
  // 黄旗:处在黄旗区间车和收到黄旗信号的车
  std::vector<int> yellow_flag_car;
  // 蓝旗
  std::vector<int> blue_flag_car;
  // 靠近DRS区的车，越近DRS区排得越靠前
  // 开启DRS的车
  std::vector<int> drs_approving;
  std::vector<int> drs_activing;
  auto car_status = PacketData(CarStatus).m_carStatusData;
  auto car_tele = PacketData(CarTelemetry).m_carTelemetryData;
  for (int i = 0; i < car_num; i++) {
    if (static_cast<VehicleFiaFlags>(car_status[i].m_vehicleFiaFlags) == VehicleFiaFlags::yellow) {
      yellow_flag_car.push_back(i);
    } else if (static_cast<VehicleFiaFlags>(car_status[i].m_vehicleFiaFlags) == VehicleFiaFlags::blue) {
      blue_flag_car.push_back(i);
    }
    if (car_status[i].m_drsActivationDistance) {
      drs_approving.push_back(i);
    }
    if (car_tele[i].m_drs) {
      drs_activing.push_back(i);
    }
  }

  return std::string();
}
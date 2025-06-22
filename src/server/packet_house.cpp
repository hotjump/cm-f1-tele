#include "packet_house.h"

#define PacketData(type) packet_[static_cast<size_t>(PacketID::type)].type
#define IpAndTime source_ip_, begin_unix_time_, last_unix_time_
#define Drivers driver_num, participants
#define TimeAndDrivers IpAndTime, Drivers
#define TimeAndDriversAndTT TimeAndDrivers, is_tt, timeTrialPBCarIdx, timeTrialRivalCarIdx
#define PacketDataToSQL(type) PacketData(type).ToSQL(TimeAndDriversAndTT);

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
  last_recive_time_ = std::time(0);
  auto packet_id = packet.header.m_packetId;
  if (static_cast<PacketID>(packet_id) == PacketID::SessionData) {
    last_session_ = PacketData(SessionData);
  }

  packet_[packet_id] = packet;

  switch (static_cast<PacketID>(packet_id)) {
    case PacketID::LapData: {
      if (TestRecvPacketExist(PacketID::SessionData, false)) {
        car_info_.FillLapData(PacketData(LapData));
      }
      break;
    }
    case PacketID::EventData: {
      if (strncmp(packet.EventData.m_eventStringCode, "BUTN", 4)) {
        event_vec_.push_back(packet.EventData);
        if (strncmp(packet.EventData.m_eventStringCode, "FLBK", 4) == 0) {
          car_info_.SetFlashBack();
        }
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
    case PacketID::CarStatus: {
      car_info_.FillCarStatus(packet.CarStatus);
      break;
    }
    case PacketID::CarTelemetry: {
      car_info_.FillCarTelemetry(packet.CarTelemetry);
      break;
    }
    case PacketID::CarDamage: {
      car_info_.FillCarDamage(packet.CarDamage);
      break;
    }
    case PacketID::CarMotion: {
      car_info_.FillCarMotion(packet.CarMotion);
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
         PacketData(SessionData).m_trackId != last_session_.m_trackId ||
         PacketData(SessionData).m_sessionLinkIdentifier != last_session_.m_sessionLinkIdentifier;
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
  return TestRecvPacketExist(PacketID::Participants) && TestRecvPacketExist(PacketID::SessionData) &&
         TestCurRecvPacketExist(PacketID::CarMotion) && TestCurRecvPacketExist(PacketID::LapData) &&
         TestCurRecvPacketExist(PacketID::CarSetup) && TestCurRecvPacketExist(PacketID::CarTelemetry) &&
         TestCurRecvPacketExist(PacketID::CarStatus) && TestCurRecvPacketExist(PacketID::CarDamage);
}

bool PacketHouse::Handle(std::string& sql) {
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
    sql = PacketData(LobbyInfo).ToSQL(source_ip_, std::time(0));
    return false;
  }

  // 新session
  if (IsNewSession()) {
    Reset(false);
    auto& session = PacketData(SessionData);
    sql = session.m_header.ToSQL(IpAndTime);
    LOG_INFO("new session: {}", sql.c_str());
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
  auto driver_num = PacketData(Participants).DriverNum(&session);
  bool is_tt = session.IsTTMode();
  uint8 timeTrialPBCarIdx = PacketData(LapData).m_timeTrialPBCarIdx;
  uint8 timeTrialRivalCarIdx = PacketData(LapData).m_timeTrialRivalCarIdx;

  if (TestCurRecvPacketExistAndClear(PacketID::FinalClassification)) {
    sql = PacketData(FinalClassification).ToSQL(TimeAndDrivers, &session);
    return false;
  }

  if (std::time(0) > last_unix_time_) {
    last_unix_time_ = std::time(0);

    if (TestCurRecvPacketExistAndClear(PacketID::SessionHistory)) {
      for (auto iter = all_session_history_.begin(); iter != all_session_history_.end(); iter++) {
        sql += iter->second.ToSQL(TimeAndDriversAndTT);
      }
    }

    if (TestCurRecvPacketExistAndClear(PacketID::EventData)) {
      for (size_t i = 0; i < event_vec_.size(); i++) {
        sql += event_vec_[i].ToSQL(TimeAndDrivers, i);
      }
      event_vec_.clear();
    }

    if (CheckPacketIsEnough()) {
      sql += session.ToSQL(IpAndTime);
      sql += PacketData(Participants).ToSQL(IpAndTime, session.HaveRealUserName());
      sql += PacketData(CarMotion).ToSQL(TimeAndDriversAndTT, session.m_trackId);
      sql += PacketDataToSQL(LapData);
      // sql += PacketDataToSQL(CarSetup);
      sql += PacketDataToSQL(CarTelemetry);
      sql += PacketDataToSQL(CarStatus);
      sql += PacketDataToSQL(CarDamage);
      sql += car_info_.ToSQL(TimeAndDriversAndTT);
      //sql += session.m_header.AddUpdate(IpAndTime);
      cur_recv_bit_.reset();
    }
  }

  return false;
}

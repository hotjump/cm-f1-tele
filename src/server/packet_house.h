#pragma once

#include <bitset>
#include <mutex>
#include <vector>

#include "car_info.h"
#include "packet/packet.h"

class PacketHouse {
 public:
  PacketHouse(uint32_t source_ip) : source_ip_(source_ip) { Reset(true); }
  void Store(const Packet& packet);
  bool Handle(std::string& sql);
  void Reset(bool include_bit);
  bool IsNewSession();
  bool CheckPacketIsEnough() const;
  void ClearRecvPacketExist(PacketID packet_id);
  bool TestCurRecvPacketExist(PacketID packet_id, bool allow_session_different = false) const;
  bool TestCurRecvPacketExistAndClear(PacketID packet_id, bool allow_session_different = false);
  bool TestRecvPacketExist(PacketID packet_id, bool allow_session_different = false) const;

 private:
  std::bitset<static_cast<size_t>(PacketID::Count)> recv_bit_;
  std::bitset<static_cast<size_t>(PacketID::Count)> cur_recv_bit_;
  Packet packet_[static_cast<size_t>(PacketID::Count)];

  std::vector<PacketEventData> event_vec_;
  std::map<uint8, PacketSessionHistoryData> all_session_history_;
  AllCarInfo car_info_;

  uint32_t source_ip_ = 0;
  std::time_t begin_unix_time_ = 0;
  std::time_t last_unix_time_ = 0;
  uint64_t session_uid_ = 0;
  PacketSessionData last_session_;
};
#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>

#include "common/mysql_handler.h"
#include "common/udp_listener.h"
#include "packet/packet.h"
#include "packet_house.h"

class Server {
 public:
  Server() {}
  Server(std::shared_ptr<UdpListener> listener, std::shared_ptr<MysqlHandler> my)
      : listener_(listener), mysql_handler_(my) {}
  ~Server();
  void Run();
  void UnPacketAndSendToMySQL(uint32_t ip, const void* raw);
  void TimeoutOp();
  void ClearIdlePacketHouse();

 private:
  std::shared_ptr<UdpListener> listener_;
  std::shared_ptr<MysqlHandler> mysql_handler_;
  std::map<uint32_t, std::shared_ptr<PacketHouse>> packet_house_map_;
  std::set<uint32_t> filter_ip_;
};
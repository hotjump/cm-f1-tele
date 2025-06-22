#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>

#include "common/mysql_handler.h"
#include "common/sqlite_handler.h"
#include "packet/packet.h"
#include "packet_house.h"
#include "udp/udp_listener.h"

class Server {
 public:
  Server() {}
  Server(std::shared_ptr<UdpListener> listener, std::shared_ptr<MySQLHandler> my,
         std::shared_ptr<SqliteDBHandler> sqlite)
      : listener_(listener), mysql_(my), sqlite_(sqlite) {}
  ~Server();
  void Run();
  void UnPacketAndSendToMySQL(uint32_t ip, const void* raw);
  void TimeoutOp();
  void ClearIdlePacketHouse();

 private:
  std::shared_ptr<UdpListener> listener_;
  std::shared_ptr<MySQLHandler> mysql_;
  std::shared_ptr<SqliteDBHandler> sqlite_;
  std::map<uint32_t, std::shared_ptr<PacketHouse>> packet_house_map_;
  std::set<uint32_t> filter_ip_;
};
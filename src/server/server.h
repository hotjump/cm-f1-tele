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
  Server(int port, const std::string& hostname, const std::string& user, const std::string& password,
         const std::string& db, int mysql_port)
      : listener_(port, 1), mysql_handler_(hostname, user, password, db, mysql_port) {}
  ~Server();
  bool Init();
  void Stop();
  void Run();
  void UnPacketAndSendToMySQL(uint32_t ip, const Packet& packet);
  void NewSession(const PacketHeader& header);

 private:
  UdpListener listener_;
  MysqlHandler mysql_handler_;
  std::map<uint32_t, std::shared_ptr<PacketHouse>> packet_house_map_;
  std::set<uint32_t> filter_ip_;

  bool stop_ = false;
};
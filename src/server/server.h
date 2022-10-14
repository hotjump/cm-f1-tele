#pragma once
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
  void UnPacketAndSendToMySQL(const Packet& packet);
  void NewSession(const PacketHeader& header);

 private:
  UdpListener listener_;
  MysqlHandler mysql_handler_;
  PacketHouse packet_;

  bool stop_ = false;
};
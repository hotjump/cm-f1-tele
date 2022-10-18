#include "server.h"

Server::~Server() { Stop(); }

void Server::Stop() { stop_ = true; }

bool Server::Init() {
  if (!listener_.Init()) {
    return false;
  }

  if (!mysql_handler_.Init()) {
    return false;
  }

  return true;
}

void Server::Run() {
  Packet packet;

  while (true) {
    bool success = listener_.Recv(&packet.raw_data, sizeof(packet.raw_data));
    if (stop_) {
      break;
    } else if (success) {
      UnPacketAndSendToMySQL(packet);
    } else {
      // std::cout << "packet timeout" << std::endl;
    }
  }

  std::cout << "f1-2020 telemetry exit." << std::endl;
}

void Server::UnPacketAndSendToMySQL(const Packet& packet) {
  if (packet.header.m_packetFormat != 2022) {
    std::cout << "packet format is not f1 2022." << std::endl;
    return;
  }

  packet_.Store(packet);
  std::string sql;
  bool is_success = packet_.Handle(sql);

  if (is_success && sql.length()) {
    std::cout << sql << std::endl;
    mysql_handler_.Query(sql);
    sql.clear();
  }
}

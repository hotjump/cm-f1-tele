#include "server.h"

// #include <curl/curl.h>
// #include <netdb.h>
// #include <sys/socket.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <chrono>
#include <functional>

#include "common/log.h"
#include "packet/f1-2025/packet.h"

Server::~Server() {}

bool Server::Run() {
  if (listener_) {
    return listener_->Recv(
        sizeof(Packet::raw_data),
        std::bind(&Server::UnPacketAndSendToDB, this, std::placeholders::_1,
                  std::placeholders::_2),
        std::bind(&Server::TimeoutOp, this));
  }
  return false;
}

static size_t GetIpInfoCallback(void* contents, size_t size, size_t nmemb, void* userp) {
  size_t realsize = size * nmemb;
  std::string* s = static_cast<std::string*>(userp);
  if (realsize) {
    *s = std::string(static_cast<const char*>(contents), realsize);
  }
  return realsize;
}

static std::string GetIpInfo(std::string ip) {
  return "";
#if 0
  CURL* curl;
  CURLcode res;
  std::string s;
  std::string url = "https://opendata.baidu.com/api.php?query=";
  url += ip;
  url += "&co=&resource_id=6006&ie=utf8&oe=utf-8";
  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, GetIpInfoCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&s);

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);

    /* Check for errors */
    if (res != CURLE_OK) {
      // LOG_F(WARNING, "curl_easy_perform() failed: %s", curl_easy_strerror(res));
    }

    /* always cleanup */
    curl_easy_cleanup(curl);
  }
  return s;
#endif
}

static std::string IpToString(uint32_t ip) {
  struct in_addr sin_addr;
  sin_addr.s_addr = htonl(ip);
  return std::string(inet_ntoa(sin_addr));
}

void Server::UnPacketAndSendToDB(uint32_t ip, const void* raw) {
  auto ip_string = IpToString(ip);
  const Packet* packet = static_cast<const Packet*>(raw);
  uint16_t packet_format = packet->header.m_packetFormat;

  if (packet_house_map_->count(ip) == 0) {
    packet_house_map_->insert({ip, std::make_shared<PacketHouse>(ip)});
    auto ip_info = GetIpInfo(ip_string);
    LOG_INFO("New Client (Format: {}) from {}, {}, {}", packet_format, ip,
             ip_string.c_str(), ip_info.c_str());

    char stmt[1024] = {0};
    const char* fmt =
        "INSERT INTO IpList VALUES(%u,%u,now(),'%s','%s','ANONYMITIES') ON "
        "DUPLICATE KEY UPDATE "
        "updateUnixTime=%u,updateTime=now(),ipString='%s',ipComeFrom='%s';\n";
    snprintf(stmt, sizeof(stmt), fmt, ip, std::time(0), ip_string.c_str(), ip_info.c_str(), std::time(0),
             ip_string.c_str(), ip_info.c_str());
    if (mysql_) {
      mysql_->Query(stmt);
    }
    if (sqlite_) {
      sqlite_->Query(stmt);
    }
  }

  auto packet_house = (*packet_house_map_)[ip];
  ;

  if (packet_format == 2025) {
    f1_2025::PacketUnion p;
    std::memcpy(p.raw_data, raw, sizeof(p.raw_data));
    packet_house->Update2025(p);
    return;  // 2025 doesn't go to DB yet
  } else if (packet_format == 2022) {
    packet_house->Store(*packet);
  }

  std::string sql;
  bool is_new_session = packet_house->Handle(sql);
  if (is_new_session) {
    if (mysql_) {
      mysql_->Query(sql);
    }
    if (sqlite_) {
      sqlite_->Query(sql);
    }
  } else if (sql.length() > 0) {
    if (mysql_) {
      mysql_->QueryAsync(sql);
    }
    if (sqlite_) {
      sqlite_->QueryAsync(sql);
    }
  }
}

void Server::TimeoutOp() {
  if (mysql_) {
    mysql_->Ping();
  }
  if (sqlite_) {
    sqlite_->Ping();
  }
  ClearIdlePacketHouse();
}

void Server::ClearIdlePacketHouse() {
  // LOG_SCOPE_FUNCTION(1);
  for (auto it = packet_house_map_->begin(); it != packet_house_map_->end();) {
    if (it->second->TestIfIdle()) {
      auto ip = IpToString(it->second->GetSourceIp());
      // LOG_SCOPE_F(INFO, "%s is idle, delete.", ip.c_str());
      packet_house_map_->erase(it++);
    } else {
      it++;
    }
  }
  // LOG_SCOPE_F(1, "%lu packet houses left.", packet_house_map_.size());
}

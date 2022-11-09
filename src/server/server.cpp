#include "server.h"

#include <curl/curl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <chrono>

#include "loguru/loguru.hpp"

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
    auto ret = listener_.Recv(&packet.raw_data, sizeof(packet.raw_data));
    if (stop_) {
      break;
    } else if (ret.has_value()) {
      if (ret.value() > 0) {
        UnPacketAndSendToMySQL(ret.value(), packet);
      } else {
        // check there is idle packet_house in packet_house_map
        for (auto it = packet_house_map_.begin(); it != packet_house_map_.end();) {
          if (it->second->TestIfIdle()) {
            packet_house_map_.erase(it++);
          } else {
            it++;
          }
        }
      }
    } else {
      break;
    }
  }

  LOG_F(INFO, " f1-2020 telemetry exit.");
}

static size_t GetIpSourceCallback(void* contents, size_t size, size_t nmemb, void* userp) {
  size_t realsize = size * nmemb;
  std::string* s = static_cast<std::string*>(userp);
  if (realsize) {
    *s = std::string(static_cast<const char*>(contents), realsize);
  }
  return realsize;
}

static std::string GetIpSource(std::string ip) {
  CURL* curl;
  CURLcode res;
  std::string s;
  std::string url = "https://ip.useragentinfo.com/json?ip=";
  url += ip;
  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, GetIpSourceCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&s);

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);

    /* Check for errors */
    if (res != CURLE_OK) {
      LOG_F(WARNING, "curl_easy_perform() failed: %s", curl_easy_strerror(res));
    }

    /* always cleanup */
    curl_easy_cleanup(curl);
  }
  return s;
}

static std::string IpToString(uint32_t ip) {
  struct in_addr sin_addr;
  sin_addr.s_addr = htonl(ip);
  return std::string(inet_ntoa(sin_addr));
}

void Server::UnPacketAndSendToMySQL(uint32_t ip, const Packet& packet) {
  LOG_SCOPE_FUNCTION(1);
  auto ip_string = IpToString(ip);

  if (packet.header.m_packetFormat != 2022) {
    if (filter_ip_.count(ip) == 0) {
      filter_ip_.insert(ip);
      LOG_F(WARNING, "packet format is not f1 2022 from %d, %s", ip_string.c_str(), GetIpSource(ip_string).c_str());
    }
    return;
  }

  if (packet_house_map_.count(ip) == 0) {
    LOG_SCOPE_F(INFO, "New Client comes.");
    packet_house_map_.insert({ip, std::make_shared<PacketHouse>(ip)});
    auto ip_source = GetIpSource(ip_string);
    LOG_F(INFO, "From %d, %s", ip_string.c_str(), ip_source.c_str());
    char stmt[512] = {0};
    const char* fmt = "REPLACE INTO IpList VALUES(%u,%u,now(),'%s','%s');\n";
    snprintf(stmt, sizeof(stmt), fmt, ip, std::time(0), ip_string.c_str(), ip_source.c_str());
    mysql_handler_.Query(stmt);
  }

  auto packet_house = packet_house_map_[ip];
  packet_house->Store(packet);

  std::string sql;
  bool is_success = packet_house->Handle(sql);

  if (is_success && sql.length()) {
    LOG_SCOPE_F(1, "[%s]mysql query", ip_string.c_str());
    LOG_F(2, "%s", sql.c_str());
    mysql_handler_.Query(sql);
    sql.clear();
  }
}

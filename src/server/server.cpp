#include "server.h"

#include <curl/curl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <chrono>

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
      UnPacketAndSendToMySQL(ret.value(), packet);
    } else {
      // std::cout << "packet timeout" << std::endl;
    }
  }

  std::cout << "[SUCCESS]: f1-2020 telemetry exit." << std::endl;
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
      std::cout << "[ERROR]: curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
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
  auto ip_string = IpToString(ip);

  if (packet.header.m_packetFormat != 2022) {
    if (filter_ip_.count(ip) == 0) {
      filter_ip_.insert(ip);
      std::cout << "[ERROR]: packet format is not f1 2022." << std::endl;
      std::cout << "[ERROR]: From: " << ip_string << " " << GetIpSource(ip_string) << std::endl;
    }
    return;
  }

  if (packet_house_map_.count(ip) == 0) {
    packet_house_map_.insert({ip, std::make_shared<PacketHouse>(ip)});
    auto ip_source = GetIpSource(ip_string);
    std::cout << "[SUCCESS]: New client" << std::endl;
    std::cout << "[SUCCESS]: From: " << ip_string << " " << ip_source << std::endl;
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
    std::cout << "[" << ip_string << "]" << std::endl;
    std::cout << sql << std::endl;
    auto start = std::chrono::system_clock::now();
    mysql_handler_.Query(sql);
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "MySQL query cost "
              << double(duration.count()) * std::chrono::microseconds::period::num /
                     std::chrono::microseconds::period::den
              << " Seconds." << std::endl;
    sql.clear();
  }
}

#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <functional>
#include <vector>

class UdpListener {
 private:
  std::vector<int> port_;
  std::vector<int> socketfd_;
  int epfd_;

  int timeout_;

 public:
  UdpListener(std::vector<int> port, int timeout) : port_(port), timeout_(timeout) {}
  ~UdpListener();

  bool Init();
  void AddSocket(int port);
  void Recv(size_t len, std::function<void(uint32_t, const void*)> recv_cb, std::function<void()> timeout_cb);
};
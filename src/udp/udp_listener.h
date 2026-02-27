#pragma once
#include <cstdint>
#include <functional>
#include <vector>

class UdpListener {
 public:
  UdpListener(std::vector<int> ports, int timeout_sec = 1) : port_(std::move(ports)), timeout_(timeout_sec) {}

  ~UdpListener();

  bool Init();
  void AddSocket(int port);
  bool Recv(size_t len, std::function<void(uint32_t, const void*)> recv_cb,
            std::function<void()> timeout_cb);

 private:
  std::vector<int> port_;
  std::vector<int> socketfd_;
  int timeout_;  // 超时时间（秒）

#ifdef _WIN32
// Windows 特有的成员
#else
  // Linux 特有的成员
  int epfd_ = -1;  // epoll 文件描述符
#endif
};
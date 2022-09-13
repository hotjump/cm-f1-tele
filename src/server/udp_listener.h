#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

class UdpListener {
 private:
  int port_;
  int timeout_;

  int socketfd_;
  sockaddr_in serverAddress_;
  sockaddr_in clientAddress_;

 public:
  UdpListener(int port, int timeout) : port_(port), timeout_(timeout) {}
  ~UdpListener();

  bool Init();
  bool Recv(void* buf, size_t len);
};
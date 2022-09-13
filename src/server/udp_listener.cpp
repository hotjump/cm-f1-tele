#include "udp_listener.h"
#include <string.h>
#include <iostream>

UdpListener::~UdpListener() {
  close(socketfd_);
  std::cout << "[success] UDP Listener at port: " << port_ << " end." << std::endl;
}

bool UdpListener::Init() {
  if ((socketfd_ = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    std::cout << "Cannot create socket file descriptor" << std::endl;
    return false;
  }

  memset(&serverAddress_, 0, sizeof(sockaddr_in));
  memset(&clientAddress_, 0, sizeof(sockaddr_in));

  serverAddress_.sin_family = AF_INET;
  serverAddress_.sin_addr.s_addr = INADDR_ANY;
  serverAddress_.sin_port = htons(port_);

  if (bind(socketfd_, reinterpret_cast<sockaddr*>(&serverAddress_), sizeof(serverAddress_)) < 0) {
    std::cout << "Bind failed" << std::endl;
    return false;
  }

  struct timeval time_out;
  time_out.tv_sec = timeout_;
  time_out.tv_usec = 0;
  if (setsockopt(socketfd_, SOL_SOCKET, SO_RCVTIMEO, &time_out, sizeof(time_out)) < 0) {
    std::cout << "setsocketopt failed" << std::endl;
    return false;
  }

  std::cout << "[success] UDP Listener at port: " << port_ << std::endl;
  return true;
}

bool UdpListener::Recv(void* buf, size_t len) {
  long packetSize;
  socklen_t length = sizeof(clientAddress_);

  packetSize = recvfrom(socketfd_, buf, len, 0, reinterpret_cast<sockaddr*>(&clientAddress_), &length);

  if (packetSize > 0) {
    return true;
  } else if (packetSize == -1 && errno == EAGAIN) {
    return false;
  }

  return false;
}
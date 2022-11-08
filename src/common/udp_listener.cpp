#include "udp_listener.h"
#include <string.h>
#include <iostream>

#include <sys/epoll.h>

UdpListener::~UdpListener() {
  for (auto socketfd : socketfd_) {
    close(socketfd);
  }
  close(epfd_);
}

void UdpListener::AddSocket(int port) {
  int socketfd;
  if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    std::cout << "[ERROR]: Cannot create socket file descriptor for " << port << std::endl;
    return;
  }

  sockaddr_in serverAddress;
  memset(&serverAddress, 0, sizeof(sockaddr_in));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port = htons(port);

  if (bind(socketfd, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
    std::cout << "[ERROR]:Bind failed" << std::endl;
    close(socketfd);
    return;
  }

  socketfd_.emplace_back(socketfd);
  std::cout << "[SUCCESS]: UDP Listener at port: " << port << std::endl;
}

bool UdpListener::Init() {
  for (auto port : port_) {
    AddSocket(port);
  }

  if (socketfd_.size() == 0) {
    std::cout << "[ERROR]: No port bind success." << std::endl;
    return false;
  }

  epfd_ = epoll_create(10);

  for (auto fd : socketfd_) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLPRI;
    ev.data.fd = fd;

    if (epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) < 0) {
      std::cout << "[ERROR] epoll_ctl failed." << std::endl;
      return false;
    }
  }

  return true;
}

optional<uint32_t> UdpListener::Recv(void* buf, size_t len) {
  optional<uint32_t> ret;
  struct epoll_event events[1];
  auto ready = epoll_wait(epfd_, events, 1, 1000 * timeout_);
  if (ready < 0) {
    std::cout << "[ERROR]: epoll_wait fail." << std::endl;
  } else if (ready == 0) {
    // std::cout << "[ERROR] timeout" << std::endl;
  } else {
    for (int i = 0; i < ready; i++) {
      sockaddr_in clientAddress;
      socklen_t length = sizeof(clientAddress);
      memset(&clientAddress, 0, sizeof(sockaddr_in));
      int fd = events[i].data.fd;
      long packetSize = recvfrom(fd, buf, len, 0, reinterpret_cast<sockaddr*>(&clientAddress), &length);
      if (packetSize > 0) {
        uint32_t ip = ntohl(clientAddress.sin_addr.s_addr);
        ret.emplace(ip);
      }
    }
  }
  return ret;
}
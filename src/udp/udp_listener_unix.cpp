#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <memory>

#include "common/log.h"
#include "udp_listener.h"

UdpListener::~UdpListener() {
  for (auto socketfd : socketfd_) {
    close(socketfd);
    LOG_INFO("close socket fd: {}", socketfd);
  }
  close(epfd_);
}

void UdpListener::AddSocket(int port) {
  int socketfd;
  if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    LOG_ERROR("Cannot create socket file descriptor for {}", port);
    return;
  }

  sockaddr_in serverAddress;
  memset(&serverAddress, 0, sizeof(sockaddr_in));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port = htons(port);

  if (bind(socketfd, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
    // LOG_F(WARNING, "Bind failed for %d", port);
    close(socketfd);
    return;
  }

  socketfd_.emplace_back(socketfd);
  LOG_INFO("UDP Listener at port {}", port);
}

bool UdpListener::Init() {
  for (auto port : port_) {
    AddSocket(port);
  }

  if (socketfd_.size() == 0) {
    LOG_ERROR("No port bind success.");
    return false;
  }

  epfd_ = epoll_create(10);

  for (auto fd : socketfd_) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLPRI;
    ev.data.fd = fd;

    if (epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) < 0) {
      LOG_ERROR("epoll_ctl failed.");
      return false;
    }
  }

  return true;
}

void UdpListener::Recv(size_t len, std::function<void(uint32_t, const void*)> recv_cb,
                       std::function<void()> timeout_cb) {
  std::unique_ptr<char[]> buf(new char[len]);
  const int event_num = 10;
  struct epoll_event events[event_num];
  int ready = 0;

  while ((ready = epoll_wait(epfd_, events, event_num, 1000 * timeout_)) >= 0) {
    if (ready == 0) {
      timeout_cb();
    }
    for (int i = 0; i < ready; i++) {
      sockaddr_in clientAddress;
      socklen_t length = sizeof(clientAddress);
      memset(&clientAddress, 0, sizeof(sockaddr_in));
      int fd = events[i].data.fd;
      long packetSize = recvfrom(fd, buf.get(), len, 0, reinterpret_cast<sockaddr*>(&clientAddress), &length);
      if (packetSize > 0) {
        recv_cb(ntohl(clientAddress.sin_addr.s_addr), buf.get());
      }
    }
  }

  // LOG_F(WARNING, "epoll_wait failed: %s", strerror(errno));
}
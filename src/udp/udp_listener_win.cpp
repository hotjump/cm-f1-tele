#include <winsock2.h>
#include <ws2tcpip.h>

#include <memory>
#include <string>

#include "common/log.h"
#include "udp_listener.h"

// 获取 Windows 错误消息
const char* GetErrorMessage() {
  static char error_msg[1024];
  DWORD error_code = WSAGetLastError();
  FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error_msg,
                 sizeof(error_msg), NULL);
  return error_msg;
}

UdpListener::~UdpListener() {
  for (auto socketfd : socketfd_) {
    closesocket(socketfd);
    LOG_INFO("Closed socket fd: {}", socketfd);
  }
  WSACleanup();
}

bool UdpListener::Init() {
  // 初始化 Winsock
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    LOG_ERROR("WSAStartup failed: {}", GetErrorMessage());
    return false;
  }

  for (auto port : port_) {
    AddSocket(port);
  }

  if (socketfd_.empty()) {
    LOG_ERROR("No port bind success.");
    return false;
  }

  return true;
}

void UdpListener::AddSocket(int port) {
  auto socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (socketfd == INVALID_SOCKET) {
    LOG_ERROR("Cannot create socket for port {}: {}", port, GetErrorMessage());
    return;
  }

  sockaddr_in serverAddress = {0};
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port = htons(port);

  if (bind(socketfd, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
    // LOG_F(WARNING, "Bind failed for port %d: %s", port, GetErrorMessage());
    closesocket(socketfd);
    return;
  }

  socketfd_.push_back(socketfd);
  LOG_INFO("UDP Listener started on port {} (fd={})", port, socketfd);
}

void UdpListener::Recv(size_t buffer_size, std::function<void(uint32_t, const void*)> recv_cb,
                       std::function<void()> timeout_cb) {
  std::unique_ptr<char[]> buffer(new char[buffer_size]);

  while (true) {
    fd_set read_fds;
    FD_ZERO(&read_fds);

    int max_fd = 0;
    for (auto fd : socketfd_) {
      FD_SET(fd, &read_fds);
      if (fd > max_fd) max_fd = fd;
    }

    timeval timeout;
    timeout.tv_sec = timeout_;
    timeout.tv_usec = 0;

    int ready = select(max_fd + 1, &read_fds, nullptr, nullptr, &timeout);
    if (ready == 0) {
      // 超时
      timeout_cb();
      continue;
    } else if (ready < 0) {
      // LOG_F(WARNING, "select failed: %s", GetErrorMessage());
      break;
    }

    // 处理所有就绪的套接字
    for (auto fd : socketfd_) {
      if (FD_ISSET(fd, &read_fds)) {
        sockaddr_in client_address = {0};
        socklen_t client_size = sizeof(client_address);

        int bytes_received =
            recvfrom(fd, buffer.get(), buffer_size, 0, reinterpret_cast<sockaddr*>(&client_address), &client_size);

        if (bytes_received > 0) {
          recv_cb(ntohl(client_address.sin_addr.s_addr), buffer.get());
        }
      }
    }
  }
}
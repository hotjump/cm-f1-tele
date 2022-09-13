#include <csignal>
#include <iostream>
#include <memory>

#include "common/enum_macro.h"
#include "common/get_opt.h"
#include "packet/packet.h"
#include "server.h"

std::shared_ptr<Server> server;

void signalHandler(int signum) {
  if (signum == SIGINT || signum == SIGTERM || signum == SIGQUIT || signum == SIGABRT) {
    server->Stop();
  }
}

int main(int argc, char* argv[]) {
  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);
  signal(SIGQUIT, signalHandler);
  signal(SIGABRT, signalHandler);

  auto show_help = [&]() {
    std::cout << argv[0]
              << " [-h|--help|-?] [--udp-port=port] [-h=hostname|--host=hostname] [-u=root|--user=root] "
                 "[--pass=root|--password=root] [-d=f1_2022|--db=f1_2022|--database=f1_2022] "
                 "[--mysql-port=12306]"
              << std::endl;
    exit(0);
  };

  bool help = getarg(false, "-h", "--help", "-?");
  int udp_port = getarg(20778, "--udp-port");
  std::string hostname = getarg("127.0.0.1", "-h", "--host");
  std::string user = getarg("root", "-u", "--user");
  std::string password = getarg("root", "--pass", "--password");
  std::string db = getarg("f1_2022", "-d", "--db", "--database");
  int mysql_port = getarg(12306, "--mysql-port");

  if (help || argc <= 1) {
    show_help();
  }

  server = std::make_shared<Server>(udp_port, hostname, user, password, db, mysql_port);

  if (!server->Init()) {
    std::cout << "Failed to initialize server." << std::endl;
    return 1;
  }

  server->Run();

  return 0;
}
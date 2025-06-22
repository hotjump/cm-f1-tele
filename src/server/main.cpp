#include <codecvt>
#include <csignal>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <string>

#include "common/db_handler.h"
#include "common/enum_macro.h"
#include "common/get_opt.h"
#include "common/log.h"
#include "common/mysql_handler.h"
#include "common/sqlite_handler.h"
#include "packet/packet.h"
#include "server.h"
#include "spdlog/spdlog.h"
#include "udp/udp_listener.h"
#include "ui/ftx.h"

std::set<int> signal_catch = {SIGINT, SIGTERM, SIGABRT};
void signalHandler(int signum) { return; }
void signalMask() {
  for (auto sig : signal_catch) {
    signal(sig, signalHandler);
  }
}

int main(int argc, const char* argv[]) {
  signalMask();
  auto show_help = [&]() {
    std::cout << argv[0]
              << " [-h|--help|-?] [--udp-port=port] [-h=hostname|--host=hostname] [-u=root|--user=root] "
                 "[--pass=root|--password=root] [-d=f1_2022|--db=f1_2022|--database=f1_2022] "
                 "[--mysql-port=12306] [--sql-thread-num=1] [--verbosity=0]"
              << std::endl;
    exit(0);
  };

  // get opt
  struct getopt args(argc, argv);
  std::vector<int> default_udp_port({20777});
  bool help = args.getarg(false, "-h", "--help", "-?");
  auto udp_port = args.getarg(default_udp_port, "--udp-port");
  std::string hostname = args.getarg("127.0.0.1", "-h", "--host");
  std::string user = args.getarg("root", "-u", "--user");
  std::string password = args.getarg("root", "--pass", "--password");
  std::string db = args.getarg("f1_2022", "-d", "--db", "--database");
  int mysql_port = args.getarg(12306, "--mysql-port");
  size_t sql_thread_num = args.getarg(1, "--sql-thread-num");

  if (help || argc <= 1) {
    show_help();
  }

  std::stringstream ss;
  ss << argv[0] << "." << args.getarg("2077", "--udp-port") << ".log";

  // init server
  auto udp_listener = std::make_shared<UdpListener>(udp_port, 60);
  if (!udp_listener->Init()) {
    LOG_ERROR("UDP listener init failed.");
    return 1;
  }

  auto sqlite = std::make_shared<SqliteDBHandler>(sql_thread_num, db);
  if (!sqlite->Init()) {
    LOG_ERROR("sqlite3 handler init failed.");
    return 1;
  }

  auto mysql = std::make_shared<MySQLHandler>(sql_thread_num, MySQLArgs(hostname, user, password, db, mysql_port));
  if (!mysql->Init()) {
    LOG_ERROR("mysql handler init failed.");
    return 1;
  }

  FTXUI ui(mysql);
  ui.Init();
  ui.Run();

  return EXIT_SUCCESS;

  auto server = std::make_shared<Server>(udp_listener, mysql, sqlite);
  // server->Run();
  
  return 0;
}
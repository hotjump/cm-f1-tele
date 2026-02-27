#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <codecvt>
#include <csignal>
#include <functional>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <string>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl2.h"
#include "common/enum_macro.h"
#include "common/get_opt.h"
#include "common/log.h"
#include "imgui.h"
#include "implot.h"
#include "packet/f1-2025/packet.h"
#include "server.h"
#include "spdlog/spdlog.h"
#include "udp/udp_listener.h"
#include "visualization/telemetry_model.h"
#include "visualization/telemetry_widgets.h"

std::set<int> signal_catch = {SIGINT, SIGTERM, SIGABRT};
void signalHandler(int signum) { return; }
void signalMask() {
  for (auto sig : signal_catch) {
    signal(sig, signalHandler);
  }
}

int main(int argc, const char* argv[]) {
#if 0
  signalMask();
  auto show_help = [&]() {
    std::cout << argv[0]
              << " [-h|--help|-?] [--udp-port=port] "
                 "[-h=hostname|--host=hostname] [-u=root|--user=root] "
                 "[--pass=root|--password=root] "
                 "[-d=f1_2022|--db=f1_2022|--database=f1_2022] "
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
  ss << argv[0] << "." << args.getarg("20777", "--udp-port") << ".log";
  auto sqlite = std::make_shared<SqliteDBHandler>(sql_thread_num, db);
  if (!sqlite->Init()) {
    LOG_ERROR("sqlite3 handler init failed.");
    return 1;
  }

  auto duck = std::make_shared<DuckDBHandlerWrapper>(sql_thread_num, db);
  if (!duck->Init()) {
    LOG_ERROR("duckdb handler init failed.");
    return 1;
  }
#endif

  // auto mysql = std::make_shared<MySQLHandler>(
  //     sql_thread_num, MySQLArgs(hostname, user, password, db, mysql_port));
  // if (!mysql->Init()) {
  //  LOG_ERROR("mysql handler init failed.");
  //   return 1;
  //  }
  std::vector<int> default_udp_port({20777});
  // init server
  auto udp_listener = std::make_shared<UdpListener>(default_udp_port, 10);
  if (!udp_listener->Init()) {
    LOG_ERROR("UDP listener init failed.");
    // return 1;
  }

  auto packet_house_map = std::make_shared<std::map<uint32_t, std::shared_ptr<PacketHouse>>>();
  auto server = std::make_shared<Server>(udp_listener, nullptr, nullptr, packet_house_map);

  if (!glfwInit()) {
    return 1;
  }

  glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
  GLFWwindow* window = glfwCreateWindow(1280, 720, "F1 Telemetry Client", nullptr, nullptr);
  if (window == nullptr) {
    glfwTerminate();
    return 1;
  }

  glfwMaximizeWindow(window);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL2_Init();

  TelemetryStream stream;

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();

    server->Run();
    stream.Update(glfwGetTime());
    RenderTelemetryDashboard(stream);

    ImGui::Render();

    int display_w = 0;
    int display_h = 0;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  ImGui_ImplOpenGL2_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

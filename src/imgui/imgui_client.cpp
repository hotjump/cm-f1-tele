#include "imgui.h"
#include "implot.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl2.h"

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "visualization/telemetry_model.h"
#include "visualization/telemetry_widgets.h"

int main(int, char**) {
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

    stream.Update(glfwGetTime());
    RenderTelemetryDashboard(stream.Samples());

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

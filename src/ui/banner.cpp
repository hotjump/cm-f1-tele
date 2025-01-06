#include "ui/banner.h"

using namespace ftxui;

ftxui::Element Banner::Render() {
  auto data = my_->QueryData("call GetSessionInfo(0,0,0);");
  auto make_box = [](std::string header, std::string content) {
    return window(text(header) | hcenter | bold, text(content) | hcenter);
  };

  Elements columns;

  auto& headers = data.front();
  for (const auto& header : headers) {
    columns.push_back(make_box(header, ""));
  }

  auto document = hflow(columns) | border;
  return document;
}
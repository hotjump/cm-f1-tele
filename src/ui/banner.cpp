#include "ui/banner.h"

using namespace ftxui;

ftxui::Element Banner::Render() {
  // auto data = my_->QueryData("call LiveQualifying(0,0,0);");

  auto make_box = [](std::string header, std::string content) {
    return window(text(header) | hcenter | bold, text(content) | hcenter);
  };

  auto document = hbox({
                      make_box("aa", "aa"),
                      make_box("bb", "aa"),
                      make_box("cc", "aa"),
                  }) |
                  border;

  return document;
}
#include "ui/table.h"

#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>  // for Table, TableSelection
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/string.hpp>
#include <random>

ftxui::Element TableGrid::Render() {
  auto data = my_->QueryData("call LiveQualifying(0,0,0);");
  // auto table = ftxui::Table(data);

  // std::vector<std::vector<ftxui::Element>> eles;
  // std::vector<ftxui::Element> ele;
  // std::random_device rd;
  // std::default_random_engine eng(rd());
  // std::uniform_real_distribution<float> distr(0, 1);

  // ele.push_back(ftxui::gaugeUp(distr(eng)) | ftxui::color(ftxui::Color::Red) | ftxui::border);
  // ele.push_back(ftxui::gaugeDown(distr(eng)) | ftxui::color(ftxui::Color::Green) | ftxui::border);

  // eles.emplace_back(ele);
  auto table = std::make_shared<ftxui::Table>(data);
  table->SelectAll().Border(ftxui::LIGHT);
  table->SelectRow(0).Decorate(ftxui::bold);
  table->SelectRow(0).SeparatorVertical(ftxui::LIGHT);
  return table->Render();
}
#pragma once

template <typename T>
class optional {
  bool has_value_ = false;
  T val_;

 public:
  T value() { return val_; }
  void emplace(T val) {
    val_ = val;
    has_value_ = true;
  }
  bool has_value() { return has_value_; }
};
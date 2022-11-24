#pragma once

template <typename t>
const char* ToCStr(t& data, size_t k, const char* default_value = "-") {
  if (k >= data.size()) {
    return default_value;
  } else {
    return data[k];
  }
}

template <typename t>
const char* MapToCStr(const std::map<t, const char*>& data, t k, const char* default_value = "-") {
  if (0 == data.count(k)) {
    return default_value;
  } else {
    return data.at(k);
  }
}
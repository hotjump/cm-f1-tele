#pragma once

#include <stdio.h>

class TimeFormat {
 private:
  int minutes;
  int seconds;
  int milliseconds;
  char str[32];

 public:
  TimeFormat() {}

  TimeFormat(double value) {
    int tmp_value = (int)value;
    minutes = tmp_value / 60;
    seconds = tmp_value % 60;
    milliseconds = value * 1000 - tmp_value * 1000;
    if (minutes > 0)
      snprintf(str, 32, "%d:%02d.%03d", minutes, seconds, milliseconds);
    else
      snprintf(str, 32, "%d.%03d", seconds, milliseconds);
  }

  TimeFormat(float value) {
    int tmp_value = (int)value;
    minutes = tmp_value / 60;
    seconds = tmp_value % 60;
    milliseconds = value * 1000 - tmp_value * 1000;
    if (minutes > 0)
      snprintf(str, 32, "%d:%02d.%03d", minutes, seconds, milliseconds);
    else
      snprintf(str, 32, "%d.%03d", seconds, milliseconds);
  }

  TimeFormat(uint32_t value) {
    int tmp_value = (int)value;
    milliseconds = tmp_value % 1000;
    tmp_value = (tmp_value - milliseconds) / 1000;
    minutes = tmp_value / 60;
    seconds = tmp_value % 60;
    if (minutes > 0)
      snprintf(str, 32, "%d:%02d.%03d", minutes, seconds, milliseconds);
    else
      snprintf(str, 32, "%d.%03d", seconds, milliseconds);
  }

  TimeFormat(int value) {
    int tmp_value = (int)value;
    milliseconds = tmp_value % 1000;
    tmp_value = (tmp_value - milliseconds) / 1000;
    minutes = tmp_value / 60;
    seconds = tmp_value % 60;
    if (minutes > 0)
      snprintf(str, 32, "%d:%02d.%03d", minutes, seconds, milliseconds);
    else
      snprintf(str, 32, "%d.%03d", seconds, milliseconds);
  }

  const char* c_str() {
    if (minutes == 0 && seconds == 0 && milliseconds == 0) {
      return "";
    } else {
      return str;
    }
  }
};

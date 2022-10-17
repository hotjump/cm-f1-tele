#pragma once

#include <array>
#include <string>

/*
    https://stackoverflow.com/questions/5093460/how-to-convert-an-enum-type-variable-to-a-string
*/

// concatenation macros
#define CONCAT_(A, B) A##B
#define CONCAT(A, B) CONCAT_(A, B)

// generic expansion and stringification macros
#define EXPAND(X) X
#define STRINGIFY(ARG) #ARG
#define EXPANDSTRING(ARG) STRINGIFY(ARG)

// number of arguments macros
#define NUM_ARGS_(X100, X99, X98, X97, X96, X95, X94, X93, X92, X91, X90, X89, X88, X87, X86, X85, X84, X83, X82, X81, \
                  X80, X79, X78, X77, X76, X75, X74, X73, X72, X71, X70, X69, X68, X67, X66, X65, X64, X63, X62, X61,  \
                  X60, X59, X58, X57, X56, X55, X54, X53, X52, X51, X50, X49, X48, X47, X46, X45, X44, X43, X42, X41,  \
                  X40, X39, X38, X37, X36, X35, X34, X33, X32, X31, X30, X29, X28, X27, X26, X25, X24, X23, X22, X21,  \
                  X20, X19, X18, X17, X16, X15, X14, X13, X12, X11, X10, X9, X8, X7, X6, X5, X4, X3, X2, X1, N, ...)   \
  N
#define NUM_ARGS(...)                                                                                                  \
  EXPAND(NUM_ARGS_(__VA_ARGS__, 100, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80,   \
                   79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, \
                   54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
                   29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4,   \
                   3, 2, 1))

// argument extraction macros
#define FIRST_ARG(ARG, ...) ARG
#define REST_ARGS(ARG, ...) __VA_ARGS__

// arguments to strings macros
#define ARGS_STR__(N, ...) ARGS_STR_##N(__VA_ARGS__)
#define ARGS_STR_(N, ...) ARGS_STR__(N, __VA_ARGS__)
#define ARGS_STR(...) ARGS_STR_(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

#define ARGS_STR_1(ARG) EXPANDSTRING(ARG)
#define ARGS_STR_2(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_1(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_3(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_2(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_4(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_3(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_5(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_4(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_6(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_5(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_7(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_6(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_8(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_7(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_9(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_8(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_10(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_9(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_11(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_10(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_12(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_11(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_13(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_12(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_14(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_13(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_15(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_14(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_16(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_15(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_17(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_16(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_18(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_17(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_19(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_18(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_20(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_19(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_21(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_20(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_22(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_21(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_23(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_22(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_24(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_23(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_25(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_24(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_26(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_25(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_27(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_26(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_28(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_27(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_29(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_28(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_30(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_29(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_31(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_30(EXPAND(REST_ARGS(__VA_ARGS__)))
#define ARGS_STR_32(...) EXPANDSTRING(FIRST_ARG(__VA_ARGS__)), ARGS_STR_31(EXPAND(REST_ARGS(__VA_ARGS__)))

// expand until _100 or as much as you need

#define ENUM(NAME, ...)                                                                                        \
  enum class NAME { __VA_ARGS__ };                                                                             \
                                                                                                               \
  static const std::array<std::string, NUM_ARGS(__VA_ARGS__)> CONCAT(NAME, Strings) = {ARGS_STR(__VA_ARGS__)}; \
                                                                                                               \
  inline const std::string& ToString(NAME value) {                                                             \
    return CONCAT(NAME, Strings)[static_cast<std::underlying_type<NAME>::type>(value)];                        \
  }                                                                                                            \
                                                                                                               \
  inline std::ostream& operator<<(std::ostream& os, NAME value) {                                              \
    os << ToString(value);                                                                                     \
    return os;                                                                                                 \
  }

#define EnumToCStr(Name, value)                                                                 \
  (static_cast<int>(value) == -1 || static_cast<size_t>(value) >= CONCAT(Name, Strings).size()) \
      ? "invalid"                                                                               \
      : ToString(static_cast<Name>(value)).c_str()

#define EnumToString(Name, value)                                                               \
  (static_cast<int>(value) == -1 || static_cast<size_t>(value) >= CONCAT(Name, Strings).size()) \
      ? std::string("invalid")                                                                  \
      : ToString(static_cast<Name>(value))

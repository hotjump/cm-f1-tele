# CMake最低版本号要求
cmake_minimum_required (VERSION 3.5)

# project variables
MESSAGE(STATUS "platform: ${CMAKE_SYSTEM_NAME}")
if (UNIX AND NOT APPLE)
    MESSAGE(STATUS "unix")
elseif (WIN32)
    MESSAGE(STATUS "windows")
elseif (APPLE)
    MESSAGE(STATUS "mac os")
else ()
    MESSAGE(STATUS "other platform")
endif ()

# 项目信息
project (F1-2022-TELE)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

SET(CMAKE_CXX_FLAGS "-std=c++17 -g -ggdb -Wno-deprecated-declarations ${CMAKE_CXX_FLAGS}")

include(FetchContent)
# --- Fetch FTXUI --------------------------------------------------------------

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v5.0.0
)

set(FETCHCONTENT_QUIET OFF)  # 可选：显示下载进度

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# 声明 spdlog 依赖
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY git@github.com:gabime/spdlog.git
    GIT_TAG v1.12.0  # 指定稳定版本，可根据需求更改
)

# 使能系统模式以避免 spdlog 的警告污染项目
set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "" FORCE)
set(FETCHCONTENT_QUIET OFF)  # 可选：显示下载进度

# 自动下载并构建 spdlog
FetchContent_MakeAvailable(spdlog)

# 指定本地下载的 ZIP 文件路径
set(FETCHCONTENT_ARCHIVE_DIR ${PROJECT_SOURCE_DIR}/third-party CACHE PATH "")
message(STATUS "Local SQLite3 directory: ${FETCHCONTENT_ARCHIVE_DIR}")

FetchContent_Declare(
    sqlite3
    URL ""  # 留空，禁用网络下载
    DOWNLOAD_COMMAND ""  # 禁用下载命令
    SOURCE_DIR "${FETCHCONTENT_ARCHIVE_DIR}/sqlite-amalgamation-3500000"  # 解压后的目录名
)

FetchContent_MakeAvailable(sqlite3)

# 创建 SQLite3 库目标
add_library(sqlite3 SHARED
    ${sqlite3_SOURCE_DIR}/sqlite3.c
    ${sqlite3_SOURCE_DIR}/sqlite3.h
    ${sqlite3_SOURCE_DIR}/sqlite3ext.h
)

# 添加编译选项
target_compile_definitions(sqlite3 PRIVATE
    SQLITE_ENABLE_RTREE=1
    SQLITE_ENABLE_FTS3=1
    # 添加其他需要的编译选项
)

aux_source_directory(. SRC_LIST)
aux_source_directory(./src SRC_LIST)
aux_source_directory(./src/autocam SRC_LIST)
aux_source_directory(./src/packet SRC_LIST)
aux_source_directory(./src/server SRC_LIST)
aux_source_directory(./src/common SRC_LIST)
aux_source_directory(./src/ui SRC_LIST)
# aux_source_directory(./third-party/loguru SRC_LIST)

aux_source_directory(./src/distribution SRC_DIS)

# --- lib --------------------------------------------------------------
if (APPLE)
    link_directories(/usr/local/opt/mysql-client/lib)
else (UNIX)
    link_directories(/usr/lib64/mysql /usr/lib64)
endif ()
    #link_directories(${PROJECT_SOURCE_DIR}/third-party/sqlite/lib)
    link_directories(${PROJECT_SOURCE_DIR}/third-party/mysql-connector-c/lib)

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/autocam
    ${PROJECT_SOURCE_DIR}/src/packet
    ${PROJECT_SOURCE_DIR}/src/server
    ${PROJECT_SOURCE_DIR}/src/common
    ${PROJECT_SOURCE_DIR}/src/ui
    ${PROJECT_SOURCE_DIR}/third-party
    ${PROJECT_SOURCE_DIR}/third-party/CircularBuffer
    ${PROJECT_SOURCE_DIR}/third-party/sqlite/include
    ${PROJECT_SOURCE_DIR}/_deps/ftxui-src/include

if (APPLE)
    /usr/local/opt/mysql-client/include/mysql
else (UNIX)
    /usr/include/mysql
endif ()
    ${PROJECT_SOURCE_DIR}/third-party/mysql-connector-c/include
)

# 指定生成目标
if(WIN32)
    # Windows 平台
    add_executable(f1-2022-tele-dev  ${SRC_LIST} ./src/udp/udp_listener_win.cpp)
else()
    # Linux/macOS 平台
    add_executable(f1-2022-tele-dev  ${SRC_LIST} ./src/udp/udp_listener_unix.cpp)
endif()

add_executable(distribution-test  ${SRC_DIS})

target_link_libraries(f1-2022-tele-dev 
PRIVATE ftxui::screen
PRIVATE ftxui::dom
PRIVATE ftxui::component)

if (WIN32)
    target_link_libraries(f1-2022-tele-dev LINK_PUBLIC stdc++fs dl pthread sqlite3 spdlog::spdlog mysql ws2_32)
elseif (UNIX)
    target_link_libraries(f1-2022-tele-dev LINK_PUBLIC stdc++fs dl pthread sqlite3 spdlog::spdlog mysqlclient)
endif()

# copy sql file to bin path
set(sql_path ${EXECUTABLE_OUTPUT_PATH}/sql)
file(GLOB SQL_FILES ${PROJECT_SOURCE_DIR}/src/packet/*.sql)
execute_process( COMMAND ${CMAKE_COMMAND} -E remove_directory ${sql_path})
execute_process( COMMAND ${CMAKE_COMMAND} -E make_directory ${sql_path})
execute_process( COMMAND ${CMAKE_COMMAND} -E copy ${SQL_FILES} ${sql_path})

if (WIN32)
    execute_process( COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/third-party/mysql-connector-c/lib/libmysql.dll ${EXECUTABLE_OUTPUT_PATH})
endif()


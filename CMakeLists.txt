# CMake最低版本号要求
cmake_minimum_required (VERSION 2.6)

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

# 加载子目录
add_subdirectory(src)

aux_source_directory(. SRC_LIST)
aux_source_directory(./src SRC_LIST)
aux_source_directory(./src/autocam SRC_LIST)
aux_source_directory(./src/packet SRC_LIST)
aux_source_directory(./src/server SRC_LIST)
aux_source_directory(./src/common SRC_LIST)
aux_source_directory(./src/ui SRC_LIST)
aux_source_directory(./third-party/loguru SRC_LIST)

aux_source_directory(./src/distribution SRC_DIS)

# --- MySQL Client --------------------------------------------------------------
if (APPLE)
    link_directories(/usr/local/opt/mysql-client/lib)
else ()
    link_directories(/usr/lib64/mysql /usr/lib64)
endif ()

# --- Fetch FTXUI --------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v5.0.0
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/autocam
    ${PROJECT_SOURCE_DIR}/src/packet
    ${PROJECT_SOURCE_DIR}/src/server
    ${PROJECT_SOURCE_DIR}/src/common
    ${PROJECT_SOURCE_DIR}/src/ui
    ${PROJECT_SOURCE_DIR}/third-party
    ${PROJECT_SOURCE_DIR}/third-party/CircularBuffer
    ${PROJECT_SOURCE_DIR}/_deps/ftxui-src/include

if (APPLE)
    /usr/local/opt/mysql-client/include/mysql
else ()
    /usr/include/mysql
endif ()
)

# 指定生成目标
add_executable(f1-2022-tele-dev  ${SRC_LIST})
add_executable(distribution-test  ${SRC_DIS})

target_link_libraries(f1-2022-tele-dev 
PRIVATE ftxui::screen
PRIVATE ftxui::dom
PRIVATE ftxui::component)

target_link_libraries(f1-2022-tele-dev LINK_PUBLIC stdc++fs dl pthread mysqlclient curl sqlite3)

# copy sql file to bin path
set(sql_path ${EXECUTABLE_OUTPUT_PATH}/sql)
file(GLOB SQL_FILES ${PROJECT_SOURCE_DIR}/src/packet/*.sql)
execute_process( COMMAND ${CMAKE_COMMAND} -E remove_directory ${sql_path})
execute_process( COMMAND ${CMAKE_COMMAND} -E make_directory ${sql_path})
execute_process( COMMAND ${CMAKE_COMMAND} -E copy ${SQL_FILES} ${sql_path})


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

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

SET(CMAKE_CXX_FLAGS "-std=c++11 -g -ggdb -Wno-deprecated-declarations ${CMAKE_CXX_FLAGS}")

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/autocam
    ${PROJECT_SOURCE_DIR}/src/packet
    ${PROJECT_SOURCE_DIR}/src/server
    ${PROJECT_SOURCE_DIR}/src/common
    ${PROJECT_SOURCE_DIR}/third-party
    ${PROJECT_SOURCE_DIR}/third-party/CircularBuffer

if (APPLE)
    /usr/local/opt/mysql-client/include/mysql
else ()
    /usr/include/mysql
endif ()
)

# 加载子目录
add_subdirectory(src)

aux_source_directory(. SRC_LIST)
aux_source_directory(./src SRC_LIST)
aux_source_directory(./src/autocam SRC_LIST)
aux_source_directory(./src/packet SRC_LIST)
aux_source_directory(./src/server SRC_LIST)
aux_source_directory(./src/common SRC_LIST)
aux_source_directory(./third-party/loguru SRC_LIST)

aux_source_directory(./src/distribution SRC_DIS)

if (APPLE)
    link_directories(/usr/local/opt/mysql-client/lib)
else ()
    link_directories(/usr/lib64/mysql /usr/lib64)
endif ()

# 指定生成目标
add_executable(f1-2022-tele-dev  ${SRC_LIST})
#add_executable(f1-2022-tele  ${SRC_LIST})
add_executable(distribution-test  ${SRC_DIS})

target_link_libraries(f1-2022-tele-dev LINK_PUBLIC dl pthread mysqlclient curl sqlite3)
#target_link_libraries(f1-2022-tele dl pthread mysqlclient curl)

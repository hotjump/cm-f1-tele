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
set(FETCHCONTENT_QUIET OFF)  # 可选：显示下载进度

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

set(IMGUI_ZIP ${PROJECT_SOURCE_DIR}/third-party/imgui-1.92.6.zip)
set(IMGUI_EXTRACT_DIR ${PROJECT_SOURCE_DIR}/third-party/imgui-1.92.6)

if(EXISTS ${IMGUI_ZIP} AND NOT EXISTS ${IMGUI_EXTRACT_DIR}/imgui.cpp)
    find_program(UNZIP_EXECUTABLE unzip)
    if(UNZIP_EXECUTABLE)
        execute_process(
            COMMAND ${UNZIP_EXECUTABLE} -o ${IMGUI_ZIP} -d ${PROJECT_SOURCE_DIR}/third-party
            RESULT_VARIABLE _imgui_unzip_res
            OUTPUT_QUIET ERROR_QUIET
        )
        if(NOT _imgui_unzip_res EQUAL 0)
            message(FATAL_ERROR "Failed to extract ${IMGUI_ZIP} (unzip returned ${_imgui_unzip_res}).")
        endif()
    else()
        message(FATAL_ERROR "'unzip' not found — cannot extract ${IMGUI_ZIP}.")
    endif()
endif()

if(NOT EXISTS ${IMGUI_EXTRACT_DIR}/imgui.cpp)
    message(FATAL_ERROR "ImGui not found at ${IMGUI_EXTRACT_DIR}.")
endif()

set(imgui_SOURCE_DIR ${IMGUI_EXTRACT_DIR})

set(IMPLOT_ZIP ${PROJECT_SOURCE_DIR}/third-party/implot-0.17.zip)
set(IMPLOT_EXTRACT_DIR ${PROJECT_SOURCE_DIR}/third-party/implot-0.17)

if(EXISTS ${IMPLOT_ZIP} AND NOT EXISTS ${IMPLOT_EXTRACT_DIR}/implot.cpp)
    find_program(UNZIP_EXECUTABLE unzip)
    if(UNZIP_EXECUTABLE)
        execute_process(
            COMMAND ${UNZIP_EXECUTABLE} -o ${IMPLOT_ZIP} -d ${PROJECT_SOURCE_DIR}/third-party
            RESULT_VARIABLE _implot_unzip_res
            OUTPUT_QUIET ERROR_QUIET
        )
        if(NOT _implot_unzip_res EQUAL 0)
            message(FATAL_ERROR "Failed to extract ${IMPLOT_ZIP} (unzip returned ${_implot_unzip_res}).")
        endif()
    else()
        message(FATAL_ERROR "'unzip' not found — cannot extract ${IMPLOT_ZIP}.")
    endif()
endif()

if(NOT EXISTS ${IMPLOT_EXTRACT_DIR}/implot.cpp)
    message(FATAL_ERROR "ImPlot not found at ${IMPLOT_EXTRACT_DIR}.")
endif()

set(implot_SOURCE_DIR ${IMPLOT_EXTRACT_DIR})

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
set(GLFW_ZIP ${PROJECT_SOURCE_DIR}/third-party/glfw-3.4.zip)
set(GLFW_EXTRACT_DIR ${PROJECT_SOURCE_DIR}/third-party/glfw-3.4)

if(EXISTS ${GLFW_ZIP} AND NOT EXISTS ${GLFW_EXTRACT_DIR}/CMakeLists.txt)
    find_program(UNZIP_EXECUTABLE unzip)
    if(UNZIP_EXECUTABLE)
        execute_process(
            COMMAND ${UNZIP_EXECUTABLE} -o ${GLFW_ZIP} -d ${PROJECT_SOURCE_DIR}/third-party
            RESULT_VARIABLE _glfw_unzip_res
            OUTPUT_QUIET ERROR_QUIET
        )
        if(NOT _glfw_unzip_res EQUAL 0)
            message(FATAL_ERROR "Failed to extract ${GLFW_ZIP} (unzip returned ${_glfw_unzip_res}).")
        endif()
    else()
        message(FATAL_ERROR "'unzip' not found — cannot extract ${GLFW_ZIP}.")
    endif()
endif()

if(NOT EXISTS ${GLFW_EXTRACT_DIR}/CMakeLists.txt)
    message(FATAL_ERROR "GLFW not found at ${GLFW_EXTRACT_DIR}.")
endif()

FetchContent_Declare(
    glfw
    URL ""
    DOWNLOAD_COMMAND ""
    SOURCE_DIR "${GLFW_EXTRACT_DIR}"
)

FetchContent_MakeAvailable(glfw)

find_package(OpenGL REQUIRED)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl2.cpp
)

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

add_library(implot STATIC
    ${implot_SOURCE_DIR}/implot.cpp
    ${implot_SOURCE_DIR}/implot_items.cpp
)

target_include_directories(implot PUBLIC
    ${implot_SOURCE_DIR}
)

target_link_libraries(implot PUBLIC imgui)

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

# --- DuckDB (prebuilt via automatic download) ------------------------------
# This project will automatically download a prebuilt DuckDB zip (linux amd64)
# into `third-party/` during CMake configuration if it's not already present.
# The archive URL is configurable via `DUCKDB_URL` but defaults to the
# release referenced by the user request.

set(DUCKDB_URL "https://install.duckdb.org/v1.4.2/libduckdb-linux-amd64.zip" CACHE STRING "URL to DuckDB prebuilt zip for linux-amd64")
set(DUCKDB_ARCHIVE_DIR ${PROJECT_SOURCE_DIR}/third-party)
set(DUCKDB_ZIP ${DUCKDB_ARCHIVE_DIR}/libduckdb-linux-amd64.zip)
set(DUCKDB_EXTRACT_DIR ${DUCKDB_ARCHIVE_DIR}/duckdb)

# Ensure archive dir exists
file(MAKE_DIRECTORY ${DUCKDB_ARCHIVE_DIR})

# Download the zip with wget if not already present
if(NOT EXISTS ${DUCKDB_ZIP})
    find_program(WGET_EXECUTABLE wget)
    if(WGET_EXECUTABLE)
        message(STATUS "Downloading DuckDB from ${DUCKDB_URL} to ${DUCKDB_ZIP} (using wget)")
        execute_process(
            COMMAND ${WGET_EXECUTABLE} -c -O ${DUCKDB_ZIP} ${DUCKDB_URL}
            RESULT_VARIABLE _duckdb_wget_res
            OUTPUT_QUIET ERROR_QUIET
        )
        if(NOT _duckdb_wget_res EQUAL 0)
            message(WARNING "wget failed with code ${_duckdb_wget_res}. Please download ${DUCKDB_URL} manually into ${DUCKDB_ZIP}.")
        endif()
    else()
        message(WARNING "'wget' not found. Please download ${DUCKDB_URL} to ${DUCKDB_ZIP} before building.")
    endif()
else()
    message(STATUS "DuckDB archive already present: ${DUCKDB_ZIP}")
endif()

# Extract the zip into third-party/duckdb if not already extracted
if(EXISTS ${DUCKDB_ZIP} AND NOT EXISTS ${DUCKDB_EXTRACT_DIR})
    find_program(UNZIP_EXECUTABLE unzip)
    file(MAKE_DIRECTORY ${DUCKDB_EXTRACT_DIR})
    if(UNZIP_EXECUTABLE)
        message(STATUS "Extracting ${DUCKDB_ZIP} to ${DUCKDB_EXTRACT_DIR}")
        execute_process(
            COMMAND ${UNZIP_EXECUTABLE} -o ${DUCKDB_ZIP} -d ${DUCKDB_EXTRACT_DIR}
            RESULT_VARIABLE _duckdb_unzip_res
            OUTPUT_QUIET ERROR_QUIET
        )
        if(_duckdb_unzip_res EQUAL 0)
            message(STATUS "Extracted DuckDB to ${DUCKDB_EXTRACT_DIR}")
        else()
            message(WARNING "Failed to extract ${DUCKDB_ZIP} (unzip returned ${_duckdb_unzip_res}). Please extract it manually to ${DUCKDB_EXTRACT_DIR}.")
        endif()
    else()
        message(WARNING "'unzip' not found — cannot extract ${DUCKDB_ZIP}. Please extract the zip manually to ${DUCKDB_EXTRACT_DIR}.")
    endif()
endif()

# Try to locate duckdb include and library from the extracted archive or system paths
find_path(DUCKDB_INCLUDE_DIR
    NAMES duckdb.h
    HINTS ${DUCKDB_EXTRACT_DIR} ${DUCKDB_EXTRACT_DIR}/include /usr/include /usr/local/include
)

find_library(DUCKDB_LIBRARY
    NAMES duckdb libduckdb duckdb_static
    HINTS ${DUCKDB_EXTRACT_DIR} ${DUCKDB_EXTRACT_DIR}/lib /usr/lib /usr/local/lib
)

if(DUCKDB_INCLUDE_DIR AND DUCKDB_LIBRARY)
    message(STATUS "Using DuckDB library: ${DUCKDB_LIBRARY}")
    message(STATUS "DuckDB include dir: ${DUCKDB_INCLUDE_DIR}")
    set(DUCKDB_PREBUILT_FOUND TRUE)
else()
    message(STATUS "DuckDB prebuilt not found in ${DUCKDB_EXTRACT_DIR} or system paths. Please ensure ${DUCKDB_ZIP} is present and extracted.")
    set(DUCKDB_PREBUILT_FOUND FALSE)
endif()

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
aux_source_directory(./src/visualization SRC_LIST)
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
    ${PROJECT_SOURCE_DIR}/third-party
    ${PROJECT_SOURCE_DIR}/third-party/CircularBuffer
    ${PROJECT_SOURCE_DIR}/third-party/sqlite/include
    ${PROJECT_SOURCE_DIR}/third-party/mysql-connector-c/include
)

IF (APPLE)
    INCLUDE_DIRECTORIES(/usr/local/opt/mysql-client/include/mysql)
ELSEIF (UNIX)
    INCLUDE_DIRECTORIES(/usr/include/mysql)
ENDIF ()

# 指定生成目标
if(WIN32)
    # Windows 平台
    add_executable(f1-2022-tele-dev  ${SRC_LIST} ./src/udp/udp_listener_win.cpp)
else()
    # Linux/macOS 平台
    add_executable(f1-2022-tele-dev  ${SRC_LIST} ./src/udp/udp_listener_unix.cpp)
endif()

add_executable(distribution-test  ${SRC_DIS})

if (WIN32)
    target_link_libraries(f1-2022-tele-dev PRIVATE implot stdc++fs dl pthread sqlite3 spdlog::spdlog mysql ws2_32)
elseif (UNIX)
    target_link_libraries(f1-2022-tele-dev PRIVATE implot stdc++fs dl pthread sqlite3 spdlog::spdlog mysqlclient)
endif()

# Link DuckDB if a prebuilt library was found
if (DUCKDB_PREBUILT_FOUND)
    target_include_directories(f1-2022-tele-dev PRIVATE ${DUCKDB_INCLUDE_DIR})
    target_link_libraries(f1-2022-tele-dev PRIVATE ${DUCKDB_LIBRARY})
else()
    message(STATUS "DuckDB not linked (prebuilt not found). If you want to link DuckDB, set -DUSE_SYSTEM_DUCKDB=ON and -DDUCKDB_ROOT or install duckdb system-wide.")
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

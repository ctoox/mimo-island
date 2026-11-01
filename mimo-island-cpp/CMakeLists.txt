cmake_minimum_required(VERSION 3.16)
project(mimo-island LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt 设置
set(CMAKE_PREFIX_PATH "C:/Qt/6.7.0/msvc2019_64")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

# 源文件
set(SOURCES
    src/main.cpp
    src/DynamicIsland.cpp
    src/FetchThread.cpp
    src/ProfileManager.cpp
    src/SpringValue.cpp
    app.rc
)

# 头文件
set(HEADERS
    src/DynamicIsland.h
    src/FetchThread.h
    src/ProfileManager.h
    src/SpringValue.h
)

# 创建可执行文件
add_executable(mimo-island ${SOURCES} ${HEADERS})

# 链接 Qt 库
target_link_libraries(mimo-island PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)

# Windows 特定设置
if(WIN32)
    set_target_properties(mimo-island PROPERTIES
        WIN32_EXECUTABLE ON
        LINK_FLAGS "/SUBSYSTEM:WINDOWS"
    )
endif()

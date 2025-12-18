cmake_minimum_required(VERSION 3.16)
project(PhoneBookQT LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Core)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.h)

add_executable(PhoneBookQT
    main.cpp
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(PhoneBookQT PRIVATE include)

target_link_libraries(PhoneBookQT
    Qt5::Widgets
    Qt5::Core
)

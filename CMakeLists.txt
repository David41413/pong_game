cmake_minimum_required(VERSION 3.22)
project(HelloSFML LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System Audio)

add_executable(main src/main.cpp)

target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(main PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio)

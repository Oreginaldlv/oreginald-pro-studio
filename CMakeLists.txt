cmake_minimum_required(VERSION 3.22)
project(OreginaldProStudio VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

enable_testing()

option(OPS_ENABLE_ASIO "Enable ASIO support on Windows" ON)
option(OPS_BUILD_APP "Build JUCE desktop app" ON)

if(NOT DEFINED JUCE_DIR)
  message(FATAL_ERROR "JUCE_DIR not set. Configure with -DJUCE_DIR=<path-to-JUCE>")
endif()

if(NOT EXISTS "${JUCE_DIR}/CMakeLists.txt")
  message(FATAL_ERROR "JUCE_DIR is invalid. Expected ${JUCE_DIR}/CMakeLists.txt")
endif()

add_subdirectory("${JUCE_DIR}" JUCE)

if(OPS_BUILD_APP)
  add_subdirectory(source)
endif()

add_subdirectory(tests)
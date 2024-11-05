cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(zBot VERSION 3.0.0 LANGUAGES CXX)

file(GLOB SOURCES
	src/framecounting.cpp
    src/gui.cpp
    src/keybinds.cpp
    src/physicsbypass.cpp
    src/playbackmanager.cpp
    src/recordmanager.cpp
    src/speedhackaudio.cpp
    # src/practicemode.cpp             # The old practice mode implementation
    src/zKeyChecker/keychecker.cpp 
    # src/zPracticeMode/practicemode.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    src
)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} $ENV{GEODE_SDK}/build)

CPMAddPackage("gh:maxnut/GDReplayFormat#c70e8694401570d410acf43d02bed1764187389f")
CPMAddPackage("gh:matcool/gd-imgui-cocos#324dc5d41c7d210d90732df071ca415aa3a34310")
target_compile_definitions(${PROJECT_NAME} PRIVATE MOD_VERSION="${CMAKE_PROJECT_VERSION}")

if (WIN32)
target_link_libraries(${PROJECT_NAME} imgui-cocos libGDR)
elseif (APPLE)
target_link_libraries(${PROJECT_NAME} imgui-cocos libGDR "-framework IOKit")
endif()


setup_geode_mod(${PROJECT_NAME})

cmake_minimum_required(VERSION 3.16)

project(dde-dock
  VERSION 0.1
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Quick Widgets QuickControls2 REQUIRED)

add_executable(${PROJECT_NAME}
  src/models/windowelement.h
  src/models/windowelement.cpp
  src/models/elementGroupModel.h
  src/models/elementGroupModel.cpp
  src/backgrounds/testbackground/backgroundtest.h
  src/backgrounds/testbackground/backgroundtest.cpp
  src/main.cpp
  qml/root.qrc
)

target_include_directories(${PROJECT_NAME} PUBLIC
  src
)


target_link_libraries(${PROJECT_NAME} PUBLIC
  Qt5::Quick
  Qt5::Widgets
  Qt5::QuickControls2
)

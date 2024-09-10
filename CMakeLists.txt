cmake_minimum_required(VERSION 3.5)

project(imagesAligner LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

if(ANDROID)
  add_library(imagesAligner SHARED
    main.cpp
    imageswindow.cpp
    imageswindow.h
  )
else()
  add_executable(imagesAligner
    main.cpp
    imageswindow.cpp
    imageswindow.h
  )
endif()

target_link_libraries(imagesAligner PRIVATE Qt5::Widgets)

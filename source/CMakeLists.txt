cmake_minimum_required(VERSION 3.5)

project(integral LANGUAGES CXX)

string(FIND "${Qt5_DIR}" "cortexa7hf-neon-poky-linux-gnueabi" position)

if(${position} GREATER -1)
    set(OE_QMAKE_PATH_EXTERNAL_HOST_BINS /opt/fsl-imx-x11/4.1.15-2.1.0/sysroots/x86_64-pokysdk-linux/usr/bin)

    set(CMAKE_C_COMPILER "arm-poky-linux-gnueabi-gcc")
    set(CMAKE_CXX_COMPILER "arm-poky-linux-gnueabi-g++")

    set(CMAKE_SYSROOT /opt/fsl-imx-x11/4.1.15-2.1.0/sysroots/cortexa7hf-neon-poky-linux-gnueabi)
    set(CMAKE_CXX_FLAGS "-mfloat-abi=hard")
endif()

message("ANDROID = ${ANDROID}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/interface)

if(ANDROID)
  add_library(integral SHARED
    main.cpp
    mainwindow.cpp
    mainwindow.h
  )
else()
  add_executable(integral
    main.cpp
    interface/mainwindow.cpp
  )
endif()

target_link_libraries(integral PRIVATE Qt5::Widgets)

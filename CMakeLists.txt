cmake_minimum_required(VERSION 3.19)
project(gods98)

set(CMAKE_CXX_STANDARD 14)

set(SRC_MAIN
    main.cpp
    registry.cpp registry.h
    input.cpp input.h
    sound.cpp sound.h
    directdraw.cpp directdraw.h debug.cpp debug.h)

add_executable(gods98 WIN32 ${SRC_MAIN})
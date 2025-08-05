cmake_minimum_required(VERSION 3.16)
project(pairs_backtester)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
set(CMAKE_CXX_STANDARD 17)
add_executable(pairs_backtester main.cpp
        main.cpp
        compute.cpp
        compute.h
        dataScraper.cpp
        dataScraper.h
        csvExport.cpp
        csvExport.h
)

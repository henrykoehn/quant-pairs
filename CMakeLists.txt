cmake_minimum_required(VERSION 3.16)
project(pairs_backtester)

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

cmake_minimum_required(VERSION 3.28)
project(se_comp_arch24_omp_Yashi213)

OPTION (USE_OpenMP "Use OpenMP" ON)
IF(USE_OpenMP)
    FIND_PACKAGE(OpenMP)
    IF(OPENMP_FOUND)
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    ENDIF()
ENDIF()



set(CMAKE_CXX_STANDARD 17)

include_directories(Parser)
include_directories(Calculate)

add_executable(se_comp_arch24_omp_Yashi213
        Parser.cpp
        Parser.h
        main.cpp
        hit.cpp
        hit.h
        Calculate.h
        Calculate.cpp
)

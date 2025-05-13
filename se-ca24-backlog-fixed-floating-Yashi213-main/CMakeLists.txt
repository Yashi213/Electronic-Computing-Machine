cmake_minimum_required(VERSION 3.29)
project(se_ca24_backlog_fixed_floating_Yashi213)

set(CMAKE_CXX_STANDARD 14)

include_directories(library/fixed)
include_directories(library/float)
include_directories(library/pars)

add_executable(se_ca24_backlog_fixed_floating_Yashi213
    library/fixed/CustomFloat.cpp
    library/fixed/CustomFloat.h
    library/float/float16.cpp
    library/float/float16.h
    library/float/float32.cpp
    library/float/float32.h
    library/pars/Parsing.cpp
    library/pars/Parsing.h
    main.cpp)

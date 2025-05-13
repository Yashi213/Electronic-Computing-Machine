add_library(fixed
        CustomFloat.cpp
        CustomFloat.h
        ../float/float32.cpp
)

target_include_directories(fixed PUBLIC ${PROJECT_SOURCE_DIR})
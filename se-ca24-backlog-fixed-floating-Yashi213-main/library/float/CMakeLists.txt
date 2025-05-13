add_library(float
        float32.cpp
        float32.h
        float16.cpp
        float16.h
)

target_include_directories(float PUBLIC ${PROJECT_SOURCE_DIR})
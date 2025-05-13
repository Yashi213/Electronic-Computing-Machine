#include <iostream>
#include <fstream>
#include "hit.h"
#include "omp.h"
#include <random>
#include <ctime>

class Calculate {
private:
    std::string output;
    float x_min;
    float x_max;
    float y_min;
    float y_max;
    float z_min;
    float z_max;
    uint64_t max_iter;
public:
    double time{};
    int threads{};
    Calculate(const float* axis_range, uint64_t max_iter, std::string& output);
    uint64_t hit_count = 0;
    float get_volume_omp(int num_threads, bool is_default, int chunk_size = 10);
    float get_volume();
};

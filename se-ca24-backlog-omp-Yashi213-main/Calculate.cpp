#include "Calculate.h"

Calculate::Calculate(const float *axis_range, uint64_t max_iter_, std::string& output) {
    this->x_min = axis_range[0];
    this->x_max = axis_range[1];
    this->y_min = axis_range[2];
    this->y_max = axis_range[3];
    this->z_min = axis_range[4];
    this->z_max = axis_range[5];
    this->max_iter = max_iter_;
    this->output = output;
}


float xorshift32_double(float min, float max, uint32_t& state) {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return min + (max - min) * (state) / (UINT32_MAX);
}

float Calculate::get_volume_omp(int num_threads, bool is_default, int chunk_size){
    hit_count = 0;
    int n;
    if(!is_default){
        n = num_threads;
    } else{
        n = omp_get_max_threads();
    }
    auto begin = omp_get_wtime();
#pragma omp parallel num_threads(n)
    {
        uint32_t state = 123456;
        int counter = 0;

#pragma omp for schedule(static)
        for (uint64_t i = 0; i < max_iter; i++) {
            if(hit_test(xorshift32_double(x_min, x_max, state), xorshift32_double(y_min, y_max, state), xorshift32_double(z_min, z_max, state))) {
                counter++;
            }
        }
#pragma omp atomic
        hit_count += counter;
    }
    auto end = omp_get_wtime();
    auto elapsed_seconds = (end - begin) * 1000;
    float vol = float(hit_count) / float(max_iter) * (x_max - x_min) * (y_max - y_min) * (z_max - z_min);
    FILE* file = fopen(output.c_str(), "w");
    fprintf(file, "%g\n", vol);
    printf("Time (%i threads): %f\n", n, elapsed_seconds);
    //std::cout << elapsed_seconds << '\n';
    threads = num_threads;
    time = elapsed_seconds;
    return vol;
}


float Calculate::get_volume() {
    hit_count = 0;
    uint32_t state = 123456;
    // оставил реализацию других генераторов
    // std::random_device dev;
    // std::mt19937 rng(dev());
    // std::ranlux24_base rng(dev());
    // std::uniform_real_distribution<float> dist1(x_min, x_max);
    // std::uniform_real_distribution<float> dist2(y_min, y_max);
    // std::uniform_real_distribution<float> dist3(z_min, z_max);

    auto begin = omp_get_wtime();
    for (int i = 0; i < max_iter; i++) {
        if(hit_test(xorshift32_double(x_min, x_max, state), xorshift32_double(y_min, y_max, state), xorshift32_double(z_min, z_max, state))) {
            hit_count++;
        }

        //генератор mt19937 и ranlux24_base
        // if (hit_test(dist1(rng), dist2(rng), dist3(rng))) {
        //     hit_count++;
        // }
    }
    auto end = omp_get_wtime();
    auto elapsed_seconds = (end - begin) * 1000;
    float vol = float(hit_count) / float(max_iter) * (x_max - x_min) * (y_max - y_min) * (z_max - z_min);
    FILE* file = fopen(output.c_str(), "w");
    fprintf(file, "%g\n", vol);
    printf("Time (%i threads): %f\n", 1, elapsed_seconds);
    threads = 1;
    time = elapsed_seconds;
    return vol;
}
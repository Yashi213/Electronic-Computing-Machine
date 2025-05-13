#include "hit.h"
#include <math.h>
bool hit_test(float x, float y, float z) {
    return (x * x * x * x - 2 * x * x * x + 4 * (z * z + y * y) <= 0.0f);
}

const float* get_axis_range() {
    float tmp = 3 * sqrt(3) / 8;
    static const float range[] = {0.0f, 2.0f, -tmp, tmp, -tmp, tmp};
    return range;
}
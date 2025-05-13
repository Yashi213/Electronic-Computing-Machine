#include <cstdint>
#include <iostream>
#include <iomanip>


class float32 {
private:
    uint64_t value = 0;
    int16_t exp = 0;
    int normalized_exp = 0;
    uint64_t mantisa = 0;
    bool negative = false;
    int rounding_type = 0;


public:
    float32(){};
    float32(uint64_t val, int rounding_type_);

    float32(uint64_t exp_, uint64_t mantisa_, bool negative_);

    void NormalizeExp();

    void GetNormalExp();

    friend std::ostream& operator <<(std::ostream& c, float32& f);
    friend float32 operator +(float32& first, float32& second);
    friend float32 operator *(float32& first, float32& second);
    friend float32 operator -(float32& first, float32& second);
    friend float32 operator /(float32& first, float32& second);
};

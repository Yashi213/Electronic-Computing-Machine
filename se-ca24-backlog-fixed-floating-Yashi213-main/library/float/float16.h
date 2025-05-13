#include <cstdint>
#include <iostream>
#include <iomanip>

#include <string>


class float16 {
private:
    int rounding_type;
    uint16_t value = 0;
    int8_t exp = 0;
    int normalized_exp = 0;
    uint32_t mantisa = 0;
    bool negative = false;


public:
    float16(){};

    float16(uint16_t val, int rounding_type_);
    float16(uint8_t exp_, uint32_t mantisa_, bool negative_);

    void NormalizeExp();

    void GetNormalExp();

    static uint32_t Round(int rounding_type, uint64_t result, uint64_t shift, bool minus);

    friend std::ostream& operator <<(std::ostream& c, float16& f);
    friend float16 operator +(float16& first, float16& second);
    friend float16 operator *(float16& first, float16& second);
    friend float16 operator -(float16& first, float16& second);
    friend float16 operator /(float16& first, float16& second);
};

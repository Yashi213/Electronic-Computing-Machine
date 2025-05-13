#include <string>
#include <iostream>
#include <cstdint>
#include <string>

class CustomFloat {
private:
    int A;
    int B;
    int rounding_type;
    uint64_t whole;
    uint64_t fractional = 0;


public:
    bool minus = false;
    int32_t val;
    CustomFloat(int A_, int B_, int rounding_type_);
    CustomFloat(){};

    uint32_t Round(uint64_t result);

    void GetResult(int32_t val);
    friend std::ostream& operator << (std::ostream& f, CustomFloat& a);

    static uint32_t Round(int rounding_type, uint64_t result, int shift, bool minus);
};

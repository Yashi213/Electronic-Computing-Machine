#include <sstream>
#include "float32.h"

#include <cmath>
#include <random>

float32::float32(uint64_t val, int rounding_type_) {
    value = val;
    negative =  static_cast<bool>(val >> 31);
    rounding_type = rounding_type_;
    exp = ((static_cast<int32_t>(val >> 23)) & ((1 << 8) - 1)) - (1 << (8 - 1)) + 1;
    mantisa = val & ((1 << 23) - 1);
    if (exp != -127 && exp != 128) {
        mantisa += (1 << 23);
    }
}


void float32::NormalizeExp() {
    if(!(exp == -127 && mantisa != 0)) {
        return;
    }
    exp++;
    while ((mantisa >> 23) == 0) {
        exp--;
        mantisa <<= 1;
    }
}

std::ostream &operator<<(std::ostream& c, float32 &f) {
    if((f.exp == 128) && (f.mantisa == 0)) {
        c << ((f.negative) ? "-inf" : "inf");
    } else if ((f.exp == 128) && (f.mantisa != 0)) {
        c << "nan";
    } else if ((f.exp == -127) && (f.mantisa == 0)) {
        if (f.negative) {
            c << "-";
        }
        c << "0x0." << std::string(6, '0') << "p+0";
    } else {
        f.NormalizeExp();
        if (f.negative) {
            c << "-";
        }
        c << "0x1." << std::setfill('0') << std::setw(6);
        c << std::hex << ((f.mantisa) & ((1 << 23) - 1)) * 2 << std::dec;
        if (f.exp >= 0) {
            c << "p+";
        } else{
            c << "p";
        }
        c << (static_cast<int16_t>(f.exp) - f.normalized_exp);
    }
    return c;
}

uint32_t Round2(int rounding_type, uint64_t result, uint64_t shift, bool minus) {
    uint64_t mask = (1 << shift) - 1;
    if (rounding_type == 0) {
        return result >> shift;
    }else if (rounding_type == 1) {
        if ((result & mask) > (1 << (shift - 1))) {
            return (result >> shift) + 1;
        } else if ((result & mask) < (1ULL << (shift - 1))) {
            return (result >> shift);
        } else {
            if (result & (1ULL << shift)) {
                return (result >> shift) + 1;
            } else {
                return (result >> shift);
            }
        }
    } else if(rounding_type == 2) {
        if (minus) {
            return result >> shift;
        } else if (result & mask) {
            return (result >> shift) + 1;
        } else {
            return result >> shift;
        }
    } else if(rounding_type == 3) {
        if (!minus) {
            return result >> shift;
        } else if (result & mask) {
            return (result >> shift) + 1;
        } else {
            return (result >> shift);
        }
    }
    return 0;
}

void float32::GetNormalExp() {
    if(this->exp > - 127 && this->exp < 128) {
        return;
    }
    if(this->exp <= -127) {
        if(exp > -150) {
            mantisa >>= (-128 - exp);
            exp = -127;
        }else {
            exp = -127;
            if (rounding_type == 2 && !negative || rounding_type == 3 && negative) {
                mantisa = 1;
            }else {
                mantisa = 0;
            }
        }
    }else {
        if (rounding_type == 1 || rounding_type == 2 && !negative || rounding_type == 3 && negative) {
            exp = 128;
            mantisa = 0;
        }else {
            exp = 127;
            mantisa = (1 << 24);
        }
    }
}

float32 operator+(float32& first, float32& second) {
    float32 result(first);

    if ((first.exp == 128) && (first.mantisa == 0) && (second.exp == 128) && (second.mantisa == 0) && first.negative != second.negative) {
        result.mantisa = 1;
        result.exp = 128;
        return result;
    }
    if ((first.exp == 128) && (first.mantisa == 0) || (second.exp == 128) && (second.mantisa == 0)) {
        if ((first.exp == 128) && (first.mantisa == 0)) {
            return result;
        } else {
            return second;
        }
    }
    if ((first.exp == 128) && (first.mantisa != 0) || (second.exp == 128) && (second.mantisa != 0)) {
        if ((first.exp == 128) && (first.mantisa != 0)) {
            return result;
        } else {
            return second;
        }
    }
    if (first.mantisa == second.mantisa && first.exp == second.exp && first.negative != second.negative) {
        result.mantisa = 0;
        result.exp = -127;
        if (first.rounding_type == 3) {
            result.negative = true;
        } else {
            result.negative = false;
        }
        return result;
    }
    if ((first.exp == -127) && (first.mantisa == 0) || (second.exp == -127) && (second.mantisa == 0)) {
        if ((first.exp == -127) && (first.mantisa == 0) && (second.exp == -127) && (second.mantisa == 0)) {
            if (first.negative && second.negative) {
                return result;
            }
            result.mantisa = 0;
            result.exp = -127;
            result.negative = false;
            return result;
        }
        if ((first.exp == -127) && (first.mantisa == 0)) {
            return second;
        } else {
            return result;
        }
    }

    first.NormalizeExp();
    second.NormalizeExp();

    float32 max_val = first;
    float32 min_val = first;

    if (first.exp > second.exp) {
        min_val = second;
    } else if (first.exp < second.exp) {
        max_val = second;
    } else if (first.mantisa > second.mantisa) {
        min_val = second;
    } else {
        max_val = second;
    }

    if(first.negative == second.negative) {
        uint32_t diff = abs(first.exp - second.exp);
        result.exp = std::max(first.exp, second.exp);
        if(diff <= 26) {
            uint64_t sum = (static_cast<uint64_t>(max_val.mantisa) << diff) + min_val.mantisa;
            if ((sum >> diff >> 23) > 1) {
                diff++;
                result.exp++;
            }
            result.mantisa = Round2(first.rounding_type, sum, diff, result.negative);
        }else {
            if(first.rounding_type == 2 && !result.negative || first.rounding_type == 3 && result.negative) {
                result.mantisa = max_val.mantisa + 1;
            }else {
                result.mantisa = max_val.mantisa;
            }
        }
        result.GetNormalExp();
        return result;
    }else {
        if (max_val.negative) {
            max_val.negative = false;
            max_val.GetNormalExp();
            min_val.GetNormalExp();
            return min_val - max_val;
        } else {
            min_val.negative = false;
            max_val.GetNormalExp();
            min_val.GetNormalExp();
            return max_val - min_val;
        }
    }
}

float32 operator-(float32 &first, float32 &second) {
    float32 result(first);

    if ((first.exp == 128) && (first.mantisa == 0) && (second.exp == 128) && (second.mantisa == 0) && first.negative != second.negative) {
        result.mantisa = 1;
        result.exp = 16;
        return result;
    }
    if ((first.exp == 128) && (first.mantisa == 0) || (second.exp == 128) && (second.mantisa == 0)) {
        if ((first.exp == 128) && (first.mantisa == 0)) {
            return result;
        } else {
            second.negative = true;
            return second;
        }
    }
    if ((first.exp == 128) && (first.mantisa != 0) || (second.exp == 128) && (second.mantisa != 0)) {
        if ((first.exp == 128) && (first.mantisa != 0)) {
            return result;
        } else {
            return second;
        }
    }
    if (first.mantisa == second.mantisa && first.exp == second.exp && first.negative != second.negative) {
        result.mantisa = 0;
        result.exp = -127;
        if (first.rounding_type == 3) {
            result.negative = true;
        } else {
            result.negative = false;
        }
        return result;
    }
    if ((first.exp == -127) && (first.mantisa == 0) || (second.exp == -127) && (second.mantisa == 0)) {
        if ((first.exp == -127) && (first.mantisa == 0) && (second.exp == -127) && (second.mantisa == 0)) {
            if (first.negative && second.negative) {
                return result;
            }
            result.mantisa = 0;
            result.exp = -127;
            result.negative = false;
            return result;
        }
        if ((first.exp == -127) && (first.mantisa == 0)) {
            second.negative = true;
            return second;
        } else {
            return result;
        }
    }

    if (first.negative != second.negative) {
        second.negative = !second.negative;
        return first + second;
    }

    first.NormalizeExp();
    second.NormalizeExp();

    if (first.exp < second.exp || first.exp == second.exp && first.mantisa < second.mantisa) {
        first.GetNormalExp();
        second.GetNormalExp();
        second.negative = !second.negative;
        first.negative = !first.negative;
        return second - first;
    }

    float32 max_val = first;
    float32 min_val = second;


    result.exp = max_val.exp;
    result.negative = max_val.negative;
    uint32_t exp_shift = max_val.exp - min_val.exp;
    if (exp_shift <=  25) {
        uint64_t sum = (static_cast<uint64_t>(max_val.mantisa) << exp_shift) - min_val.mantisa;
        while ((sum >> exp_shift >> 23) == 0) {
            --result.exp;
            sum <<= 1;
        }
        result.mantisa = Round2(first.rounding_type, sum, exp_shift, result.negative);
    } else {
        if (first.rounding_type == 2 && result.negative || first.rounding_type == 3 && !result.negative || first.rounding_type == 0) {
            result.mantisa = max_val.mantisa - 1;
        } else {
            result.mantisa = max_val.mantisa;
        }
    }
    result.GetNormalExp();
    return result;
}


float32 operator*(float32 &first, float32 &second) {
    float32 result(first);

    if ((first.exp == 128) && (first.mantisa == 0) && (second.exp == 128) && (second.mantisa == 0) && first.negative != second.negative) {
        result.mantisa = 0;
        result.exp = 128;
        result.negative = first.negative != second.negative;
        return result;
    }
    if ((first.exp == 128) && (first.mantisa == 0) || (second.exp == 128) && (second.mantisa == 0)) {
        if ((first.exp == 128) && (first.mantisa == 0)) {
            return result;
        } else {
            return second;
        }
    }
    if ((first.exp == 128) && (first.mantisa != 0) || (second.exp == 128) && (second.mantisa != 0)) {
        if ((first.exp == 128) && (first.mantisa != 0)) {
            return result;
        } else {
            return second;
        }
    }
    if (first.mantisa == second.mantisa && first.exp == second.exp && first.negative != second.negative) {
        result.mantisa = 0;
        result.exp = -127;
        if (first.rounding_type == 3) {
            result.negative = true;
        } else {
            result.negative = false;
        }
        return result;
    }
    if ((first.exp == -127) && (first.mantisa == 0) || (second.exp == -127) && (second.mantisa == 0)) {
        if ((first.exp == -127) && (first.mantisa == 0) && (second.exp == -127) && (second.mantisa == 0)) {
            if (first.negative && second.negative) {
                return result;
            }
            result.mantisa = 0;
            result.exp = -127;
            result.negative = false;
            return result;
        }
        if ((first.exp == -127) && (first.mantisa == 0)) {
            second.negative = true;
            return second;
        } else {
            return result;
        }
    }

    first.NormalizeExp();
    second.NormalizeExp();
    result.negative = first.negative != second.negative;
    result.exp = first.exp + second.exp;

    uint64_t mul = static_cast<uint64_t>(first.mantisa) * second.mantisa;

    if ((mul >> 23 >> 23) > 1) {
        result.exp++;
    }
    uint32_t shift = 0;
    if (result.exp <= -127) {
        for (int i = -127; i >= result.exp; --i) {
            shift++;
        }
    }
    if ((mul >> 23 >> 23) > 1) {
        result.mantisa = Round2(first.rounding_type, mul, 24 + shift, result.negative);
        result.mantisa <<= shift;
    }else {
        result.mantisa = Round2(first.rounding_type, mul, 23 + shift, result.negative);
    }
    result.mantisa <<= shift;

    result.GetNormalExp();
    return result;

}

float32 operator/(float32& first, float32& second) {
    float32 result(first);

    if (first.exp == 128 && first.mantisa != 0 || second.exp == 128 && second.mantisa != 0) {
        if (first.exp == 128 && first.mantisa != 0) {
            return result;
        } else {
            return second;
        }
    }
    if ((first.exp == 128 && first.mantisa == 0 && second.exp == 128 && second.mantisa == 0) ||
        (first.exp == -127 && first.mantisa == 0 && second.exp == -127 && second.mantisa == 0)) {
        result.mantisa = 1;
        result.exp = 128;
        return result;
    }
    if (first.exp == 128 && first.mantisa == 0 || second.exp == -127 && second.mantisa == 0) {
        result.mantisa = 0;
        result.exp = 128;
        result.negative = first.negative != second.negative;
        return result;
    }
    if (first.exp == -127 && first.mantisa == 0 || first.exp == 128 && first.mantisa == 0) {
        result.mantisa = 0;
        result.exp = -127;
        result.negative = first.negative != second.negative;
        return result;
    }

    first.NormalizeExp();
    second.NormalizeExp();
    result.negative = first.negative != second.negative;
    result.exp = first.exp - second.exp;

    if (first.mantisa / second.mantisa == 0) {
        result.exp--;
    }

    uint64_t div_tmp = second.mantisa;
    uint32_t shift = 0;

    if (result.exp <= -127) {
        for (int i = -127; i >= result.exp ; --i) {
            div_tmp <<= 1;
            shift++;
        }
    }
    uint64_t div_result;
    uint64_t reminder;
    if (first.mantisa / second.mantisa == 0) {
         div_result = (static_cast<uint64_t>(first.mantisa) << 24) / div_tmp;
         reminder = (static_cast<uint64_t>(first.mantisa) << 24) - div_result * div_tmp;
    }else {
         div_result = (static_cast<uint64_t>(first.mantisa) << 23) / div_tmp;
         reminder = (static_cast<uint64_t>(first.mantisa) << 23) - div_result * div_tmp;
    }

    if(result.rounding_type == 0) {
        result.mantisa= div_result << shift;
    }else if(result.rounding_type == 1) {
        if (2 * reminder >= div_tmp) {
            result.mantisa = (div_result + 1) << shift;
        } else if (2 * reminder < div_tmp) {
            result.mantisa = div_result << shift;
        } else if (div_result & 1) {
            result.mantisa = (div_result + 1) << shift;
        } else {
            result.mantisa = div_result << shift;
        }
    }else if(result.rounding_type == 2) {
        if (result.negative) {
            result.mantisa = div_result << shift;
        } else if (reminder) {
            result.mantisa= (div_result + 1) << shift;
        } else {
            result.mantisa= div_result << shift;
        }
    }else if(result.rounding_type == 3) {
        if (!result.negative) {
            result.mantisa = div_result << shift;
        } else if (reminder) {
            result.mantisa = (div_result + 1) << shift;
        } else {
            result.mantisa = div_result << shift;
        }
    }

    result.GetNormalExp();
    return result;
}
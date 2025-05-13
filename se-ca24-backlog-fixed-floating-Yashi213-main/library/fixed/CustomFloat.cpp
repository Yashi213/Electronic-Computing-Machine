
#include "CustomFloat.h"

std::string CorrectFrac1(uint64_t Frac){
    if (100 > Frac && Frac >= 10){
        return "0" + std::to_string(Frac);
    } else if (Frac < 10) {
        return "00" + std::to_string(Frac);
    } else {
        return std::to_string(Frac);
    }
}

int64_t* Rounding(int64_t value, int A, int B, int rounding_type){
    int64_t* result;
    int64_t whole = (int64_t) value >> B;
    int64_t frac =  (int64_t) value & ((1 << B) - 1);
    if(rounding_type == 0){
        frac =  frac * 1000 >> B;
        whole &= ((1 << A) - 1);
    }
    result[0] = frac;
    result[1] = whole;
    return result;
}


uint32_t CustomFloat::Round(int rounding_type, uint64_t result, int shift, bool minus) {
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

CustomFloat::CustomFloat(int A_, int B_, int rounding_type_) {
    A = A_;
    B = B_;
    rounding_type = rounding_type_;
}

std::ostream &operator<<(std::ostream &f, CustomFloat &a) {
    if(a.minus && !(a.whole == 0 && a.fractional == 0)) {
        f << '-';
    }
    f << a.whole << "." << CorrectFrac1(a.fractional);
    return f;
}

void CustomFloat::GetResult(int32_t val_) {
    if(val_ >> (A + B - 1) & 1){
        this->val = -val_;
        minus = true;
    } else{
        this->val = val_;
        minus = false;
    }
    whole = val >> B;
    fractional = (val & ((1 << B) - 1)) * 1000LL;


    this->fractional = CustomFloat::Round(this->rounding_type, this->fractional, this->B, this->minus);
    if (this->fractional >= 1000) {
        this->whole++;
        this->fractional %= 1000;
    }

    uint64_t mask = (static_cast<uint64_t> (1) << A) - 1;
    whole &= mask;
}


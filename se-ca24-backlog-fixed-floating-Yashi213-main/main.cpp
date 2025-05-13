#include <iostream>
#include <cstdint>
#include "library/pars/Parsing.h"
#include "library/fixed/CustomFloat.h"
#include "library/float/float32.h"
#include "library/float/float16.h"

int main(int argc, char** argv){
    Parsing pars(argc, argv);


    if(!pars.FloatSingle && !pars.FloatHalf){
        if(pars.operation == "+"){
            CustomFloat a(pars.format[0], pars.format[1], pars.rounding_type);
            a.GetResult(static_cast<int32_t>(pars.firs_val + pars.second_val));
            std::cout << a;
        } else if(pars.operation == "-"){
            CustomFloat a(pars.format[0], pars.format[1], pars.rounding_type);
            a.GetResult(pars.firs_val - pars.second_val);
            std::cout << a;
        } else if(pars.operation == "/"){
            CustomFloat a(pars.format[0], pars.format[1], pars.rounding_type);
            if(pars.second_val == 0){
                std::cerr << "division by zero";
                return 0;
            }
            if(pars.firs_val < 0 != pars.second_val < 0 && (pars.firs_val != 0 && pars.second_val != 0)) {
                a.minus = true;
            }
            uint64_t result = ((static_cast<uint64_t>(abs(pars.firs_val)) << pars.format[1])) / static_cast<uint64_t>(abs(pars.second_val));
            uint64_t reminder =  ((static_cast<uint64_t>(abs(pars.firs_val)) << pars.format[1])) - result * static_cast<uint64_t>(abs(pars.second_val));
            uint64_t first_val = static_cast<uint64_t>(abs(pars.firs_val));
            uint64_t second_val = static_cast<uint64_t>(abs(pars.second_val));
            uint64_t second_res;
            uint64_t mask = (static_cast<uint64_t> (1) << 32) - 1;
            if(pars.rounding_type == 0) {
                second_res = result & mask;
            } else if(pars.rounding_type == 1) {
                if (2 * reminder > second_val) {
                    second_res = (result + 1) & mask;
                } else if (2 * reminder < second_val) {
                    second_res = result & mask;
                } else if (result & 1) {
                   second_res = (result + 1) & mask;
                } else {
                    second_res = result & mask;
                }
            } else if(pars.rounding_type == 2) {
                if (a.minus) {
                    second_res = result & mask;
                } else if (reminder) {
                    second_res = (result + 1) & mask;
                } else {
                    second_res = result & mask;
                }
            } else if(pars.rounding_type == 3) {
                if (!a.minus) {
                    second_res = result & mask;
                } else if (reminder) {
                    second_res = (result + 1) & mask;
                } else {
                    second_res = result & mask;
                }
            }
            if(a.minus) {
                second_res = -second_res;
            }
            a.GetResult(static_cast<int32_t>(second_res));



            std::cout << a;
        } else if(pars.operation == "*" || pars.operation == "M") {
            CustomFloat a(pars.format[0], pars.format[1], pars.rounding_type);
            uint64_t result = (static_cast<uint64_t>(abs(pars.firs_val)) * static_cast<uint64_t>(abs(pars.second_val)));

            if(pars.firs_val < 0 != pars.second_val < 0 && (pars.firs_val != 0 && pars.second_val != 0)) {
                a.minus = true;
            }
            result = CustomFloat::Round(pars.rounding_type ,result, pars.format[1], a.minus);
            if(a.minus) {
                result = -result;
            }


            a.GetResult(static_cast<int32_t>(result));
            if(a.minus) {
                a.val = -a.val;
            }
            std::cout << a;
        } else{
            CustomFloat a(pars.format[0], pars.format[1], pars.rounding_type);
            a.GetResult(pars.firs_val);
            std::cout << a;
        }
    } else if(pars.FloatSingle){
        if(pars.operation == "+"){
            float32 f(pars.firs_val, pars.rounding_type);
            float32 f2(pars.second_val, pars.rounding_type);
            float32 res = f + f2;
            std::cout << res;
        } else if(pars.operation == "*" || pars.operation == "M"){
            float32 f(pars.firs_val, pars.rounding_type);
            float32 f2(pars.second_val, pars.rounding_type);
            float32 res = f * f2;
            std::cout << res;
        } else if(pars.operation == "-"){
            float32 f(pars.firs_val, pars.rounding_type);
            float32 f2(pars.second_val, pars.rounding_type);
            float32 res = f - f2;
            std::cout << res;
        }
        else if(pars.operation == "/") {
            float32 f(pars.firs_val, pars.rounding_type);
            float32 f2(pars.second_val, pars.rounding_type);
            float32 res = f / f2;
            std::cout << res;
        } else{
            float32 f(pars.firs_val, pars.rounding_type);
            std::cout << f;
        }
    } else if(pars.FloatHalf){
        if(pars.operation == "+"){
            float16 f(pars.firs_val, pars.rounding_type);
            float16 f2(pars.second_val, pars.rounding_type);
            float16 res = f + f2;
            std::cout << res;
        } else if(pars.operation == "*" or pars.operation == "M"){
            float16 f(pars.firs_val, pars.rounding_type);
            float16 f2(pars.second_val, pars.rounding_type);
            float16 res = f * f2;
            std::cout << res;
        } else if(pars.operation == "-"){
            float16 f(pars.firs_val, pars.rounding_type);
            float16 f2(pars.second_val, pars.rounding_type);
            float16 res = f - f2;
            std::cout << res;
        }
        else if(pars.operation == "/"){
            float16 f(pars.firs_val, pars.rounding_type);
            float16 f2(pars.second_val, pars.rounding_type);
            float16 res = f / f2;
            std::cout << res;
        } else{
            float16 f(pars.firs_val, pars.rounding_type);
            std::cout << f;
        }
    }



    return 0;
}
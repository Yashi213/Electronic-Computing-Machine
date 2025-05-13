#include <cstring>
#include "Parsing.h"

int64_t get_right_value(uint64_t num, int a, int b){
    num <<= 64 - (a + b);
    int64_t x = (int64_t) num;
    x >>= 64 - (a + b);
    return x;
}

void Parsing::ParseFormat(std::string format){
    std::string A;
    std::string B;
    bool flag = true;
    for(int i = 0; i < format.length(); i++){
        if(isdigit(format[i]) && flag){
            A += format[i];
        } else if(!isdigit(format[i])){
            flag = false;
        } else{
            B+=format[i];
        }
    }
    this->format[0] = std::stoi(A);
    this->format[1] = std::stoi(B);
}

Parsing::Parsing(int argc, char** argv) {
    rounding_type = std::stoi(argv[2]);
    if(std::strcmp(argv[1], "f") == 0){
        FloatSingle = true;
        firs_val = std::stoul(argv[3], nullptr, 16);
        if (argc == 4) {
            action = false;
        } else if (argc == 6) {
            operation = argv[4];
            second_val = std::stoul(argv[5], nullptr, 16);
            action = true;
        } else {
            throw "Bad data";
        }
    } else if ((std::strcmp(argv[1], "h") == 0)){
        FloatHalf = true;
        firs_val = std::stoul(argv[3], nullptr, 16);
        if (argc == 4) {
            action = false;
        } else if (argc == 6) {
            operation = argv[4];
            second_val = std::stoul(argv[5], nullptr, 16);
            action = true;
        } else {
            throw "Bad data";
        }
    } else{
        this->ParseFormat(argv[1]);
        firs_val = get_right_value(std::stoul(argv[3], nullptr, 16), format[0], format[1]);
        if (argc == 4) {
            action = false;
        } else if (argc == 6) {
            operation = argv[4];
            second_val = get_right_value(std::stoul(argv[5], nullptr, 16), format[0], format[1]);
            action = true;
        } else {
            throw "Bad data";
        }
    }
    // firs_val = firs_val << (32 - format[0] - format[1]);
    // firs_val = firs_val >> (32 - format[0] - format[1]);
    // second_val = second_val << (32 - format[0] - format[1]);
    // second_val = second_val >> (32 - format[0] - format[1]);
}

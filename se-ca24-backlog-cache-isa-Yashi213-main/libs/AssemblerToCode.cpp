#include <fstream>
#include <sstream>
#include "AssemblerToCode.h"

AssemblerToCode::AssemblerToCode(std::vector <std::vector<std::string>> &commands_, std::string& bin_file_){
    commands = commands_;
    bin_file = bin_file_;
    registers.resize(29);
    std::fill(registers.begin(), registers.end(), 0);
    for(int i = 0; i < 16; i++){
        cache[i] = std::vector<line>(4);
    }
}

int AssemblerToCode::GetIndex(std::string& s){
    if(s[0] == 't'){
        return int(s[1]) - '0';
    } else if(s[0] == 'a'){
        return int(s[1]) - '0' + 7;
    } else if(s == "zero"){
        return 28;
    } else{
        return int(s[1]) - '0' + 15;
    }
    return 0;
}

uint32_t GetInteger(std::string& s){
    char *next;
    if(s[0] == '0' && s[1] == 'x'){
        uint32_t value = strtol(const_cast<char* > (s.data()), &next, 16);
        return value;
    }
    return static_cast<uint32_t> (std::stoul(s));
}

void AssemblerToCode::incrementTime() {
    for(int i = 0; i < 16; i++){
        for(int j = 0; j < 4; j++){
            cache[i][j].time++;
        }
    }
}

void AssemblerToCode::DoCodeLRU(std::vector<std::vector<std::string>>& commands_){
    std::ofstream bin(bin_file, std::ios::app);
    for(int i = 0; i < commands_.size(); i++){
        if(commands_[i][0] == "lui"){
            int t = GetIndex(commands_[i][1]);
            registers[GetIndex(commands_[i][1])] = GetInteger(commands_[i][2]) << 12;
        }
        if(commands_[i][0] == "addi"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] + std::stoi(commands_[i][3]);
        }
        if(commands_[i][0] == "add"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] + registers[GetIndex(commands_[i][3])];
        }
        if(commands_[i][0] == "srli"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] >> std::stoi(commands_[i][3]);
        }
        if(commands_[i][0] == "mul"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] * registers[GetIndex(commands_[i][3])];
        }
        if(commands_[i][0] == "lw" || commands_[i][0] == "lh" || commands_[i][0] == "lb" || commands_[i][0] == "lbu" || commands_[i][0] == "lhu"){
            incrementTime();
            try_cache++;
            uint32_t reg = registers[GetIndex(commands_[i][3])] + stoi(commands_[i][2]);
            uint8_t index = reg >> 5 & 15;
            uint16_t tag = reg >> 9 & 1023;
            bool flag = false;
            for(int j = 0; j < 4; j++){
                if(cache[index][j].tag == tag){
                    hit++;
                    cache[index][j].time = 0;
                    flag = true;
                    break;
                }
            }
            if(flag){
                continue;
            }
            int max_index = 0;
            uint64_t max_time = cache[index][0].time;
            for(int j = 0; j < 4; j++){
                if(max_time < cache[index][j].time){
                    max_index = j;
                    max_time = cache[index][j].time;
                }
            }
            cache[index][max_index].tag = tag;
            cache[index][max_index].time = 0;

        }
        if(commands_[i][0] == "sw" || commands_[i][0] == "sb" || commands_[i][0] == "sh"){
            incrementTime();
            try_cache++;
            uint32_t reg = registers[GetIndex(commands_[i][3])] + + stoi(commands_[i][2]);
            uint8_t index = reg >> 5 & 15;
            uint16_t tag = reg >> 9 & 1023;
            int max_index = 0;
            uint64_t max_time = cache[index][0].time;
            bool flag = false;
            for(int j = 0; j < 4; j++){
                if(cache[index][j].tag == tag){
                    hit++;
                    cache[index][j].time = 0;
                    flag = true;
                    break;
                }
            }
            if(flag){
                continue;
            }
            for(int j = 0; j < 4; j++){
                if(max_time < cache[index][j].time){
                    max_index = j;
                    max_time = cache[index][j].time;
                }
            }
            cache[index][max_index].tag = tag;
            cache[index][max_index].time = 0;
        }
        if(commands_[i][0] == "blt"){
            int tmp =  (std::stoi(commands_[i][3])) / 4;
            if(registers[GetIndex(commands_[i][1])] < registers[GetIndex(commands_[i][2])]){
                i = i + tmp - 1;
            }
        }
        if(commands_[i][0] == "auipc"){
            registers[GetIndex(commands_[i][1])] = (i + GetInteger(commands_[i][2])) << 12;
        }
        if(commands_[i][0] == "slti"){
            if(registers[GetIndex(commands_[i][2])] < stoi(commands_[i][3])){
                registers[GetIndex(commands_[i][1])] = 1;
            } else if(registers[GetIndex(commands_[i][1])] == registers[GetIndex(commands_[i][2])]){
                registers[GetIndex(commands_[i][1])] = stoi(commands_[i][3]);
            } else{
                registers[GetIndex(commands_[i][1])] = 0;
            }
        }
        if(commands_[i][0] == "sltiu"){
            if(registers[GetIndex(commands_[i][1])] > registers[GetIndex(commands_[i][2])]){
                registers[GetIndex(commands_[i][1])] = 1;
            } else if(registers[GetIndex(commands_[i][1])] == registers[GetIndex(commands_[i][2])]){
                registers[GetIndex(commands_[i][1])] = abs(stoi(commands_[i][3]));
            } else{
                registers[GetIndex(commands_[i][1])] = 0;
            }
        }
        if(commands_[i][0] == "xori"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] ^ stoi(commands_[i][3]);
        }
        if(commands_[i][0] == "ori"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] | stoi(commands_[i][3]);
        }
        if(commands_[i][0] == "andi"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] & stoi(commands_[i][3]);
        }
        if(commands_[i][0] == "slli"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] << stoi(commands_[i][3]);
        }
        if(commands_[i][0] == "srai"){
            registers[GetIndex(commands_[i][1])] = abs(registers[GetIndex(commands_[i][2])] >> stoi(commands_[i][3]));
        }
        if(commands_[i][0] == "sub"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] - registers[GetIndex(commands_[i][3])];
        }
        if(commands_[i][0] == "sll"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] << registers[GetIndex(commands_[i][3])];
        }
        if(commands_[i][0] == "slt"){
            if(registers[GetIndex(commands_[i][1])] < registers[GetIndex(commands_[i][2])]){
                registers[GetIndex(commands_[i][1])] = 1;
            } else if(registers[GetIndex(commands_[i][1])] == registers[GetIndex(commands_[i][2])]){
                registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])];
            } else{
                registers[GetIndex(commands_[i][1])] = 0;
            }
        }
        if(commands_[i][0] == "sltu"){
            if(registers[GetIndex(commands_[i][1])] < registers[GetIndex(commands_[i][2])]){
                registers[GetIndex(commands_[i][1])] = 1;
            } else if(registers[GetIndex(commands_[i][1])] == registers[GetIndex(commands_[i][2])]){
                registers[GetIndex(commands_[i][1])] = abs(registers[GetIndex(commands_[i][2])]);
            } else{
                registers[GetIndex(commands_[i][1])] = 0;
            }
        }
        if(commands_[i][0] == "xor"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] ^ registers[GetIndex(commands_[i][3])];
        }
        if(commands_[i][0] == "srl"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] << registers[GetIndex(commands_[i][3])];
        }
        if(commands_[i][0] == "sra"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] >> registers[GetIndex(commands_[i][3])];
        }
        if(commands_[i][0] == "ori"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] | registers[GetIndex(commands_[i][3])];
        }
        if(commands_[i][0] == "andi"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] & registers[GetIndex(commands_[i][3])];
        }
        if(commands_[i][0] == "beq"){
            int tmp =  (std::stoi(commands_[i][3])) / 4;
            if(registers[GetIndex(commands_[i][1])] == registers[GetIndex(commands_[i][2])]){
                i = i + tmp - 1;
            }
        }
        if(commands_[i][0] == "bne"){
            int tmp =  (std::stoi(commands_[i][3])) / 4;
            if(registers[GetIndex(commands_[i][1])] != registers[GetIndex(commands_[i][2])]){
                i = i + tmp - 1;
            }
        }
        if(commands_[i][0] == "bge"){
            int tmp =  (std::stoi(commands_[i][3])) / 4;
            if(registers[GetIndex(commands_[i][1])] >= registers[GetIndex(commands_[i][2])]){
                i = i + tmp - 1;
            }
        }
        if(commands_[i][0] == "bltu"){
            int tmp =  (std::stoi(commands_[i][3])) / 4;
            if(abs(registers[GetIndex(commands_[i][1])]) < abs(registers[GetIndex(commands_[i][2])])){
                i = i + tmp - 1;
            }
        }
        if(commands_[i][0] == "bltu"){
            int tmp =  (std::stoi(commands_[i][3])) / 4;
            if(abs(registers[GetIndex(commands_[i][1])]) >= abs(registers[GetIndex(commands_[i][2])])){
                i = i + tmp - 1;
            }
        }
        if(commands_[i][0] == "mulh"){
            registers[GetIndex(commands_[i][1])] = (registers[GetIndex(commands_[i][2])] * (registers[GetIndex(commands_[i][3])])) >> 16;
        }
        if(commands_[i][0] == "mulhsu"){
            registers[GetIndex(commands_[i][1])] = (registers[GetIndex(commands_[i][2])] * abs(registers[GetIndex(commands_[i][3])])) >> 16;
        }
        if(commands_[i][0] == "mulhu"){
            registers[GetIndex(commands_[i][1])] = (abs(registers[GetIndex(commands_[i][2])]) * abs(registers[GetIndex(commands_[i][3])])) >> 16;
        }
        if(commands_[i][0] == "div"){
            registers[GetIndex(commands_[i][1])] = (registers[GetIndex(commands_[i][2])] / (registers[GetIndex(commands_[i][3])]));
        }
        if(commands_[i][0] == "divu"){
            registers[GetIndex(commands_[i][1])] = (abs(registers[GetIndex(commands_[i][2])]) / abs(registers[GetIndex(commands_[i][3])]));
        }
        if(commands_[i][0] == "rem"){
            registers[GetIndex(commands_[i][1])] = (registers[GetIndex(commands_[i][2])] % (registers[GetIndex(commands_[i][3])]));
        }
        if(commands_[i][0] == "remu"){
            registers[GetIndex(commands_[i][1])] = (abs(registers[GetIndex(commands_[i][2])]) % abs(registers[GetIndex(commands_[i][3])]));
        }
        if(commands_[i][0] == "jal"){
            registers[GetIndex(commands_[i][1])] = i + 4;
            i += stoi(commands_[i][2]);
        }
        if(commands_[i][0] == "jalr"){
            registers[GetIndex(commands_[i][1])] = i + 4;
            i = stoi(commands_[i][2]) & ~1;
        }

    }
    std::cout << "LRU\thit rate: ";
    printf("%3.4f%%", float(hit) / float(try_cache) * 100);
    std::cout << '\n';
}

void AssemblerToCode::UpdateFlags(uint32_t index){
    for(int j = 0; j < 4; j++){
        if(!cache[index][j].flag){
            return;
        }
    }
    for(int j = 0; j < 4; j++){
        cache[index][j].flag = false;
    }
}

int AssemblerToCode::getRegisterIndex(std::string& s){
    if(s[0] == 't'){
        if(int(s[1]) - '0' <= 2){
            return int(s[1]) - '0' + 5;
        } else{
            return int(s[1]) - '0' + 25;
        }
    } else if(s[0] == 's'){
        if(int(s[1]) - '0' == 0 || int(s[1]) - '0' == 1){
            return int(s[1]) - '0' + 8;
        } else{
            return int(s[1]) - '0' + 16;
        }
    } else if(s[0] == 'a') {
        return int(s[1]) - '0' + 10;
    }
    return 0;
}

void AssemblerToCode::DoCodePLRU(std::vector<std::vector<std::string>> &commands_) {
    std::ofstream bin(bin_file, std::ios::in);
    bin.close();
    bin.open(bin_file, std::ios::out | std::ios::trunc);
    for(int i = 0; i < commands_.size(); i++){
        uint32_t machine;
        if(commands_[i][0] == "lui"){
            registers[GetIndex(commands_[i][1])] = GetInteger(commands_[i][2]) << 12;
            machine = GetInteger(commands_[i][2]) << 12 | (getRegisterIndex(commands_[i][1]) << 7) | 55;
        }
        if(commands_[i][0] == "addi"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] + std::stoi(commands_[i][3]);
            machine = GetInteger(commands_[i][3]) << 20 | (getRegisterIndex(commands_[i][1]) << 15) | (getRegisterIndex(commands_[i][1]) << 7) | (19);
        }
        if(commands_[i][0] == "add"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] + registers[GetIndex(commands_[i][3])];
            machine = getRegisterIndex(commands_[i][3]) << 20 | getRegisterIndex(commands_[i][2]) << 15 | getRegisterIndex(commands_[i][1]) << 7 | 51;
        }
        if(commands_[i][0] == "srli"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] >> std::stoi(commands_[i][3]);
            machine = std::stoi(commands_[i][3]) << 20 | getRegisterIndex(commands_[i][2]) << 15 | (5 << 12) |
                    getRegisterIndex(commands_[i][1]) << 7 | 19;
        }
        if(commands_[i][0] == "mul"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] * registers[GetIndex(commands_[i][3])];
            machine = (1 << 25) | getRegisterIndex(commands_[i][3]) << 20 | getRegisterIndex(commands_[i][2]) << 15 |
                    getRegisterIndex(commands_[i][1]) << 7 | 51;
        }
        if(commands_[i][0] == "lw" | commands_[i][0] == "lb" | commands_[i][0] == "lh" | commands_[i][0] == "lbu" | commands_[i][0] == "lhu"){
            try_cache++;
            machine = std::stoi(commands_[i][2]) << 20 | getRegisterIndex(commands_[i][3]) << 15 | (2 << 12) |
                    getRegisterIndex(commands_[i][1]) << 7 | 3;
            uint32_t reg = registers[GetIndex(commands_[i][3])] + stoi(commands_[i][2]);
            uint8_t index = reg >> 5 & 15;
            uint16_t tag = reg >> 9 & 1023;
            bool flag = false;
            for(int j = 0; j < 4; j++){
                if(cache[index][j].tag == tag){
                    hit++;
                    cache[index][j].flag = true;
                    flag = true;
                    UpdateFlags(index);
                    cache[index][j].flag = true;
                    break;
                }
            }
            if(flag){
                continue;
            }
            int32_t max_index = -1;
            for(int j = 0; j < 4; j++){
                if(!cache[index][j].flag){
                    max_index = j;
                    break;
                }
            }

            cache[index][max_index].tag = tag;
            cache[index][max_index].flag = true;
            UpdateFlags(index);
            cache[index][max_index].flag = true;
        }
        if(commands_[i][0] == "sw" | commands_[i][0] == "sb" | commands_[i][0] == "su"){
            try_cache++;
            machine = (std::stoi(commands_[i][2])) >> 5 << 25 | getRegisterIndex(commands_[i][1]) << 20 |
                    getRegisterIndex(commands_[i][3]) << 15 | (2 << 12) | (std::stoi(commands_[i][2]) & 31) << 7 | 35;
            uint32_t reg = registers[GetIndex(commands_[i][3])] + stoi(commands_[i][2]);
            uint8_t index = reg >> 5 & 15;
            uint16_t tag = reg >> 9 & 1023;
            bool flag = false;
            for(int j = 0; j < 4; j++){
                if(cache[index][j].tag == tag){
                    hit++;
                    cache[index][j].flag = true;
                    flag = true;
                    UpdateFlags(index);
                    cache[index][j].flag = true;
                    break;
                }
            }
            if(flag){
                continue;
            }
            int max_index = -1;
            for(int j = 0; j < 4; j++){
                if(!cache[index][j].flag){
                    max_index = j;
                    break;
                }
            }

            cache[index][max_index].tag = tag;
            cache[index][max_index].flag = true;
            UpdateFlags(index);
            cache[index][max_index].flag = true;
        }
        if(commands_[i][0] == "blt"){
            int tmp =  (std::stoi(commands_[i][3])) / 4;
            std::bitset<12> b(std::stoi(commands_[i][3]));
            int32_t offset = std::stoi(commands_[i][3]);
            if(registers[GetIndex(commands_[i][1])] < registers[GetIndex(commands_[i][2])]){
                i = i + tmp - 1;
            }
        }
        if(commands_[i][0] == "auipc"){
            registers[GetIndex(commands_[i][1])] = (i + GetInteger(commands_[i][2])) << 12;
        }
        if(commands_[i][0] == "slti"){
            if(registers[GetIndex(commands_[i][2])] < stoi(commands_[i][3])){
                registers[GetIndex(commands_[i][1])] = 1;
            } else if(registers[GetIndex(commands_[i][1])] == registers[GetIndex(commands_[i][2])]){
                registers[GetIndex(commands_[i][1])] = stoi(commands_[i][3]);
            } else{
                registers[GetIndex(commands_[i][1])] = 0;
            }
        }
        if(commands_[i][0] == "sltiu"){
            if(registers[GetIndex(commands_[i][1])] > registers[GetIndex(commands_[i][2])]){
                registers[GetIndex(commands_[i][1])] = 1;
            } else if(registers[GetIndex(commands_[i][1])] == registers[GetIndex(commands_[i][2])]){
                registers[GetIndex(commands_[i][1])] = abs(stoi(commands_[i][3]));
            } else{
                registers[GetIndex(commands_[i][1])] = 0;
            }
        }
        if(commands_[i][0] == "xori"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] ^ stoi(commands_[i][3]);
        }
        if(commands_[i][0] == "ori"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] | stoi(commands_[i][3]);
        }
        if(commands_[i][0] == "andi"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] & stoi(commands_[i][3]);
        }
        if(commands_[i][0] == "slli"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] << stoi(commands_[i][3]);
        }
        if(commands_[i][0] == "srai"){
            registers[GetIndex(commands_[i][1])] = abs(registers[GetIndex(commands_[i][2])] >> stoi(commands_[i][3]));
        }
        if(commands_[i][0] == "sub"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] - registers[GetIndex(commands_[i][3])];
        }
        if(commands_[i][0] == "sll"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] << registers[GetIndex(commands_[i][3])];
        }
        if(commands_[i][0] == "slt"){
            if(registers[GetIndex(commands_[i][1])] < registers[GetIndex(commands_[i][2])]){
                registers[GetIndex(commands_[i][1])] = 1;
            } else if(registers[GetIndex(commands_[i][1])] == registers[GetIndex(commands_[i][2])]){
                registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])];
            } else{
                registers[GetIndex(commands_[i][1])] = 0;
            }
        }
        if(commands_[i][0] == "sltu"){
            if(registers[GetIndex(commands_[i][1])] < registers[GetIndex(commands_[i][2])]){
                registers[GetIndex(commands_[i][1])] = 1;
            } else if(registers[GetIndex(commands_[i][1])] == registers[GetIndex(commands_[i][2])]){
                registers[GetIndex(commands_[i][1])] = abs(registers[GetIndex(commands_[i][2])]);
            } else{
                registers[GetIndex(commands_[i][1])] = 0;
            }
        }
        if(commands_[i][0] == "xor"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] ^ registers[GetIndex(commands_[i][3])];
        }
        if(commands_[i][0] == "srl"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] << registers[GetIndex(commands_[i][3])];
        }
        if(commands_[i][0] == "sra"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] >> registers[GetIndex(commands_[i][3])];
        }
        if(commands_[i][0] == "ori"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] | registers[GetIndex(commands_[i][3])];
        }
        if(commands_[i][0] == "andi"){
            registers[GetIndex(commands_[i][1])] = registers[GetIndex(commands_[i][2])] & registers[GetIndex(commands_[i][3])];
        }
        if(commands_[i][0] == "beq"){
            int tmp =  (std::stoi(commands_[i][3])) / 4;
            if(registers[GetIndex(commands_[i][1])] == registers[GetIndex(commands_[i][2])]){
                i = i + tmp - 1;
            }
        }
        if(commands_[i][0] == "bne"){
            int tmp =  (std::stoi(commands_[i][3])) / 4;
            if(registers[GetIndex(commands_[i][1])] != registers[GetIndex(commands_[i][2])]){
                i = i + tmp - 1;
            }
        }
        if(commands_[i][0] == "bge"){
            int tmp =  (std::stoi(commands_[i][3])) / 4;
            if(registers[GetIndex(commands_[i][1])] >= registers[GetIndex(commands_[i][2])]){
                i = i + tmp - 1;
            }
        }
        if(commands_[i][0] == "bltu"){
            int tmp =  (std::stoi(commands_[i][3])) / 4;
            if(abs(registers[GetIndex(commands_[i][1])]) < abs(registers[GetIndex(commands_[i][2])])){
                i = i + tmp - 1;
            }
        }
        if(commands_[i][0] == "bltu"){
            int tmp =  (std::stoi(commands_[i][3])) / 4;
            if(abs(registers[GetIndex(commands_[i][1])]) >= abs(registers[GetIndex(commands_[i][2])])){
                i = i + tmp - 1;
            }
        }
        if(commands_[i][0] == "mulh"){
            registers[GetIndex(commands_[i][1])] = (registers[GetIndex(commands_[i][2])] * (registers[GetIndex(commands_[i][3])])) >> 16;
        }
        if(commands_[i][0] == "mulhsu"){
            registers[GetIndex(commands_[i][1])] = (registers[GetIndex(commands_[i][2])] * abs(registers[GetIndex(commands_[i][3])])) >> 16;
        }
        if(commands_[i][0] == "mulhu"){
            registers[GetIndex(commands_[i][1])] = (abs(registers[GetIndex(commands_[i][2])]) * abs(registers[GetIndex(commands_[i][3])])) >> 16;
        }
        if(commands_[i][0] == "div"){
            registers[GetIndex(commands_[i][1])] = (registers[GetIndex(commands_[i][2])] / (registers[GetIndex(commands_[i][3])]));
        }
        if(commands_[i][0] == "divu"){
            registers[GetIndex(commands_[i][1])] = (abs(registers[GetIndex(commands_[i][2])]) / abs(registers[GetIndex(commands_[i][3])]));
        }
        if(commands_[i][0] == "rem"){
            registers[GetIndex(commands_[i][1])] = (registers[GetIndex(commands_[i][2])] % (registers[GetIndex(commands_[i][3])]));
        }
        if(commands_[i][0] == "remu"){
            registers[GetIndex(commands_[i][1])] = (abs(registers[GetIndex(commands_[i][2])]) % abs(registers[GetIndex(commands_[i][3])]));
        }
        if(commands_[i][0] == "jal"){
            registers[GetIndex(commands_[i][1])] = i + 4;
            i += stoi(commands_[i][2]);
        }
        if(commands_[i][0] == "jalr"){
            registers[GetIndex(commands_[i][1])] = i + 4;
            i = stoi(commands_[i][2]) & ~1;
        }
    }
    std::cout << "pLRU\thit rate: ";
    printf("%3.4f%%", float(hit) / float(try_cache) * 100);
}

std::string ParseBinCommand(uint32_t command){
    std::string digits[16] = {"0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"};
    std::string hex;
    do
    {
        hex.insert (0, digits[command % 16]);
        command /= 16;
    }
    while (command!=0);
    std::string new_hex;
    for(int i = 0; i < hex.length(); i+=2){
        if(hex[hex.length() - i - 2] != NULL){
            new_hex.push_back(hex[hex.length() - i - 2]);
        } else{
            new_hex.push_back('0');
        }
        if(hex[hex.length() - i - 1] != NULL){
            new_hex.push_back(hex[hex.length() - i - 1]);
        } else{
            new_hex.push_back('0');
        }
        new_hex.push_back(' ');
    }
    return new_hex;
}

void AssemblerToCode::GetBinFile(){
    std::ofstream bin(bin_file, std::ios::in);
    bin.close();
    bin.open(bin_file, std::ios::out | std::ios::binary);
    for(int i = 0; i < commands.size(); i++){
        uint32_t machine;
        if(commands[i][0] == "lui"){
            machine = GetInteger(commands[i][2]) << 12 | (getRegisterIndex(commands[i][1]) << 7) | 55;
        }
        if(commands[i][0] == "addi"){
            machine = GetInteger(commands[i][3]) << 20 | (getRegisterIndex(commands[i][1]) << 15) | (getRegisterIndex(commands[i][1]) << 7) | (19);
        }
        if(commands[i][0] == "add"){
            machine = getRegisterIndex(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 15 | getRegisterIndex(commands[i][1]) << 7 | 51;
        }
        if(commands[i][0] == "srli"){
            machine = std::stoi(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 15 | (5 << 12) |
                      getRegisterIndex(commands[i][1]) << 7 | 19;
        }
        if(commands[i][0] == "mul"){
            machine = (1 << 25) | getRegisterIndex(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 15 |
                      getRegisterIndex(commands[i][1]) << 7 | 51;
        }
        if(commands[i][0] == "lw"){
            machine = std::stoi(commands[i][2]) << 20 | getRegisterIndex(commands[i][3]) << 15 | (2 << 12) |
                      getRegisterIndex(commands[i][1]) << 7 | 3;
        }
        if(commands[i][0] == "sw"){
            machine = (std::stoi(commands[i][2])) >> 5 << 25 | getRegisterIndex(commands[i][1]) << 20 |
                      getRegisterIndex(commands[i][3]) << 15 | (2 << 12) | (std::stoi(commands[i][2]) & 31) << 7 | 35;
        }
        if(commands[i][0] == "blt"){
            int32_t offset = std::stoi(commands[i][3]);
            machine = (((offset >> 12) & 1) << 31) | (((offset >> 5) & 0x3f) << 25) | getRegisterIndex(commands[i][2]) << 20 |
                    getRegisterIndex(commands[i][1]) << 15 | 4 << 12 | (((offset >> 1) & 0xf) << 8) | (((offset >> 11) & 1) << 7) | 99;
        }
        if(commands[i][0] == "auipc"){
            machine = stoi(commands[i][2]) << 12 | getRegisterIndex(commands[i][i]) << 7 | 23;
        }
        if(commands[i][0] == "slti"){
            machine = stoi(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 14 | 2 << 12 | getRegisterIndex(commands[i][1]) << 7
                    | 19;
        }
        if(commands[i][0] == "sltiu"){
            machine = stoi(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 14 | 3 << 12 | getRegisterIndex(commands[i][1]) << 7
                      | 19;
        }
        if(commands[i][0] == "xori"){
            machine = stoi(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 14 | 4 << 12 | getRegisterIndex(commands[i][1]) << 7
                      | 19;
        }
        if(commands[i][0] == "ori"){
            machine = stoi(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 14 | 6 << 12 | getRegisterIndex(commands[i][1]) << 7
                      | 19;
        }
        if(commands[i][0] == "andi"){
            machine = stoi(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 14 | 7 << 12 | getRegisterIndex(commands[i][1]) << 7
                      | 19;
        }
        if(commands[i][0] == "slli"){
            machine = stoi(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 15 | 1 << 12 | getRegisterIndex(commands[i][1]) << 7 |
                    19;
        }
        if(commands[i][0] == "srli"){
            machine = stoi(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 15 | 5 << 12 | getRegisterIndex(commands[i][1]) << 7 |
                      19;
        }
        if(commands[i][0] == "srai"){
            machine = 8 << 27 | stoi(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 15 | 5 << 12 | getRegisterIndex(commands[i][1]) << 7 |
                      19;
        }
        if(commands[i][0] == "sub"){
            machine = 8 << 27 | getRegisterIndex(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 15 | getRegisterIndex(commands[i][1]) << 7 | 51;
        }
        if(commands[i][0] == "sll"){
            machine = getRegisterIndex(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 15 | 1 << 12 | getRegisterIndex(commands[i][1]) << 7 | 51;
        }
        if(commands[i][0] == "slt"){
            machine = getRegisterIndex(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 15 | 2 << 12 | getRegisterIndex(commands[i][1]) << 7 | 51;
        }
        if(commands[i][0] == "sltu"){
            machine = getRegisterIndex(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 15 | 3 << 12 | getRegisterIndex(commands[i][1]) << 7 | 51;
        }
        if(commands[i][0] == "xor"){
            machine = getRegisterIndex(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 15 | 4 << 12 | getRegisterIndex(commands[i][1]) << 7 | 51;
        }
        if(commands[i][0] == "srl"){
            machine = getRegisterIndex(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 15 | 5 << 12 | getRegisterIndex(commands[i][1]) << 7 | 51;
        }
        if(commands[i][0] == "sra"){
            machine = 8 << 27 | getRegisterIndex(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 15 | 5 << 12 | getRegisterIndex(commands[i][1]) << 7 | 51;
        }
        if(commands[i][0] == "or"){
            machine = getRegisterIndex(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 15 | 6 << 12 | getRegisterIndex(commands[i][1]) << 7 | 51;
        }
        if(commands[i][0] == "and"){
            machine = getRegisterIndex(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 15 | 7 << 12 | getRegisterIndex(commands[i][1]) << 7 | 51;
        }
        if(commands[i][0] == "lb"){
            machine = std::stoi(commands[i][2]) << 20 | getRegisterIndex(commands[i][3]) << 15 |
                      getRegisterIndex(commands[i][1]) << 7 | 3;
        }
        if(commands[i][0] == "lh"){
            machine = std::stoi(commands[i][2]) << 20 | getRegisterIndex(commands[i][3]) << 15 | (1 << 12) |
                      getRegisterIndex(commands[i][1]) << 7 | 3;
        }
        if(commands[i][0] == "lbu"){
            machine = std::stoi(commands[i][2]) << 20 | getRegisterIndex(commands[i][3]) << 15 | (4 << 12) |
                      getRegisterIndex(commands[i][1]) << 7 | 3;
        }
        if(commands[i][0] == "lhu"){
            machine = std::stoi(commands[i][2]) << 20 | getRegisterIndex(commands[i][3]) << 15 | (5 << 12) |
                      getRegisterIndex(commands[i][1]) << 7 | 3;
        }
        if(commands[i][0] == "sb"){
            machine = (std::stoi(commands[i][2])) >> 5 << 25 | getRegisterIndex(commands[i][1]) << 20 |
                      getRegisterIndex(commands[i][3]) << 15 | (std::stoi(commands[i][2]) & 31) << 7 | 35;
        }
        if(commands[i][0] == "sh"){
            machine = (std::stoi(commands[i][2])) >> 5 << 25 | getRegisterIndex(commands[i][1]) << 20 |
                      getRegisterIndex(commands[i][3]) << 15 | (1 << 12) | (std::stoi(commands[i][2]) & 31) << 7 | 35;
        }
        if(commands[i][0] == "jal"){
            int32_t offset = stoi(commands[i][3]);
            machine = (((offset >> 20) & 1) << 31) | (((offset >> 10) & 0x7ff) << 21) | (((offset >> 11) & 1) << 20) |
                    (((offset >> 19) & 0xff00) << 12) | getRegisterIndex(commands[i][1]) << 7 | 111;
        }
        if(commands[i][0] == "jalr"){
            machine = stoi(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 15 | getRegisterIndex(commands[i][1]) << 7
                    | 111;
        }
        if(commands[i][0] == "beq"){
            int32_t offset = std::stoi(commands[i][3]);
            machine = (((offset >> 12) & 1) << 31) | (((offset >> 5) & 0x3f) << 25) | getRegisterIndex(commands[i][2]) << 20 |
                      getRegisterIndex(commands[i][1]) << 15 | (((offset >> 1) & 0xf) << 8) | (((offset >> 11) & 1) << 7) | 99;
        }
        if(commands[i][0] == "bne"){
            int32_t offset = std::stoi(commands[i][3]);
            machine = (((offset >> 12) & 1) << 31) | (((offset >> 5) & 0x3f) << 25) | getRegisterIndex(commands[i][2]) << 20 |
                      getRegisterIndex(commands[i][1]) << 15 | 1 << 12 | (((offset >> 1) & 0xf) << 8) | (((offset >> 11) & 1) << 7) | 99;
        }
        if(commands[i][0] == "bge"){
            int32_t offset = std::stoi(commands[i][3]);
            machine = (((offset >> 12) & 1) << 31) | (((offset >> 5) & 0x3f) << 25) | getRegisterIndex(commands[i][2]) << 20 |
                      getRegisterIndex(commands[i][1]) << 15 | 5 << 12 | (((offset >> 1) & 0xf) << 8) | (((offset >> 11) & 1) << 7) | 99;
        }
        if(commands[i][0] == "bltu"){
            int32_t offset = std::stoi(commands[i][3]);
            machine = (((offset >> 12) & 1) << 31) | (((offset >> 5) & 0x3f) << 25) | getRegisterIndex(commands[i][2]) << 20 |
                      getRegisterIndex(commands[i][1]) << 15 | 6 << 12 | (((offset >> 1) & 0xf) << 8) | (((offset >> 11) & 1) << 7) | 99;
        }
        if(commands[i][0] == "bgeu"){
            int32_t offset = std::stoi(commands[i][3]);
            machine = (((offset >> 12) & 1) << 31) | (((offset >> 5) & 0x3f) << 25) | getRegisterIndex(commands[i][2]) << 20 |
                      getRegisterIndex(commands[i][1]) << 15 | 7 << 12 | (((offset >> 1) & 0xf) << 8) | (((offset >> 11) & 1) << 7) | 99;
        }
        if(commands[i][0] == "mulh"){
            machine = (1 << 25) | getRegisterIndex(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 15 |
                      1 << 7 | getRegisterIndex(commands[i][1]) << 7 | 51;
        }
        if(commands[i][0] == "mulhsu"){
            machine = (1 << 25) | getRegisterIndex(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 15 |
                      2 << 7 | getRegisterIndex(commands[i][1]) << 7 | 51;
        }
        if(commands[i][0] == "mulhu"){
            machine = (1 << 25) | getRegisterIndex(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 15 |
                      3 << 7 | getRegisterIndex(commands[i][1]) << 7 | 51;
        }
        if(commands[i][0] == "div"){
            machine = (1 << 25) | getRegisterIndex(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 15 |
                      4 << 7 | getRegisterIndex(commands[i][1]) << 7 | 51;
        }
        if(commands[i][0] == "divu") {
            machine = (1 << 25) | getRegisterIndex(commands[i][3]) << 20 | getRegisterIndex(commands[i][2]) << 15 |
                      5 << 7 | getRegisterIndex(commands[i][1]) << 7 | 51;
        }
        std::ofstream bin2("bin.txt", std::ios::in);
        bin2.close();
        bin2.open("bin.txt", std::ios::out | std::ios::binary);
        bin2 << ParseBinCommand(machine) << '\n';
        bin.write(reinterpret_cast<const char *>(&machine), 4);
    }
}
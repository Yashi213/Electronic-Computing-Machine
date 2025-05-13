#include <iostream>
#include <vector>
#include <cstdint>
#include <bitset>

struct line{
    uint32_t data;
    int32_t tag = -1;
    uint64_t time = 0;
    bool flag = false;
    line(): time(0){};
};

class AssemblerToCode{
public:
    explicit AssemblerToCode(std::vector<std::vector<std::string>>& commands_, std::string& bin_file);
    void DoCodeLRU(std::vector<std::vector<std::string>>& commands);
    void GetBinFile();
    void DoCodePLRU(std::vector<std::vector<std::string>>& commands);
    uint32_t hit = 0;
    uint32_t try_cache = 0;
protected:
    std::string bin_file;
    void incrementTime();
    void UpdateFlags(uint32_t index);
    int GetIndex(std::string& s);
    int getRegisterIndex(std::string& s);
    std::vector<std::vector<std::string>> commands;
    std::vector<int64_t> registers;
    std::vector<std::vector<line>> cache = std::vector<std::vector<line>>(16);
};

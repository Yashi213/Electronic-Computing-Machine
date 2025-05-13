#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>

class Parse{
protected:
    std::string asm_path;
    std::vector<int> RAM;
    void GetCommands();
public:
    std::string bin_file;
    int repl_type;
    std::vector<std::vector<std::string>> commands;
    Parse(int argc, char** argv);
};

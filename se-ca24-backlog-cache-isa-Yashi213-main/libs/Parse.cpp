#include "Parse.h"

void Parse::GetCommands() {
    std::ifstream file(asm_path);
    std::string s;
    while (std::getline(file, s)){
        char* words = strtok(const_cast<char *>(s.data()), " ,  ");
        std::vector<std::string> tmp;
        while (words != nullptr){
            tmp.emplace_back(words);
            words = std::strtok(nullptr, " ,    ");
        }
        commands.push_back(tmp);
    }
}

Parse::Parse(int argc, char** argv) {
    for(int i = 1; i < argc; i++){
        if(std::string (argv[i]) == "--asm"){
            asm_path = argv[i + 1];
            GetCommands();
        }
        if(std::string (argv[i]) == "--replacement"){
            repl_type = atoi(argv[i + 1]);
        }
        if(std::string (argv[i]) == "--bin"){
            bin_file = argv[i + 1];
        }
    }
}
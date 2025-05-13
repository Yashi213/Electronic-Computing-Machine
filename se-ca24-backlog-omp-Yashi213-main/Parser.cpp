#include "Parser.h"

Parser::Parser(char **argv, int argc) {
    this->argv = argv;
    this->argc = argc;
}

void Parser::parse() {
    for(int i = 0; i < argc; i++){
        if(std::strcmp(argv[i], "--no-omp") == 0){
            threads = 1;
        } else if(std::strcmp(argv[i], "--omp-threads") == 0){
            if(std::strcmp(argv[i + 1], "default") == 0){
                default_threads = true;
                threads = default_threads_count;
            } else {
                is_threads = true;
                threads = atoi(argv[i + 1]);
            }
        } else if(std::strcmp(argv[i], "--input") == 0){
            input = argv[i + 1];
            std::ifstream in(input);
            std::string str;
            std::getline(in, str, '\n');
            max_iterations = atoi(str.c_str());
        } else if(std::strcmp(argv[i], "--output") == 0){
            output = argv[i + 1];
        }
    }

}


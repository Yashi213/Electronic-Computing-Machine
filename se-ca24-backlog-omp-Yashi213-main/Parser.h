#include <cstdint>
#include <fstream>
#include <string>
#include <cstring>

class Parser {
private:
    int default_threads_count = 4;
    char** argv;
    int argc;
    std::string str_arg;
public:
    uint64_t max_iterations;
    std::string input;
    std::string output;
    int threads;
    bool is_threads = false;
    bool default_threads = false;
    Parser(char** argv, int argc);
    void parse();
};
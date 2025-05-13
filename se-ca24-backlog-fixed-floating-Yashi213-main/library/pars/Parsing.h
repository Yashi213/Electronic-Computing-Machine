#include <string>
#include <cstdint>


class Parsing {
private:
    void ParseFormat(std::string format);
public:
    int format[2];
    bool action = false;
    int rounding_type;
    int64_t firs_val;
    int64_t second_val = 0;
    std::string operation;
    bool FloatSingle = false;
    bool FloatHalf = false;

    Parsing(int argc, char** argv);
};


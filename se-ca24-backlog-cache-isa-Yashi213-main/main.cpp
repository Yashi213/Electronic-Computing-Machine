#include <iostream>
#include "libs/Parse.h"
#include "libs/AssemblerToCode.h"


int main(int argc, char** argv){
    Parse h = Parse(argc, argv);
    AssemblerToCode a(h.commands, h.bin_file);
    if(h.repl_type == 0){
        a.DoCodeLRU(h.commands);
        AssemblerToCode b(h.commands, h.bin_file);
        b.DoCodePLRU(h.commands);
    } else if(h.repl_type == 1){
        a.DoCodeLRU(h.commands);
    } else{
        a.DoCodePLRU(h.commands);
    }
    a.GetBinFile();
    return 0;
}
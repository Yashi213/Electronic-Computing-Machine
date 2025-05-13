#include "Parser.h"
#include "hit.h"
#include "Calculate.h"



int main(int argc, char** argv) {
    const float* a;
    a = get_axis_range();
    Parser pars(argv, argc);
    pars.parse();
    Calculate c(a, pars.max_iterations, pars.output);
    if(pars.is_threads){
        c.get_volume_omp(pars.threads, false, 1000);
    } else{
        if(pars.default_threads){
            c.get_volume_omp(-1, true, 1000);
        } else{
            c.get_volume();
        }
    }
    return 0;

}
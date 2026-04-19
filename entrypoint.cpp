#include "lextest.h"
#include <cstdio>
#include <cstdlib>

int main(int argc, char **argv) { 
    lx::parse_args(argc, argv);
    return lx::run_all(lx::get_registry()); 
}

#include "lisence.h"
#include <cstdio>
#include <iostream>
#include <cstring>
using namespace std;

//#define FUNC_SIM
//#define MULTICYCLE_SIM
#define PIPELINE_SIM

#ifdef FUNC_SIM
#include "func_sim.h"
Func_Sim sim;
#endif

#ifdef MULTICYCLE_SIM
#include "multicycle_sim.h"
Multicycle_Sim sim;
#endif

#ifdef PIPELINE_SIM
#include "pipeline_sim.h"
Pipeline_Sim sim;
#endif

int main(int argc, char *argv[])
{
    unsigned int verbose = 0;

    char *interested[20] = {NULL};
    char *filename = NULL;
    char elfname[] = "./tmp/elf";
    int n_interested = 0;

    if (argc == 1)
    {
        printf("Insufficient arguments\n");
        help_print(argv[0]);
        return -1;
    }
    else if (argc == 2)  // insufficient arguments
    {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0)
        {
            help_print(argv[0]);
            return -1;
        }
    }
    filename = argv[1];
    for (int i = 2; i < argc; i++)
        if (argv[i][0] == '-')
        {
            if (strcmp(argv[i], "-v") == 0)
                verbose = (verbose > 1) ? verbose : 1;
            else if (strcmp(argv[i], "-d") == 0)
                verbose = (verbose > 2) ? verbose : 2;
            else
            {
                printf("Unknown argument: %s\n", argv[i]);
                help_print(argv[0]);
                return -1;
            }
        }
        else
            interested[n_interested++] = argv[i];

    if (!sim.init(filename, elfname, interested, n_interested))
    {
        printf("No such file: %s\n", filename);
        return -2;
    }
    sim.sim(verbose);
    sim.print_res();

    return 0;
}

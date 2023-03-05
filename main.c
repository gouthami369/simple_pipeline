//
//  main.c
//  Pipeline
//
//

#include <stdio.h>
#include <stdlib.h>
#include "cpu.c"
#include "cpu.h"

int binary_flag;

void run_cpu_fun(FILE* fp){

    CPU *cpu = CPU_init();
    if (!cpu)
    {
        fprintf(stderr," Unable to initialize CPU\n");
        exit(1);
    }
    CPU_run(cpu, fp);
    CPU_stop(cpu);
}

int main(int argc, const char * argv[]) {
    if (argc<=1) {
        fprintf(stderr, "Error : missing required args\n");
        return -1;
    }
    FILE* fp = fopen(argv[1], "r");
    printf("%s", argv[1]);
    run_cpu_fun(fp);
    return 0;
}

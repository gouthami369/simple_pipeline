#ifndef _CPU_H_
#define _CPU_H_
#include <stdbool.h>
#include <assert.h>

typedef struct Register
{
    int value;          // contains register value
    bool is_writing;    // indicate that the register is current being written
    // True: register is not ready
    // False: register is ready
} Register;

typedef struct {
    int opcode;
    int rd;
    int rs1;
    int rs2;
    int imm1;
    int imm2;
    int rs1_value;
    int rs2_value;
    int rd_value;
    int memory_address;
    int has_instr;
    char instr[256];
}CPU_state;

/* Model of CPU */
typedef struct CPU
{
    int clock_cycle;
    bool stall;
    Register *regs; // Integer register file

    /* Pipeline stages */
    CPU_state fetch;
    CPU_state decode;
    CPU_state instr_analyze;
    CPU_state register_read;
    CPU_state adder;
    CPU_state multiplier;
    CPU_state divider;
    CPU_state branch;
    CPU_state memory1;
    CPU_state memory2;
    CPU_state writeback;
} CPU;

CPU* CPU_init();

Register* create_registers(int size);

int CPU_run(CPU* cpu, FILE* fp);

void CPU_stop(CPU* cpu);

#endif

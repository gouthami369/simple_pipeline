#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "cpu.h"

#define REG_COUNT 128
char* RET = "ret";
char* operands[7] = {"set", "add", "sub", "mul", "div", "load", "ret"};

static void clear(CPU* cpu) {
    cpu->fetch.imm1 = 0;
    cpu->fetch.imm2 = 0;
    cpu->fetch.rs1 = 0;
    cpu->fetch.rs2 = 0;
    cpu->fetch.rd = 0;
}

static int get_int_value_from_opcode (char* opcode) {
    for (size_t i = 0; i < sizeof(operands) / sizeof(operands[0]); i++) {
        if (strcmp(opcode, operands[i]) == 0) {
            return i;
        }
    }
    return 6;
}
static void fetch(CPU* cpu) {
    if (cpu->fetch.has_instr) {
        char instr[256];
        strcpy(instr, cpu->fetch.instr);
        char *token;
        printf("IF                    :%s", cpu->fetch.instr);
        strtok(instr, " ");
        token = strtok(NULL, " ");
        cpu->fetch.opcode = get_int_value_from_opcode(token);
        token = strtok(NULL, " ");
        if(token)
            cpu->fetch.rd = atoi(token + 1);
        token = strtok(NULL, " ");
        if (token) {
            if (token[0] == '#') {
                cpu->fetch.imm1 = atoi(token + 1);
            } else {
                cpu->fetch.rs1 = atoi(token + 1);
            }
            token = strtok(NULL, " ");
            if (token) {
                if (token[0] == '#') {
                    cpu->fetch.imm2 = atoi(token + 1);
                } else {
                    cpu->fetch.rs2 = atoi(token + 1);
                }
            }
        }
        if (!cpu->stall) {
            cpu->decode = cpu->fetch;
        }
        if (cpu->fetch.opcode == 6) {
            cpu->fetch.has_instr = false;
        }
//        printf("   opcode: %d", cpu->fetch.opcode);
//        printf("   des: %d", cpu->fetch.rd);
//        printf("   src_reg1 : %d", cpu->fetch.rs1);
//        printf("   src_reg2 : %d", cpu->fetch.rs2);
//        printf("   immd1 : %d", cpu->fetch.imm1);
//        printf("   imm2 : %d", cpu->fetch.imm2);
        clear(cpu);

    }
}

static void decode(CPU* cpu)
{
    int executeNextInstr = false;
    if (cpu->decode.has_instr == 1)
    {
        printf("ID                    :%s", cpu->decode.instr);
        //char* operands[7] = {"set", "add", "sub", "mul", "div", "load", "ret"};
        switch (cpu->decode.opcode) {
            case 0:
                if (cpu->decode.rs1!=0 && cpu->regs[cpu->decode.rs1].is_writing==true)
                    executeNextInstr = true;
                break;
            case 1:
            case 2:
            case 3:
            case 4:
                if ((cpu->decode.rs1!=0 && cpu->regs[cpu->decode.rs1].is_writing==true) &&
                    (cpu->decode.rs2!=0 && cpu->regs[cpu->decode.rs2].is_writing==true))
                    executeNextInstr = true;
                break;
            case 5:
            case 6:
                break;

        }

        if (executeNextInstr) {
            cpu->instr_analyze = cpu->decode;
            cpu->stall = false;
            cpu->decode.has_instr = false;
        } else {
            cpu->stall = true;
        }
    }
}

static void instr_analyze(CPU* cpu)
{
    if (cpu->instr_analyze.has_instr)
    {
        printf("IA                    :%s", cpu->instr_analyze.instr);
        cpu->register_read = cpu->instr_analyze;
        cpu->instr_analyze.has_instr = false;
    }
}

static void register_read(CPU* cpu)
{
    if (cpu->register_read.has_instr)
    {
        printf("RR                    :%s", cpu->register_read.instr);
        cpu->adder = cpu->register_read;
        cpu->register_read.has_instr = false;
    }
}

static void adder(CPU* cpu)
{
    if (cpu->adder.has_instr)
    {
        printf("ADD                   :%s", cpu->adder.instr);
        cpu->multiplier = cpu->adder;
        cpu->adder.has_instr = false;
    }
}

static void multiplier(CPU* cpu)
{
    if (cpu->multiplier.has_instr)
    {
        printf("MUL                   :%s", cpu->multiplier.instr);
        cpu->divider = cpu->multiplier;
        cpu->multiplier.has_instr = false;
    }
}

static void divider(CPU* cpu)
{
    if (cpu->divider.has_instr)
    {
        printf("DIV                   :%s", cpu->divider.instr);
        cpu->branch = cpu->divider;
        cpu->divider.has_instr = false;
    }
}

static void branch(CPU* cpu)
{
    if (cpu->branch.has_instr)
    {
        printf("BR                    :%s", cpu->branch.instr);
        cpu->memory1 = cpu->branch;
        cpu->branch.has_instr = false;
    }
}

static void memory1(CPU* cpu)
{
    if (cpu->memory1.has_instr)
    {
        printf("MEM1                  :%s", cpu->memory1.instr);
        cpu->memory2 = cpu->memory1;
        cpu->memory1.has_instr = false;
    }
}

static void memory2(CPU* cpu)
{
    if (cpu->memory2.has_instr)
    {
        printf("MEM2                  :%s", cpu->memory2.instr);
        cpu->writeback = cpu->memory2;
        cpu->memory2.has_instr = false;
    }
}

static bool writeback(CPU* cpu)
{
    if (cpu->writeback.has_instr)
    {
        printf("WB                    :%s", cpu->writeback.instr);
        cpu->writeback.has_instr = false;
        if (cpu->writeback.opcode==6) {
            return 1; }
    }
    return 0; //default value
}

CPU* CPU_init()
{
    CPU* cpu = malloc(sizeof(*cpu));
    if (!cpu) {
        return NULL;
    }
    /* Create register files */
    cpu->regs= create_registers(REG_COUNT);
    cpu->clock_cycle = 1;
    cpu->fetch.has_instr = true;
    cpu->stall = false;
    return cpu;
}

/*
 * This function de-allocates CPU.
 */
void CPU_stop(CPU* cpu)
{
    free(cpu);
}

/*
 * This function prints the content of the registers.
 */
void print_registers(CPU *cpu){


    printf("================================\n");
    printf("--------------------------------\n");
    for (int reg=0; reg<REG_COUNT; reg++) {
        printf("REG[%2d]   |   Value=%d  \n",reg,cpu->regs[reg].value);
        printf("--------------------------------\n");
    }
    printf("================================\n\n");
}

/*
 *  CPU CPU simulation loop
 */
int CPU_run(CPU* cpu, FILE *fp)
{
//    Register Register[REG_FILE_SIZE] = {{0, 1}}; // Initialize register file with 0 and valid status
    while (1) {
        if (!cpu->stall) {
            fgets(cpu->fetch.instr, sizeof(cpu->fetch.instr), fp);
        }
        printf("===========================================\n");
        printf("Clock Cycle #: %d\n", cpu->clock_cycle++);
        printf("-----------------------------------------------------\n");
        if (writeback(cpu)) {
            break;
        }
        memory2(cpu);
        memory1(cpu);
        branch(cpu);
        divider(cpu);
        multiplier(cpu);
        adder(cpu);
        register_read(cpu);
        instr_analyze(cpu);
        decode(cpu);
        fetch(cpu);
    }

    // Print the contents of the register file after all instructions are executed
//    print_registers(cpu);
    // printing the simulation results
    printf("Stalled cycles due to structural hazard: \n");
    printf("Total execution cycles: %d\n", cpu->clock_cycle);
    printf("Total instruction simulated:\n" );
    printf("IPC: \n");

    return 0;
}

Register* create_registers(int size){
    Register* regs = malloc(sizeof(*regs) * size);
    if (!regs) {
        return NULL;
    }
    for (int i=0; i<size; i++){
        regs[i].value = 0;
        regs[i].is_writing = false;
    }
    return regs;
}


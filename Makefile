all:
	gcc -g -o sim *.c
clean:
	rm -f sim

#char* operands[7] = {"set", "add", "sub", "mul", "div", "load", "ret"};
#
#static void clear(CPU* cpu) {
#    cpu->fetch.imm1 = 0;
#    cpu->fetch.imm2 = 0;
#    cpu->fetch.rs1 = 0;
#    cpu->fetch.rs2 = 0;
#    cpu->fetch.rd = 0;
#}
#
static int get_int_value_from_opcode (char* opcode) {
    printf("opcode: %s", opcode);
    for (size_t i = 0; i < sizeof(operands) / sizeof(operands[0]); i++) {
//        printf("%d", strcmp(opcode, operands[i])==0);
//        printf("%s", operands[i]);
        if(strcmp(opcode, operands[i])==0) {
            printf("%s", operands[i]);
        }
    }
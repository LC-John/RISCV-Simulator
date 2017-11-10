#ifndef EX_H
#define EX_H

#include "reg_def.h"
#include <cstdio>

class EX
{
public:
    EX();
    void set_RegImm(unsigned int arg_rd,
                    REG arg_rs1,
                    REG arg_rs2,
                    unsigned long long arg_imm64);
    void set_pc(REG arg_pc);
    void set_ctrl(INST32_CTRL_BIT arg_ctrl);
    ERROR_NUM BranchCmp();
    ERROR_NUM ALU();
    ERROR_NUM set_next_pc();
    INST32_CTRL_BIT get_ctrl();
    unsigned long long get_ALU_output();
    REG get_pc();
    REG get_next_pc();
    REG get_rs2();
    unsigned int get_rd();
    void print();
    unsigned long long get_cycles();

private:
    REG rs1, rs2;
    unsigned int rd;
    REG pc, next_pc;
    unsigned long long imm64;
    unsigned long long ALU_output;
    INST32_CTRL_BIT ctrl;

    BRUN_CTRL get_BrUn();
    BSEL_CTRL get_BSel();
    ASEL_CTRL get_ASel();
    ALUSEL_CTRL get_ALUSel();
    PCSEL_CTRL get_PCSel();
};

#endif // EX_H

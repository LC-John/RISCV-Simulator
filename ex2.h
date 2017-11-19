#ifndef EX2_H
#define EX2_H

#include "reg_def.h"
#include <cstdio>

class EX2
{
public:
    unsigned char type;
    ERROR_NUM err_no;

    EX2();
    bool get_available();
    void set_available();
    INST32_CTRL_BIT get_ctrl();
    void set_ctrl(INST32_CTRL_BIT arg_ctrl);
    unsigned int get_rd();
    unsigned long long get_ALU_output();
    void set_pc(REG arg_pc);
    REG get_pc();
    void set_Reg(unsigned int arg_rd, unsigned long long arg_ALU_output);
    void print();

private:
    bool available;
    unsigned int rd;
    REG pc;
    unsigned long long ALU_output;
    INST32_CTRL_BIT ctrl;

    ALUSEL_CTRL get_ALUSel();
};

#endif // EX2_H

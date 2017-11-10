#ifndef ID_H
#define ID_H

#include "reg_def.h"
#include <cstdio>

class ID
{
public:
    ID();
    void set_RegImm(IMM_SET arg_imm,
                    unsigned int arg_rd,
                    unsigned int arg_rs1,
                    unsigned int arg_rs2);
    void set_pc(REG arg_pc);
    void set_ctrl(INST32_CTRL_BIT arg_ctrl);
    void read_regfile(TEMPORAL_REG* reg);
    ERROR_NUM imm_extension();
    INST32_CTRL_BIT get_ctrl();
    REG get_rs1_content();
    REG get_rs2_content();
    unsigned long long get_imm64();
    REG get_pc();
    unsigned int get_rd();
    void print();
    unsigned long long get_cycles();

private:
    IMM_SET imm;
    REG pc;
    unsigned int rd, rs1, rs2;
    unsigned long long extimm;
    REG rs1_content, rs2_content;
    INST32_CTRL_BIT ctrl;

    IMMSEL_CTRL get_ImmSel();
};

#endif // ID_H

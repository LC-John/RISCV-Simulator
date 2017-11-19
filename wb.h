#ifndef WB_H
#define WB_H

#include "reg_def.h"
#include <cstdio>

class WB
{
public:
    unsigned char type;
    ERROR_NUM err_no;

    WB();
    void set_pc(REG arg_pc);
    void set_Reg(unsigned int arg_rd,
                 unsigned long long arg_alu,
                 unsigned long long arg_mem_content);
    void set_ctrl(INST32_CTRL_BIT arg_ctrl);
    ERROR_NUM WriteBack2Regfile(TEMPORAL_REG* regfile);
    REG get_pc();
    void print();
    unsigned long long get_cycles();
    INST32_CTRL_BIT get_ctrl();
    unsigned int get_rd();
    unsigned long long get_wb();

private:
    unsigned long long alu, mem_content;
    unsigned int rd;
    unsigned long long wb;
    REG pc;
    INST32_CTRL_BIT ctrl;

    WBSEL_CTRL get_WBSel();
    REGWEN_CTRL get_RegWEn();
};

#endif // WB_H

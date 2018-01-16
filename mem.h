#ifndef MEM_H
#define MEM_H

#include "reg_def.h"
#include <cstdio>

class MEM
{
public:
    unsigned char type;
    ERROR_NUM err_no;

    MEM();
    void set_Reg(unsigned int arg_rd,
                 unsigned long long arg_alu,
                 unsigned long long arg_data);
    void set_pc(REG arg_pc);
    void set_ctrl(INST32_CTRL_BIT arg_ctrl);
    ERROR_NUM MemRW(char* memory);
    void get_meminfo(unsigned long long &addr, int &read, int &size);
    unsigned long long get_mem_content();
    REG get_pc();
    INST32_CTRL_BIT get_ctrl();
    unsigned long long get_alu();
    unsigned int get_rd();
    void print();
    unsigned long long get_cycles();

private:
    REG pc;
    unsigned int rd;
    unsigned long long alu, data;
    unsigned long long mem_content;
    INST32_CTRL_BIT ctrl;

    MEMRW_CTRL get_MemRW();
};

#endif // MEM_H

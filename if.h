#ifndef IF_H
#define IF_H

#include "reg_def.h"

class IF
{
public:
    unsigned char type;
    ERROR_NUM err_no;

    IF();
    IF(REG arg_next_pc);
    void set_inst32(char* mem);
    unsigned int get_inst32();
    void set_next_pc(REG arg_next_pc);
    void set_pc(REG arg_pc);
    REG get_next_pc();
    REG get_pc();
    void split32();
    INST32_CTRL_BIT get_ctrl_bit();
    IMM_SET get_imm_set();
    void set_inst16(char* mem);
    long long get_inst_cnt();
    unsigned long long get_cycles();
    void reset();
    void print();

    unsigned int get_opcode();
    unsigned int get_rd();
    unsigned int get_funct3();
    unsigned int get_rs1();
    unsigned int get_rs2();
    unsigned int get_funct7();
    unsigned int get_I_imm();
    unsigned int get_S_imm();
    unsigned int get_SB_imm();
    unsigned int get_U_imm();
    unsigned int get_UJ_imm();
    unsigned int get_R4_rs3();
    unsigned int get_R4_funct2();

    void set_type();
    char get_type();
    void print_assembly();
    ERROR_NUM just_sim(REG* reg, FREG* freg, char* memory);

private:
    INST inst;
    long long inst_cnt;
    REG pc, next_pc;
    unsigned int inst32_0_6;    // opcode
    unsigned int inst32_7_11;   // rd
    unsigned int inst32_12_14;  // funct3
    unsigned int inst32_15_19;  // rs1
    unsigned int inst32_20_24;  // rs2
    unsigned int inst32_25_31;  // funct7
    unsigned int inst32_20_31;  // imm, I-type
    unsigned int inst32_7_7;    // imm[11], SB-type
    unsigned int inst32_8_11;   // imm[4:1], SB-type
    unsigned int inst32_25_30;  // imm[10:5], SB-type
    unsigned int inst32_31_31;  // imm[12], SB-type, imm[20], UJ-type
    unsigned int inst32_12_31;  // imm, U-type
    unsigned int inst32_12_19;  // imm[19:12], UJ-type
    unsigned int inst32_20_20;  // imm[11], UJ-type
    unsigned int inst32_21_30;  // imm[10:1], UJ-type
    unsigned int inst32_25_26;  // funct2, R4-type
    unsigned int inst32_27_31;  // rs3, R4-type
};

#endif // IF_H

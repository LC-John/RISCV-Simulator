#include "if.h"
#include <cstdio>

static unsigned int getbit(unsigned int inst, int s,int e)
{
    return (unsigned int)(((unsigned int)inst << (31 - e)) >> (31 - e + s));
}

static char REG_NAME[32][5] =
{
    "x0", "ra", "sp", "gp",
    "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1",
    "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3",
    "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11",
    "t3", "t4", "t5", "t6"
};

static unsigned long long sext64(unsigned long long num, int sbit)
{
    unsigned long long mask = 0x0;
    if ((num & ((unsigned long long)0x1 << sbit)) != 0)
        mask = (unsigned long long)((long long)0x8000000000000000 >> (63 - sbit));
    return num | mask;
}

IF::IF()
{
    inst_cnt = 0;
    inst.inst32 = 0;
    pc = 0; next_pc = 0;
    inst32_0_6 = 0;     // opcode
    inst32_7_11 = 0;    // rd
    inst32_12_14 = 0;   // funct3
    inst32_15_19 = 0;   // rs1
    inst32_20_24 = 0;   // rs2
    inst32_25_31 = 0;   // funct7
    inst32_20_31 = 0;   // imm, I-type
    inst32_7_7 = 0;     // imm[11], SB-type
    inst32_8_11 = 0;    // imm[4:1], SB-type
    inst32_25_30 = 0;   // imm[10:5], SB-type
    inst32_31_31 = 0;   // imm[12], SB-type, imm[20], UJ-type
    inst32_12_31 = 0;   // imm, U-type
    inst32_12_19 = 0;   // imm[19:12], UJ-type
    inst32_20_20 = 0;   // imm[11], UJ-type
    inst32_21_30 = 0;   // imm[10:1], UJ-type
}

IF::IF(REG arg_next_pc)
{
    inst_cnt = 0;
    inst.inst32 = 0;
    pc = 0;
    next_pc = arg_next_pc;
    inst32_0_6 = 0;     // opcode
    inst32_7_11 = 0;    // rd
    inst32_12_14 = 0;   // funct3
    inst32_15_19 = 0;   // rs1
    inst32_20_24 = 0;   // rs2
    inst32_25_31 = 0;   // funct7
    inst32_20_31 = 0;   // imm, I-type
    inst32_7_7 = 0;     // imm[11], SB-type
    inst32_8_11 = 0;    // imm[4:1], SB-type
    inst32_25_30 = 0;   // imm[10:5], SB-type
    inst32_31_31 = 0;   // imm[12], SB-type, imm[20], UJ-type
    inst32_12_31 = 0;   // imm, U-type
    inst32_12_19 = 0;   // imm[19:12], UJ-type
    inst32_20_20 = 0;   // imm[11], UJ-type
    inst32_21_30 = 0;   // imm[10:1], UJ-type
}

void IF::set_next_pc(REG arg_next_pc)
{
    next_pc = arg_next_pc;
}

void IF::set_pc(REG arg_pc)
{
    pc = arg_pc;
}

unsigned int IF::get_inst32()
{
    return inst.inst32;
}

REG IF::get_next_pc()
{
    return next_pc;
}

REG IF::get_pc()
{
    return pc;
}

void IF::set_inst32(char* mem)
{
    pc = next_pc;
    next_pc = pc + 4;
    inst.inst32 = *((unsigned int *)(mem + pc));
}

void IF::set_inst16(char* mem)
{
    pc = next_pc;
    next_pc = pc + 2;
    inst.inst16 = *((unsigned short *)(mem + pc));
}

long long IF::get_inst_cnt()
{
    return inst_cnt;
}

void IF::split32()
{
    inst32_0_6 = getbit(inst.inst32, 0, 6);     // opcode
    inst32_7_11 = getbit(inst.inst32, 7, 11);   // rd
    inst32_12_14 = getbit(inst.inst32, 12, 14); // funct3
    inst32_15_19 = getbit(inst.inst32, 15, 19); // rs1
    inst32_20_24 = getbit(inst.inst32, 20, 24); // rs2
    inst32_25_31 = getbit(inst.inst32, 25, 31); // funct7
    inst32_20_31 = getbit(inst.inst32, 20, 31); // imm, I-type
    inst32_7_7 = getbit(inst.inst32, 7, 7);     // imm[11], SB-type
    inst32_8_11 = getbit(inst.inst32, 8, 11);   // imm[4:1], SB-type
    inst32_25_30 = getbit(inst.inst32, 25, 30); // imm[10:5], SB-type
    inst32_31_31 = getbit(inst.inst32, 31, 31); // imm[12], SB-type, imm[20], UJ-type
    inst32_12_31 = getbit(inst.inst32, 12, 31); // imm, U-type
    inst32_12_19 = getbit(inst.inst32, 12, 19); // imm[19:12], UJ-type
    inst32_20_20 = getbit(inst.inst32, 20, 20); // imm[11], UJ-type
    inst32_21_30 = getbit(inst.inst32, 21, 30); // imm[10:1], UJ-type
    inst32_25_26 = getbit(inst.inst32, 25, 26);
    inst32_27_31 = getbit(inst.inst32, 27, 31);
}

void IF::reset()
{
    inst_cnt = 0;
    inst.inst32 = 0;
    pc = 0;
    next_pc = 0;
    inst32_0_6 = 0;     // opcode
    inst32_7_11 = 0;    // rd
    inst32_12_14 = 0;   // funct3
    inst32_15_19 = 0;   // rs1
    inst32_20_24 = 0;   // rs2
    inst32_25_31 = 0;   // funct7
    inst32_20_31 = 0;   // imm, I-type
    inst32_7_7 = 0;     // imm[11], SB-type
    inst32_8_11 = 0;    // imm[4:1], SB-type
    inst32_25_30 = 0;   // imm[10:5], SB-type
    inst32_31_31 = 0;   // imm[12], SB-type, imm[20], UJ-type
    inst32_12_31 = 0;   // imm, U-type
    inst32_12_19 = 0;   // imm[19:12], UJ-type
    inst32_20_20 = 0;   // imm[11], UJ-type
    inst32_21_30 = 0;   // imm[10:1], UJ-type
}

INST32_CTRL_BIT IF::get_ctrl_bit()
{
    INST32_CTRL_BIT ctrl_bit;
    ctrl_bit.BrEq = false;
    ctrl_bit.BrLT = false;
    ctrl_bit.inst32_25_31 = inst32_25_31;
    ctrl_bit.inst32_12_14 = inst32_12_14;
    ctrl_bit.inst32_2_6 = getbit(inst.inst32, 2, 6);
    return ctrl_bit;
}

IMM_SET IF::get_imm_set()
{
    IMM_SET imm;
    switch(get_opcode())
    {
    case (0x37):
    case (0x17):
        imm.imm_U = get_U_imm(); break;
    case (0x6f):
        imm.imm_UJ = get_UJ_imm(); break;
    case (0x63):
        imm.imm_SB = get_SB_imm(); break;
    case (0x23):
        imm.imm_S = get_S_imm(); break;
    case (0x33):
    case (0x3b):
        imm.imm_I = 0; break;
    case (0x67):
    case (0x03):
    case (0x13):
    case (0x1b):
        imm.imm_I = get_I_imm(); break;
    default:
        imm.imm_I = 0; break;
    }
    return imm;
}

unsigned int IF::get_opcode()
{
    return inst32_0_6;
}

unsigned int IF::get_rd()
{
    return inst32_7_11;
}

unsigned int IF::get_funct3()
{
    return inst32_12_14;
}

unsigned int IF::get_rs1()
{
    return inst32_15_19;
}

unsigned int IF::get_rs2()
{
    return inst32_20_24;
}

unsigned int IF::get_funct7()
{
    return inst32_25_31;
}

unsigned int IF::get_I_imm()
{
    return inst32_20_31;
}

unsigned int IF::get_S_imm()
{
    return inst32_7_11 | (inst32_25_31 << 5);
}

unsigned int IF::get_SB_imm()
{
    return (inst32_7_7 << 11)
            | (inst32_8_11 << 1)
            | (inst32_25_30 << 5)
            | (inst32_31_31 << 12);
}

unsigned int IF::get_U_imm()
{
    return (inst32_12_31 << 12);
}

unsigned int IF::get_UJ_imm()
{
    return (inst32_12_19 << 12)
            | (inst32_20_20 << 11)
            | (inst32_21_30 << 1)
            | (inst32_31_31 << 20);
}

unsigned int IF::get_R4_rs3()
{
    return inst32_27_31;
}

unsigned int IF::get_R4_funct2()
{
    return inst32_25_26;
}

void IF::set_type()
{
    switch (get_opcode())
    {
    case 0x37:
    case 0x17:
        type = 'U'; break;
    case 0x6f:
        type = 'J'; break;
    case 0x67:
    case 0x13:
    case 0x1b:
        type = 'I'; break;
    case 0x03:
        type = 'L'; break;
    case 0x63:
        type = 'B'; break;
    case 0x23:
        type = 'S'; break;
    case 0x33:
    case 0x3b:
        type = 'R'; break;
    default:
        printf("Unknown Instruction: %x\n",
               inst.inst32); break;
    }
}

char IF::get_type()
{
    return type;
}

void IF::print_assembly()
{
    switch (get_opcode())
    {
    case 0x37:
        printf("lui %s, %x\n",
               REG_NAME[get_rd()], get_U_imm()); break;
    case 0x17:
        printf("auipc %s, %x\n",
               REG_NAME[get_rd()], get_U_imm()); break;
    case 0x6f:
        printf("jal %s, %x\n",
               REG_NAME[get_rd()], get_UJ_imm()); break;
    case 0x67:
        printf("jalr %s, %s, %x\n",
               REG_NAME[get_rd()], REG_NAME[get_rs1()], get_I_imm()); break;
    case 0x63:
        switch(get_funct3())
        {
        case 0x0:
            printf("beq %s, %s, 0x%x\n",
                   REG_NAME[get_rs1()], REG_NAME[get_rs2()], get_SB_imm()); break;
        case 0x1:
            printf("bne %s, %s, 0x%x\n",
                   REG_NAME[get_rs1()], REG_NAME[get_rs2()], get_SB_imm()); break;
        case 0x4:
            printf("blt %s, %s, 0x%x\n",
                   REG_NAME[get_rs1()], REG_NAME[get_rs2()], get_SB_imm()); break;
        case 0x5:
            printf("bge %s, %s, 0x%x\n",
                   REG_NAME[get_rs1()], REG_NAME[get_rs2()], get_SB_imm()); break;
        case 0x6:
            printf("bltu %s, %s, 0x%x\n",
                   REG_NAME[get_rs1()], REG_NAME[get_rs2()], get_SB_imm()); break;
        case 0x7:
            printf("bgeu %s, %s, 0x%x\n",
                   REG_NAME[get_rs1()], REG_NAME[get_rs2()], get_SB_imm()); break;
        default:
            printf("Unknown Instruction: %x\n",
                   inst.inst32); break;
        } break;
    case 0x03:
        switch(get_funct3())
        {
        case 0x0:
            printf("lb %s, 0x%x(%s)\n",
                   REG_NAME[get_rd()], get_I_imm(), REG_NAME[get_rs1()]); break;
        case 0x1:
            printf("lh %s, 0x%x(%s)\n",
                   REG_NAME[get_rd()], get_I_imm(), REG_NAME[get_rs1()]); break;
        case 0x2:
            printf("lw %s, 0x%x(%s)\n",
                   REG_NAME[get_rd()], get_I_imm(), REG_NAME[get_rs1()]); break;
        case 0x4:
            printf("lbu %s, 0x%x(%s)\n",
                   REG_NAME[get_rd()], get_I_imm(), REG_NAME[get_rs1()]); break;
        case 0x5:
            printf("lhu %s, 0x%x(%s)\n",
                   REG_NAME[get_rd()], get_I_imm(), REG_NAME[get_rs1()]); break;
        case 0x6:
            printf("lwu %s, 0x%x(%s)\n",
                   REG_NAME[get_rd()], get_I_imm(), REG_NAME[get_rs1()]); break;
        case 0x3:
            printf("ld %s, 0x%x(%s)\n",
                   REG_NAME[get_rd()], get_I_imm(), REG_NAME[get_rs1()]); break;
        default:
            printf("Unknown Instruction: %x\n",
                   inst.inst32); break;
        } break;
    case 0x23:
        switch(get_funct3())
        {
        case 0x0:
            printf("sb %s, 0x%x(%s)\n",
                   REG_NAME[get_rs2()], get_S_imm(), REG_NAME[get_rs1()]); break;
        case 0x1:
            printf("sh %s, 0x%x(%s)\n",
                   REG_NAME[get_rs2()], get_S_imm(), REG_NAME[get_rs1()]); break;
        case 0x2:
            printf("sw %s, 0x%x(%s)\n",
                   REG_NAME[get_rs2()], get_S_imm(), REG_NAME[get_rs1()]); break;
        case 0x3:
            printf("sd %s, 0x%x(%s)\n",
                   REG_NAME[get_rs2()], get_S_imm(), REG_NAME[get_rs1()]); break;
        default:
            printf("Unknown Instruction: %x\n",
                   inst.inst32); break;
        } break;
    case 0x13:
        switch(get_funct3())
        {
        case 0x0:
            printf("addi %s, %s, 0x%x\n",
                   REG_NAME[get_rd()], REG_NAME[get_rs1()], get_I_imm()); break;
        case 0x2:
            printf("slti %s, %s, 0x%x\n",
                   REG_NAME[get_rd()], REG_NAME[get_rs1()], get_I_imm()); break;
        case 0x3:
            printf("sltiu %s, %s, 0x%x\n",
                   REG_NAME[get_rd()], REG_NAME[get_rs1()], get_I_imm()); break;
        case 0x4:
            printf("xori %s, %s, 0x%x\n",
                   REG_NAME[get_rd()], REG_NAME[get_rs1()], get_I_imm()); break;
        case 0x6:
            printf("ori %s, %s, 0x%x\n",
                   REG_NAME[get_rd()], REG_NAME[get_rs1()], get_I_imm()); break;
        case 0x7:
            printf("andi %s, %s, 0x%x\n",
                   REG_NAME[get_rd()], REG_NAME[get_rs1()], get_I_imm()); break;
        case 0x1:
            if (getbit(inst.inst32, 26, 31) != 0x00)
            {
                printf("Unknown Instruction: %x\n",
                       inst.inst32); break;
            }
            printf("slli %s, %s, 0x%x\n",
                   REG_NAME[get_rd()], REG_NAME[get_rs1()], get_I_imm()); break;
        case 0x5:
            switch(getbit(inst.inst32, 26, 31))
            {
            case 0x00:
                printf("srli %s, %s, 0x%x\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], get_I_imm()); break;
            case 0x10:
                printf("srai %s, %s, 0x%x\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], get_I_imm()); break;
            default:
                printf("Unknown Instruction: %x\n",
                       inst.inst32); break;
            } break;
        } break;
    case 0x33:
        switch(get_funct3())
        {
        case 0x00:
            switch(get_funct7())
            {
            case 0x00:
                printf("add %s, %s, %s\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
            case 0x20:
                printf("sub %s, %s, %s\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
            case 0x01:
                printf("mul %s, %s, %s\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
            default:
                printf("Unknown Instruction: %x\n",
                       inst.inst32); break;
            } break;
        case 0x1:
            switch(get_funct7())
            {
            case 0x00:
                printf("sll %s, %s, %s\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
            case 0x01:
                printf("sulh %s, %s, %s\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
            default:
                printf("Unknown Instruction: %x\n",
                       inst.inst32); break;
            } break;
        case 0x2:
            switch(get_funct7())
            {
            case 0x00:
                printf("slt %s, %s, %s\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
            case 0x01:
                printf("mulhsu %s, %s, %s\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
            default:
                printf("Unknown Instruction: %x\n",
                       inst.inst32); break;
            } break;
        case 0x3:
            switch(get_funct7())
            {
            case 0x00:
                printf("sltu %s, %s, %s\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
            case 0x01:
                printf("mulhu %s, %s, %s\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
            default:
                printf("Unknown Instruction: %x\n",
                       inst.inst32); break;
            } break;
        case 0x4:
            switch(get_funct7())
            {
            case 0x00:
                printf("xor %s, %s, %s\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
            case 0x01:
                printf("div %s, %s, %s\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
            default:
                printf("Unknown Instruction: %x\n",
                       inst.inst32); break;
            } break;
        case 0x6:
            switch(get_funct7())
            {
            case 0x00:
                printf("or %s, %s, %s\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
            case 0x01:
                printf("rem %s, %s, %s\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
            default:
                printf("Unknown Instruction: %x\n",
                       inst.inst32); break;
            } break;
        case 0x7:
            switch(get_funct7())
            {
            case 0x00:
                printf("and %s, %s, %s\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
            case 0x01:
                printf("remu %s, %s, %s\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
            default:
                printf("Unknown Instruction: %x\n",
                       inst.inst32); break;
            } break;
        case 0x5:
            switch(get_funct7())
            {
            case 0x00:
                printf("srl %s, %s, %s\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
            case 0x20:
                printf("sra %s, %s, %s\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
            case 0x01:
                printf("divu %s, %s, %s\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
            default:
                printf("Unknown Instruction: %x\n",
                       inst.inst32); break;
            } break;
        default:
            printf("Unknown Instruction: %x\n",
                   inst.inst32); break;
        } break;
    case 0x1b:
        switch(get_funct3())
        {
        case 0x0:
            printf("addiw %s, %s, 0x%x\n",
                   REG_NAME[get_rd()], REG_NAME[get_rs1()], get_I_imm()); break;
        case 0x1:
            if (get_funct7() != 0x00)
            {
                printf("Unknown Instruction: %x\n",
                       inst.inst32); break;
            }
            printf("slliw %s, %s, 0x%x\n",
                   REG_NAME[get_rd()], REG_NAME[get_rs1()], get_I_imm()); break;
        case 0x5:
            switch(get_funct7())
            {
            case 0x00:
                printf("srliw %s, %s, 0x%x\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], get_I_imm()); break;
            case 0x20:
                printf("sraiw %s, %s, 0x%x\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], get_I_imm()); break;
            default:
                printf("Unknown Instruction: %x\n",
                       inst.inst32); break;
            } break;
        default:
            printf("Unknown Instruction: %x\n",
                   inst.inst32); break;
        } break;
    case 0x3b:
        switch(get_funct3())
        {
        case 0x0:
            switch(get_funct7())
            {
            case 0x00:
                printf("addw %s, %s, %s\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
            case 0x20:
                printf("subw %s, %s, %s\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
            case 0x01:
                printf("mulw %s, %s, %s\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
            default:
                printf("Unknown Instruction: %x\n",
                       inst.inst32); break;
            } break;
        case 0x1:
            if (get_funct7() != 0x00)
            {
                printf("Unknown Instruction: %x\n",
                       inst.inst32); break;
            }
            printf("sllw %s, %s, %s\n",
                   REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
        case 0x5:
            switch(get_funct3())
            {
            case 0x00:
                printf("srlw %s, %s, %s\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
            case 0x20:
                printf("sraw %s, %s, %s\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
            case 0x01:
                printf("divuw %s, %s, %s\n",
                       REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
            default:
                printf("Unknown Instruction: %x\n",
                       inst.inst32); break;
            } break;
        case 0x4:
            if (get_funct7() != 0x01)
            {
                printf("Unknown Instruction: %x\n",
                       inst.inst32); break;
            }
            printf("divw %s, %s, %s\n",
                   REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
        case 0x6:
            if (get_funct7() != 0x01)
            {
                printf("Unknown Instruction: %x\n",
                       inst.inst32); break;
            }
            printf("remw %s, %s, %s\n",
                   REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
        case 0x7:
            if (get_funct7() != 0x01)
            {
                printf("Unknown Instruction: %x\n",
                       inst.inst32); break;
            }
            printf("remuw %s, %s, %s\n",
                   REG_NAME[get_rd()], REG_NAME[get_rs1()], REG_NAME[get_rs2()]); break;
        } break;
    default:
        printf("Unknown Instruction: %x\n",
               inst.inst32); break;
    }
}

ERROR_NUM IF::just_sim(REG* reg, FREG* freg, char* memory)
{
    ERROR_NUM errno = NOTHING;
    /*
    FLOAT2UINT fp32_int32_cvt;
    DOUBLE2ULL fp64_int64_cvt;
    */
    switch (get_opcode())
    {
    case 0x37:  // lui rd, offset
        reg[get_rd()] = sext64(get_U_imm(), 31); break;
    case 0x17:  // auipc rd, offset
        reg[get_rd()] = pc + sext64(get_U_imm(), 31); break;
    case 0x6f:  // jal rd, imm
        reg[get_rd()] = pc + 4; next_pc = pc + sext64(get_UJ_imm(), 20); break;
    case 0x67:  // jalr rd, rs1, imm
        next_pc = (unsigned long long)((long long)reg[get_rs1()] + (long long)sext64(get_I_imm(), 11)) & 0xfffffffffffffffe;
        reg[get_rd()] = pc + 4; break;
    case 0x63:
        switch(get_funct3())
        {
        case 0x0:   // beq rs1, rs2, offset
            if (reg[get_rs1()] == reg[get_rs2()])
                next_pc = pc + sext64(get_SB_imm(), 12); break;
        case 0x1:   // bne rs1, rs2, offset
            if (reg[get_rs1()] != reg[get_rs2()])
                next_pc = pc + sext64(get_SB_imm(), 12); break;
        case 0x4:   // blt rs1, rs2, offset
            if ((long long)reg[get_rs1()] < (long long)reg[get_rs2()])
                next_pc = pc + sext64(get_SB_imm(), 12); break;
        case 0x5:   // bge rs1, rs2, offset
            if ((long long)reg[get_rs1()] >= (long long)reg[get_rs2()])
                next_pc = pc + sext64(get_SB_imm(), 12); break;
        case 0x6:   // bltu rs1, rs2, offset
            if ((unsigned long long)reg[get_rs1()] < (unsigned long long)reg[get_rs2()])
                next_pc = pc + sext64(get_SB_imm(), 12); break;
        case 0x7:   // bgeu rs1, rs2, offset
            if ((unsigned long long)reg[get_rs1()] >= (unsigned long long)reg[get_rs2()])
                next_pc = pc + sext64(get_SB_imm(), 12); break;
        default:
            errno = INVALID_INST; break;
        } break;
    case 0x03:
        switch(get_funct3())
        {
        case 0x0:   // lb rd, offset(rs1)
            reg[get_rd()] = sext64((unsigned long long)(*((unsigned char *)(memory+reg[get_rs1()]+sext64(get_I_imm(), 11)))), 7); break;
        case 0x1:   // lh rd, offset(rs1)
            if ((reg[get_rs1()] + sext64(get_I_imm(), 11)) % 2 != 0)
            {
                errno = INVALID_ADDR_ALIGNMENT; break;
            }
            reg[get_rd()] = sext64((unsigned long long)(*((unsigned short *)(memory+reg[get_rs1()]+sext64(get_I_imm(), 11)))), 15); break;
        case 0x2:   // lw rd, offset(rs1)
            if ((reg[get_rs1()] + sext64(get_I_imm(), 11)) % 4 != 0)
            {
                errno = INVALID_ADDR_ALIGNMENT; break;
            }
            reg[get_rd()] = sext64((unsigned long long)(*((unsigned int *)(memory+reg[get_rs1()]+sext64(get_I_imm(), 11)))), 31); break;
        case 0x4:   // lbu rd, offset(rs1)
            reg[get_rd()] = (unsigned long long)(*((unsigned char *)(memory+reg[get_rs1()]+sext64(get_I_imm(), 11)))); break;
        case 0x5:   // lhu rd, offset(rs1)
            if ((reg[get_rs1()] + sext64(get_I_imm(), 11)) % 2 != 0)
            {
                errno = INVALID_ADDR_ALIGNMENT; break;
            }
            reg[get_rd()] = (unsigned long long)(*((unsigned short *)(memory+reg[get_rs1()]+sext64(get_I_imm(), 11)))); break;
        case 0x6:   // lwu rd, offset(rs1)
            if ((reg[get_rs1()] + sext64(get_I_imm(), 11)) % 4 != 0)
            {
                errno = INVALID_ADDR_ALIGNMENT; break;
            }
            reg[get_rd()] = (unsigned long long)(*((unsigned int *)(memory+reg[get_rs1()]+sext64(get_I_imm(), 11)))); break;
        case 0x3:   // ld rd, offset(rs1)
            if ((reg[get_rs1()] + sext64(get_I_imm(), 11)) % 8 != 0)
            {
                errno = INVALID_ADDR_ALIGNMENT; break;
            }
            reg[get_rd()] = sext64((unsigned long long)(*((unsigned long long *)(memory+reg[get_rs1()]+sext64(get_I_imm(), 11)))), 63); break;
        default:
            errno = INVALID_INST; break;
        } break;
    case 0x23:
        switch(get_funct3())
        {
        case 0x0:   // sb rs2, offset(rs1)
            *(unsigned char *)(memory+reg[get_rs1()]+sext64(get_S_imm(), 11)) = (unsigned char)(reg[get_rs2()] & 0xff) ; break;
        case 0x1:   // sh rs2, offset(rs1)
            if ((reg[get_rs1()] + sext64(get_S_imm(), 11)) % 2 != 0)
            {
                errno = INVALID_ADDR_ALIGNMENT; break;
            }
            *(unsigned short *)(memory+reg[get_rs1()]+sext64(get_S_imm(), 11)) = (unsigned short)(reg[get_rs2()] & 0xffff); break;
        case 0x2:   // sw rs2, offset(rs1)
            if ((reg[get_rs1()] + sext64(get_S_imm(), 11)) % 4 != 0)
            {
                errno = INVALID_ADDR_ALIGNMENT; break;
            }
            *(unsigned int *)(memory+reg[get_rs1()]+sext64(get_S_imm(), 11)) = (unsigned int)(reg[get_rs2()] & 0xffffffff); break;
        case 0x3:   // sd rs2, offset(rs1)
            if ((reg[get_rs1()] + sext64(get_S_imm(), 11)) % 8 != 0)
            {
                errno = INVALID_ADDR_ALIGNMENT; break;
            }
            *(unsigned long long *)(memory+reg[get_rs1()]+sext64(get_S_imm(), 11)) = (unsigned long long)(reg[get_rs2()] & 0xffffffffffffffff); break;
        default:
            errno = INVALID_INST; break;
        } break;
    case 0x13:
        switch(get_funct3())
        {
        case 0x0:   // addi rd, rs1, imm
            reg[get_rd()] = (unsigned long long)((long long)reg[get_rs1()] + (long long)sext64(get_I_imm(), 11)); break;
        case 0x2:   // slti rd, rs1, imm
            reg[get_rd()] = (unsigned long long)((long long)reg[get_rs1()] < (long long)sext64(get_I_imm(), 11) ? 1 : 0); break;
        case 0x3:   // sltiu rd, rs1, imm
            reg[get_rd()] = (unsigned long long)((unsigned long long)reg[get_rs1()] < (unsigned long long)sext64(get_I_imm(), 11) ? 1 : 0); break;
        case 0x4:   // xori rd, rs1, imm
            reg[get_rd()] = (unsigned long long)((unsigned long long)reg[get_rs1()] ^ (unsigned long long)sext64(get_I_imm(), 11)); break;
        case 0x6:   // ori rd, rs1, imm
            reg[get_rd()] = (unsigned long long)((unsigned long long)reg[get_rs1()] | (unsigned long long)sext64(get_I_imm(), 11)); break;
        case 0x7:   // andi rd, rs1, imm
            reg[get_rd()] = (unsigned long long)((unsigned long long)reg[get_rs1()] & (unsigned long long)sext64(get_I_imm(), 11)); break;
        case 0x1:   // slli rd, rs1, imm
            if (getbit(inst.inst32, 26, 31) != 0x00)
            {
                errno = INVALID_INST; break;
            }
            reg[get_rd()] = (unsigned long long)((unsigned long long)reg[get_rs1()] << (unsigned long long)sext64(get_I_imm(), 11)); break;
        case 0x5:
            switch(getbit(inst.inst32, 26, 31))
            {
            case 0x00:  // srli rd, rs1, imm
                reg[get_rd()] = (unsigned long long)((unsigned long long)reg[get_rs1()] >> (unsigned long long)sext64(get_I_imm(), 11)); break;
            case 0x10:  // srai rd, rs1, imm
                reg[get_rd()] = (unsigned long long)((long long)reg[get_rs1()] >> (long long)sext64(getbit(inst.inst32, 20, 25), 5)); break;
            default:
                errno = INVALID_INST; break;
            } break;
        default:
            errno = INVALID_INST; break;
        } break;
    case 0x33:
        switch(get_funct3())
        {
        case 0x0:
            switch(get_funct7())
            {
            case 0x00:  // add rd, rs1, rs2
                reg[get_rd()] = (unsigned long long)((long long)reg[get_rs1()] + (long long)reg[get_rs2()]); break;
            case 0x20:  // sub rd, rs1, rs2
                reg[get_rd()] = (unsigned long long)((long long)reg[get_rs1()] - (long long)reg[get_rs2()]); break;
            case 0x01:  // mul rd, rs1, rs2
                reg[get_rd()] = (unsigned long long)((unsigned long long)reg[get_rs1()] * (unsigned long long)reg[get_rs2()]); break;
            default:
                errno = INVALID_INST; break;
            } break;
        case 0x1:
            switch(get_funct7())
            {
            case 0x00: // sll rd, rs1, rs2
                reg[get_rd()] = (unsigned long long)((unsigned long long)reg[get_rs1()] << ((unsigned long long)reg[get_rs2()] & 0x3f)); break;
            case 0x01:  // mulh rd, rs1, rs2
                reg[get_rd()] = 0; break;
            default:
                errno = INVALID_INST; break;
            } break;
        case 0x2:
            switch(get_funct7())
            {
            case 0x00:  // slt rd, rs1, rs2
                reg[get_rd()] = (unsigned long long)((long long)reg[get_rs1()] < (long long)reg[get_rs2()] ? 1 : 0); break;
            case 0x01:  // mulhsu rd, rs1, rs2
                reg[get_rd()] = 0; break;
            default:
                errno = INVALID_INST; break;
            } break;
        case 0x3:
            switch(get_funct7())
            {
            case 0x00:  // sltu rd, rs1, rs2
                reg[get_rd()] = (unsigned long long)((unsigned long long)reg[get_rs1()] < (unsigned long long)reg[get_rs2()] ? 1 : 0); break;
            case 0x01:  // mulhu rd, rs1, rs2
                reg[get_rd()] = 0; break;
            default:
                errno = INVALID_INST; break;
            } break;
        case 0x4:
            switch(get_funct7())
            {
            case 0x00:  // xor rd, rs1, rs2
                reg[get_rd()] = (unsigned long long)((unsigned long long)reg[get_rs1()] ^ (unsigned long long)reg[get_rs2()]); break;
            case 0x01:  // div rd, rs1, rs2
                reg[get_rd()] = (unsigned long long)((long long)reg[get_rs1()] / (long long)reg[get_rs2()]); break;
            default:
                errno = INVALID_INST; break;
            } break;
        case 0x6:
            switch(get_funct7())
            {
            case 0x00:  // or rd, rs1, rs2
                reg[get_rd()] = (unsigned long long)((unsigned long long)reg[get_rs1()] | (unsigned long long)reg[get_rs2()]); break;
            case 0x01:  // rem rd, rs1, rs2
                reg[get_rd()] = (unsigned long long)((long long)reg[get_rs1()] % (long long)reg[get_rs2()]); break;
            default:
                errno = INVALID_INST; break;
            } break;
        case 0x7:
            switch(get_funct7())
            {
            case 0x00:  // and rd, rs1, rs2
                reg[get_rd()] = (unsigned long long)((unsigned long long)reg[get_rs1()] & (unsigned long long)reg[get_rs2()]); break;
            case 0x01:  // remu rd, rs1, rs2
                reg[get_rd()] = (unsigned long long)((unsigned long long)reg[get_rs1()] % (unsigned long long)reg[get_rs2()]); break;
            default:
                errno = INVALID_INST; break;
            } break;
        case 0x5:
            switch(get_funct7())
            {
            case 0x00:  // srl rd, rs1, rs2
                reg[get_rd()] = (unsigned long long)((unsigned long long)reg[get_rs1()] >> ((unsigned long long)reg[get_rs2()] & 0x3f)); break;
            case 0x20:  // sra rd, rs1, rs2
                reg[get_rd()] = (unsigned long long)((long long)reg[get_rs1()] >> (long long)((unsigned long long)reg[get_rs2()] & 0x3f)); break;
            case 0x01:  // divu rd, rs1, rs2
                reg[get_rd()] = (unsigned long long)((unsigned long long)reg[get_rs1()] / (unsigned long long)reg[get_rs2()]); break;
            default:
                errno = INVALID_INST; break;
            } break;
        default:
            errno = INVALID_INST; break;
        } break;
    case 0x1b:
        switch(get_funct3())
        {
        case 0x0:   // addiw rd, rs1, imm
            reg[get_rd()] = sext64((unsigned long long)((long long)reg[get_rs1()] + (long long)(sext64(get_I_imm(), 11))) & 0xffffffff, 31); break;
        case 0x1:   // slliw rd, rs1, imm
            if (get_funct7() != 0x00 || getbit(inst.inst32, 25, 25))
            {
                errno = INVALID_INST; break;
            }
            reg[get_rd()] = sext64((unsigned long long)((unsigned long long)reg[get_rs1()] << (unsigned long long)(getbit(inst.inst32, 20, 24))) & 0xffffffff, 31); break;
        case 0x5:
            switch(get_funct7())
            {
            case 0x00:  // srliw rd, rs1, imm
                reg[get_rd()] = sext64((unsigned long long)((unsigned long long)reg[get_rs1()] >> (unsigned long long)(getbit(inst.inst32, 20, 24))) & 0xffffffff, 31); break;
            case 0x20:  // sraiw rd, rs1, imm
                reg[get_rd()] = sext64((unsigned long long)((long long)reg[get_rs1()] >> (long long)(getbit(inst.inst32, 20, 24))) & 0xffffffff, 31); break;
            default:
                errno = INVALID_INST; break;
            } break;
        default:
            errno = INVALID_INST; break;
        } break;
    case 0x3b:
        switch(get_funct3())
        {
        case 0x0:
            switch(get_funct7())
            {
            case 0x00:  // addw rd, rs1, rs2
                reg[get_rd()] = sext64((unsigned long long)(unsigned int)((int)reg[get_rs1()] + (int)reg[get_rs2()]), 31); break;
            case 0x20:  // subw rd, rs1, rs2
                reg[get_rd()] = sext64((unsigned long long)(unsigned int)((int)reg[get_rs1()] - (int)reg[get_rs2()]), 31); break;
            case 0x01:  // mulw rd, rs1, rs2
                reg[get_rd()] = sext64((unsigned long long)(unsigned int)((int)reg[get_rs1()] * (int)reg[get_rs2()]), 31); break;
            default:
                errno = INVALID_INST; break;
            } break;
        case 0x1:   // sllw rd, rs1, rs2
            if (get_funct7() != 0x00)
            {
                errno = INVALID_INST; break;
            }
            reg[get_rd()] = sext64((reg[get_rs1()] << (reg[get_rs2()] & 0x1f)) & 0xffffffff, 31); break;
        case 0x5:
            switch(get_funct3())
            {
            case 0x00:  // srlw rd, rs1, rs2
                reg[get_rd()] = sext64(((unsigned long long)reg[get_rs1()] >> (unsigned long long)(reg[get_rs2()] & 0x1f)) & 0xffffffff, 31); break;
            case 0x20:  // sraw rd, rs1, rs2
                reg[get_rd()] = sext64(((long long)reg[get_rs1()] >> (long long)(reg[get_rs2()] & 0x1f)) & 0xffffffff, 31); break;
            case 0x01:  // divuw rd, rs1, rs2
                reg[get_rd()] = sext64((unsigned long long)(unsigned int)((unsigned int)reg[get_rs1()] / (unsigned int)reg[get_rs2()]), 31); break;
            default:
                errno = INVALID_INST; break;
            } break;
        case 0x4:   // divw rd, rs1, rs2
            if (get_funct7() != 0x01)
            {
                errno = INVALID_INST; break;
            }
            reg[get_rd()] = sext64((unsigned long long)(unsigned int)((int)reg[get_rs1()] / (int)reg[get_rs2()]), 31); break;
        case 0x6:   // remw rd, rs1, rs2
            if (get_funct7() != 0x01)
            {
                errno = INVALID_INST; break;
            }
            reg[get_rd()] = sext64((unsigned long long)(unsigned int)((int)reg[get_rs1()] % (int)reg[get_rs2()]), 31); break;
        case 0x7:   // remuw rd, rs1, rs2
            if (get_funct7() != 0x01)
            {
                errno = INVALID_INST; break;
            }
            reg[get_rd()] = sext64((unsigned long long)(unsigned int)((unsigned int)reg[get_rs1()] % (unsigned int)reg[get_rs2()]), 31); break;
        default:
            errno = INVALID_INST; break;
        } break;
     /*
    case 0x07:
        switch(get_funct3())
        {
        case 0x2:   // flw rd, offset(rs1)
            fp32_int32_cvt.uint = *((unsigned int *)(memory+reg[get_rs1()]+sext64(get_I_imm(), 11)));
            freg[get_rd()].fp32 = fp32_int32_cvt.fp32; break;
        case 0x3:   // fld rd, offset(rs1)
            fp64_int64_cvt.ull = *((unsigned long long *)(memory+reg[get_rs1()]+sext64(get_I_imm(), 11)));
            freg[get_rd()].fp64 = fp64_int64_cvt.fp64; break;
        default: errno = INVALID_INST; break;
        } break;
    case 0x27:
        switch(get_funct3())
        {
        case 0x2:   // fsw rs2, offset(rs1)
            fp32_int32_cvt.fp32 = freg[get_rs2()].fp32;
            *((unsigned int *)(memory+reg[get_rs1()]+sext64(get_S_imm(), 11))) = fp32_int32_cvt.uint; break;
        case 0x3:   // fsd rs2, offset(rs1)
            fp64_int64_cvt.fp64 = freg[get_rs2()].fp64;
            *((unsigned long long *)(memory+reg[get_rs1()]+sext64(get_S_imm(), 11))) = fp64_int64_cvt.ull; break;
        default: errno = INVALID_INST; break;
        } break;
    case 0x43:
        switch(get_R4_funct2())
        {
        case 0x0:   // fmadd.s rd, rs1, rs2, rs3
            freg[get_rd()].fp32 = freg[get_rs1()].fp32 * freg[get_rs2()].fp32 + freg[get_R4_rs3()].fp32; break;
        case 0x1:   // fmadd.d rd, rs1, rs2, rs3
            freg[get_rd()].fp64 = freg[get_rs1()].fp64 * freg[get_rs2()].fp64 + freg[get_R4_rs3()].fp64; break;
        default: errno = INVALID_INST; break;
        } break;
    case 0x47:
        switch(get_R4_funct2())
        {
        case 0x0:   // fmsub.s rd, rs1, rs2, rs3
            freg[get_rd()].fp32 = freg[get_rs1()].fp32 * freg[get_rs2()].fp32 - freg[get_R4_rs3()].fp32; break;
        case 0x1:   // fmsub.d rd, rs1, rs2, rs3
            freg[get_rd()].fp64 = freg[get_rs1()].fp64 * freg[get_rs2()].fp64 - freg[get_R4_rs3()].fp64; break;
        default: errno = INVALID_INST; break;
        } break;
    case 0x4b:
        switch(get_R4_funct2())
        {
        case 0x0:   // fnmsub.s rd, rs1, rs2, rs3
            freg[get_rd()].fp32 = -(freg[get_rs1()].fp32 * freg[get_rs2()].fp32 - freg[get_R4_rs3()].fp32); break;
        case 0x1:   // fnmsub.d rd, rs1, rs2, rs3
            freg[get_rd()].fp64 = -(freg[get_rs1()].fp64 * freg[get_rs2()].fp64 - freg[get_R4_rs3()].fp64); break;
        default: errno = INVALID_INST; break;
        } break;
    case 0x4f:
        switch(get_R4_funct2())
        {
        case 0x0:   // fnmadd.s rd, rs1, rs2, rs3
            freg[get_rd()].fp32 = -(freg[get_rs1()].fp32 * freg[get_rs2()].fp32 + freg[get_R4_rs3()].fp32); break;
        case 0x1:   // fnmadd.d rd, rs1, rs2, rs3
            freg[get_rd()].fp64 = -(freg[get_rs1()].fp64 * freg[get_rs2()].fp64 + freg[get_R4_rs3()].fp64); break;
        default: errno = INVALID_INST; break;
        } break;
    case 0x53:
        switch(get_funct7())
        {
        case 0x00:  // fadd.s rd, rs1, rs2
            freg[get_rd()].fp32 = freg[get_rs1()].fp32 + freg[get_rs2()].fp32; break;
        case 0x04:  // fsub.s rd, rs1, rs2
            freg[get_rd()].fp32 = freg[get_rs1()].fp32 - freg[get_rs2()].fp32; break;
        case 0x08:  // fmul.s rd, rs1, rs2
            freg[get_rd()].fp32 = freg[get_rs1()].fp32 * freg[get_rs2()].fp32; break;
        case 0x0c:  // fdiv.s rd, rs1, rs2
            freg[get_rd()].fp32 = freg[get_rs1()].fp32 / freg[get_rs2()].fp32; break;
        case 0x01:  // fadd.d rd, rs1, rs2
            freg[get_rd()].fp64 = freg[get_rs1()].fp64 + freg[get_rs2()].fp64; break;
        case 0x05:  // fsub.d rd, rs1, rs2
            freg[get_rd()].fp64 = freg[get_rs1()].fp64 - freg[get_rs2()].fp64; break;
        case 0x09:  // fmul.d rd, rs1, rs2
            freg[get_rd()].fp64 = freg[get_rs1()].fp64 * freg[get_rs2()].fp64; break;
        case 0x0d:  // fdiv.d rd, rs1, rs2
            freg[get_rd()].fp64 = freg[get_rs1()].fp64 / freg[get_rs2()].fp64; break;
        case 0x60:
            switch(get_rs2())
            {
            case 0x00:  // fcvt.w.s rd, rs1
                reg[get_rd()] = sext64((unsigned long long)(int)(freg[get_rs1()].fp32), 31); break;
            case 0x01:  // fcvt.wu.s, rd, rs1
                reg[get_rd()] = (unsigned long long)(unsigned int)(freg[get_rs1()].fp32); break;
            default: errno = INVALID_INST; break;
            } break;
        case 0x38:
            switch(get_rs2())
            {
            case 0x00:  // fcvt.s.w rd, rs1
                freg[get_rd()].fp32 = (float)(int)(reg[get_rs1()] & 0xffffffff); break;
            case 0x01:  // fcvt.s.wu, rd, rs1
                freg[get_rd()].fp32 = (float)(unsigned int)(reg[get_rs1()] & 0xffffffff); break;
            default: errno = INVALID_INST; break;
            } break;
        case 0x20:
            switch(get_rs2())
            {
            case 0x01:  // fcvt.s.d rd, rs1
                freg[get_rd()].fp32 = (float)(freg[get_rs1()].fp64); break;
            default: errno = INVALID_INST; break;
            } break;
        case 0x21:
            switch(get_rs2())
            {
            case 0x00:  // fcvt.d.s rd, rs1
                freg[get_rd()].fp64 = (double)(freg[get_rs1()].fp32); break;
            default: errno = INVALID_INST; break;
            } break;
        case 0x69:
            switch(get_rs2())
            {
            case 0x00:  // fcvt.d.w rd, rs1
                freg[get_rd()].fp64 = (double)(int)(reg[get_rs1()] & 0xffffffff); break;
            case 0x01:  // fcvt.d.wu rd, rs1
                freg[get_rd()].fp64 = (double)(unsigned int)(reg[get_rs1()] & 0xffffffff); break;
            default: errno = INVALID_INST; break;
            } break;
        default: errno = INVALID_INST; break;
        } break;
    */
    default:
        errno = INVALID_INST; break;
    }
    return errno;
}

void IF::print()
{
    printf("  IF:\n");
    printf("    inst = %08x\n", get_inst32());
    printf("    type = ");
    switch(type)
    {
    case 'R': printf("R\n"); break;
    case 'I': printf("I\n"); break;
    case 'L': printf("I(LOAD)\n"); break;
    case 'S': printf("S(STORE)\n"); break;
    case 'U': printf("U\n"); break;
    case 'J': printf("UJ\n"); break;
    case 'B': printf("SB\n"); break;
    defualt: printf("unknown\n"); break;
    }
}

unsigned long long IF::get_cycles()
{
    return 1;
}

#include "id.h"

static unsigned long long sext64(unsigned long long num, int sbit)
{
    unsigned long long mask = 0x0;
    if ((num & ((unsigned long long)0x1 << sbit)) != 0)
        mask = (unsigned long long)((long long)0x8000000000000000 >> (63 - sbit));
    return num | mask;
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

ID::ID()
{
    imm.imm_I = 0;
    imm.imm_S = 0;
    imm.imm_SB = 0;
    imm.imm_U = 0;
    imm.imm_UJ = 0;
    rd = 0;
    rs1 = 0;
    rs2 = 0;
    extimm = 0;
    rs1_content = 0;
    rs2_content = 0;
    ctrl.BrEq = false;
    ctrl.BrLT = false;
    ctrl.inst32_25_31 = 0;
    ctrl.inst32_2_6 = 0;
    ctrl.inst32_12_14 = 0;
    err_no = NOTHING;
}

void ID::set_RegImm(IMM_SET arg_imm,
                    unsigned int arg_rd,
                    unsigned int arg_rs1,
                    unsigned int arg_rs2)
{
    imm.imm_I = arg_imm.imm_I;
    imm.imm_S = arg_imm.imm_S;
    imm.imm_SB = arg_imm.imm_SB;
    imm.imm_U = arg_imm.imm_U;
    imm.imm_UJ = arg_imm.imm_UJ;
    rd = arg_rd;
    rs1 = arg_rs1;
    rs2 = arg_rs2;
}

void ID::set_ctrl(INST32_CTRL_BIT arg_ctrl)
{
    ctrl.BrEq = arg_ctrl.BrEq;
    ctrl.BrLT = arg_ctrl.BrLT;
    ctrl.inst32_25_31 = arg_ctrl.inst32_25_31;
    ctrl.inst32_2_6 = arg_ctrl.inst32_2_6;
    ctrl.inst32_12_14 = arg_ctrl.inst32_12_14;
}

INST32_CTRL_BIT ID::get_ctrl()
{
    return ctrl;
}

IMMSEL_CTRL ID::get_ImmSel()
{
    switch(ctrl.inst32_2_6)
    {
    case (0x37 >> 2):
    case (0x17 >> 2):
        return U_IMM;
    case (0x6f >> 2):
        return UJ_IMM;
    case (0x63 >> 2):
        return SB_IMM;
    case (0x23 >> 2):
        return S_IMM;
    case (0x33 >> 2):
    case (0x3b >> 2):
        return R_IMM;
    case (0x67 >> 2):
    case (0x03 >> 2):
    case (0x13 >> 2):
    case (0x1b >> 2):
        return I_IMM;
    default:
        return UNKNOWN;
    }
}

void ID::read_regfile(TEMPORAL_REG *reg)
{
    rs1_content = reg[rs1].out;
    rs2_content = reg[rs2].out;
}

ERROR_NUM ID::imm_extension()
{
    switch(get_ImmSel())
    {
    case R_IMM:
        extimm = 0; return NOTHING;
    case I_IMM:
        extimm = sext64((unsigned long long)imm.imm_I, 11); return NOTHING;
    case S_IMM:
        extimm = sext64((unsigned long long)imm.imm_S, 11); return NOTHING;
    case SB_IMM:
        extimm = sext64((unsigned long long)imm.imm_SB, 12); return NOTHING;
    case U_IMM:
        extimm = sext64((unsigned long long)imm.imm_U, 31); return NOTHING;
    case UJ_IMM:
        extimm = sext64((unsigned long long)imm.imm_UJ, 20); return NOTHING;
    case UNKNOWN:
        return INVALID_INST;
    default: return INVALID_INST;
    }
    return NOTHING;
}

REG ID::get_rs1_content()
{
    return rs1_content;
}

REG ID::get_rs2_content()
{
    return rs2_content;
}

void ID::forward_rs1(REG arg_rs1_content)
{
    rs1_content = arg_rs1_content;
}

void ID::forward_rs2(REG arg_rs2_content)
{
    rs2_content = arg_rs2_content;
}

unsigned int ID::get_rd()
{
    return rd;
}

unsigned long long ID::get_imm64()
{
    return extimm;
}

void ID::set_pc(REG arg_pc)
{
    pc = arg_pc;
}

REG ID::get_pc()
{
    return pc;
}

void ID::print()
{
    printf("  ID:\n");
    printf("    status = %s\n", (err_no==NOTHING?"NTH":"STH"));
    printf("    PC = %llx\n", pc);
    printf("    inst type = %x\n", (unsigned char)type);
    printf("    reg[rs1] = %s = %08llx\n", REG_NAME[rs1], rs1_content);
    printf("    reg[rs2] = %s = %08llx\n", REG_NAME[rs2], rs2_content);
    printf("    extimm = %llx\n", extimm);
}

unsigned long long ID::get_cycles()
{
    return 1;
}

unsigned int ID::get_rs1()
{
    return rs1;
}

unsigned int ID::get_rs2()
{
    return rs2;
}

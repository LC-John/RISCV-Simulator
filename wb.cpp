#include "wb.h"

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

WB::WB()
{
    alu = 0;
    mem_content = 0;
    pc = 0;
    ctrl.BrEq = false;
    ctrl.BrLT = false;
    ctrl.inst32_25_31 = 0;
    ctrl.inst32_2_6 = 0;
    ctrl.inst32_12_14 = 0;
    err_no = NOTHING;
}

void WB::set_pc(REG arg_pc)
{
    pc = arg_pc;
}

void WB::set_Reg(unsigned int arg_rd,
                 unsigned long long arg_alu,
                 unsigned long long arg_mem_content)
{
    rd = arg_rd;
    alu = arg_alu;
    mem_content = arg_mem_content;
}

void WB::set_ctrl(INST32_CTRL_BIT arg_ctrl)
{
    ctrl.BrEq = arg_ctrl.BrEq;
    ctrl.BrLT = arg_ctrl.BrLT;
    ctrl.inst32_2_6 = arg_ctrl.inst32_2_6;
    ctrl.inst32_12_14 = arg_ctrl.inst32_12_14;
    ctrl.inst32_25_31 = arg_ctrl.inst32_25_31;
}

ERROR_NUM WB::WriteBack2Regfile(TEMPORAL_REG* regfile)
{
    switch (get_WBSel())
    {
    case WB_ALU: wb = alu; break;
    case WB_MEM: wb = mem_content; break;
    case WB_PC_PLUS_4: wb = pc + 4; break;
    case WB_DONT_CARE: wb = 0; break;
    case WB_UNKNOWN: return INVALID_INST;
    default: return INVALID_INST;
    }
    switch (get_RegWEn())
    {
    case REGW_N: return NOTHING;
    case REGW_Y: regfile[rd].in = wb; return NOTHING;
    case REGW_UNKNOWN: return INVALID_INST;
    default: return INVALID_INST;
    }
}

unsigned long long WB::get_wb()
{
    return wb;
}

WBSEL_CTRL WB::get_WBSel()
{
    switch (ctrl.inst32_2_6)
    {
    case (0x37 >> 2):
    case (0x17 >> 2):
    case (0x33 >> 2):
    case (0x3b >> 2):
    case (0x13 >> 2):
    case (0x1b >> 2):
        return WB_ALU;
    case (0x6f >> 2):
    case (0x67 >> 2):
        return WB_PC_PLUS_4;
    case (0x63 >> 2):
    case (0x23 >> 2):
        return WB_DONT_CARE;
    case (0x03 >> 2):
        return WB_MEM;
    default:
        return WB_UNKNOWN;
    }
}

REGWEN_CTRL WB::get_RegWEn()
{
    switch (ctrl.inst32_2_6)
    {
    case (0x37 >> 2):
    case (0x17 >> 2):
    case (0x33 >> 2):
    case (0x3b >> 2):
    case (0x13 >> 2):
    case (0x1b >> 2):
    case (0x6f >> 2):
    case (0x67 >> 2):
    case (0x03 >> 2):
        return REGW_Y;
    case (0x63 >> 2):
    case (0x23 >> 2):
        return REGW_N;
    default:
        return REGW_UNKNOWN;
    }
}

REG WB::get_pc()
{
    return pc;
}

void WB::print()
{
    printf("  WB:\n");
    printf("    status = %s\n", (err_no==NOTHING?"NTH":"STH"));
    printf("    PC = %llx\n", pc);
    printf("    inst type = %x\n", (unsigned char)type);
    switch (get_RegWEn())
    {
    case REGW_Y:
        printf("    reg[%d] = %s = %llx\n", rd, REG_NAME[rd], wb); break;
    case REGW_N:
        printf("    Do nothing to regfile.\n"); break;
    default: break;
    }
}

unsigned long long WB::get_cycles()
{
    return 1;
}

INST32_CTRL_BIT WB::get_ctrl()
{
    return ctrl;
}

unsigned int WB::get_rd()
{
    return rd;
}

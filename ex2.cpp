#include "ex2.h"

EX2::EX2()
{
    available = false;
    rd = -1;
    ALU_output = 0;
    pc = 0;
    ctrl.BrEq = false;
    ctrl.BrLT = false;
    ctrl.inst32_25_31 = 0;
    ctrl.inst32_2_6 = 0;
    ctrl.inst32_12_14 = 0;
    err_no = NOTHING;
}

void EX2::set_pc(REG arg_pc)
{
    pc = arg_pc;
}

REG EX2::get_pc()
{
    return pc;
}

bool EX2::get_available()
{
    return available;
}

void EX2::set_available()
{
    switch(get_ALUSel())
    {
    case MUL64:
    case MUL64H:
    case MUL64HSU:
    case MUL64HU:
        available = true;
        break;
    default:
        available = false;
        break;
    }
}

INST32_CTRL_BIT EX2::get_ctrl()
{
    return ctrl;
}

void EX2::set_ctrl(INST32_CTRL_BIT arg_ctrl)
{
    ctrl.BrEq = arg_ctrl.BrEq;
    ctrl.BrLT = arg_ctrl.BrLT;
    ctrl.inst32_25_31 = arg_ctrl.inst32_25_31;
    ctrl.inst32_2_6 = arg_ctrl.inst32_2_6;
    ctrl.inst32_12_14 = arg_ctrl.inst32_12_14;
}

unsigned int EX2::get_rd()
{
    return rd;
}

unsigned long long EX2::get_ALU_output()
{
    return ALU_output;
}

void EX2::set_Reg(unsigned int arg_rd, unsigned long long arg_ALU_output)
{
    rd = arg_rd;
    ALU_output = arg_ALU_output;
}

ALUSEL_CTRL EX2::get_ALUSel()
{
    switch(ctrl.inst32_2_6)
    {
    case (0x37 >> 2):
        return PASSB;
    case (0x17 >> 2):
        return ADD64;
    case (0x6f >> 2):
        return ADD64;
    case (0x63 >> 2):
        return ADD64;
    case (0x23 >> 2):
        return ADD64;
    case (0x33 >> 2):
        switch(ctrl.inst32_12_14)
        {
        case 0x0:
            switch(ctrl.inst32_25_31)
            {
            case 0x00: return ADD64;
            case 0x20: return SUB64;
            case 0x01: return MUL64;
            default:   return ALU_UNKNOWN;
            }
        case 0x1:
            switch(ctrl.inst32_25_31)
            {
            case 0x00: return SLL64;
            case 0x01: return MUL64H;
            default:   return ALU_UNKNOWN;
            }
        case 0x2:
            switch(ctrl.inst32_25_31)
            {
            case 0x00: return SLT64;
            case 0x01: return MUL64HSU;
            default:   return ALU_UNKNOWN;
            }
        case 0x3:
            switch(ctrl.inst32_25_31)
            {
            case 0x00: return SLT64U;
            case 0x01: return MUL64HU;
            default:   return ALU_UNKNOWN;
            }
        case 0x4:
            switch(ctrl.inst32_25_31)
            {
            case 0x00: return XOR64;
            case 0x01: return DIV64;
            default:   return ALU_UNKNOWN;
            }
        case 0x6:
            switch(ctrl.inst32_25_31)
            {
            case 0x00: return OR64;
            case 0x01: return REM64;
            default:   return ALU_UNKNOWN;
            }
        case 0x7:
            switch(ctrl.inst32_25_31)
            {
            case 0x00: return AND64;
            case 0x01: return REM64U;
            default:   return ALU_UNKNOWN;
            }
        case 0x5:
            switch(ctrl.inst32_25_31)
            {
            case 0x00: return SRL64;
            case 0x20: return SRA64;
            case 0x01: return DIV64U;
            default:   return ALU_UNKNOWN;
            }
        default:
            return ALU_UNKNOWN;
        }
    case (0x3b >> 2):
        switch(ctrl.inst32_12_14)
        {
        case 0x0:
            switch(ctrl.inst32_25_31)
            {
            case 0x00: return ADD32;
            case 0x20: return SUB32;
            case 0x01: return MUL32;
            default:   return ALU_UNKNOWN;
            }
        case 0x1:
            if (ctrl.inst32_25_31 != 0x00)
                return ALU_UNKNOWN;
            return SLL32;
        case 0x5:
            switch(ctrl.inst32_25_31)
            {
            case 0x00: return SRL32;
            case 0x20: return SRA32;
            case 0x01: return DIV32U;
            default:   return ALU_UNKNOWN;
            }
        case 0x4:
            if (ctrl.inst32_25_31 != 0x01)
                return ALU_UNKNOWN;
            return DIV32;
        case 0x6:
            if (ctrl.inst32_25_31 != 0x01)
                return ALU_UNKNOWN;
            return REM32;
        case 0x7:   // remuw rd, rs1, rs2
            if (ctrl.inst32_25_31 != 0x01)
                return ALU_UNKNOWN;
            return REM32U;
        default:
            return ALU_UNKNOWN;
        }
    case (0x67 >> 2):
        return ADD64ALIGN;
    case (0x03 >> 2):
        return ADD64;
    case (0x13 >> 2):
        switch(ctrl.inst32_12_14)
        {
        case 0x0: return ADD64;
        case 0x2: return SLT64;
        case 0x3: return SLT64U;
        case 0x4: return XOR64;
        case 0x6: return OR64;
        case 0x7: return AND64;
        case 0x1:
            if ((ctrl.inst32_25_31 >> 1) != 0x00)
                return ALU_UNKNOWN;
            return SLL64;
        case 0x5:
            switch(ctrl.inst32_25_31 >> 1)
            {
            case 0x00: return SRL64;
            case 0x10: return SRA64;
            default:   return ALU_UNKNOWN;
            }
        default:  return ALU_UNKNOWN;
        }
    case (0x1b >> 2):
        switch(ctrl.inst32_12_14)
        {
        case 0x0: return ADD32;
        case 0x1:
            if (ctrl.inst32_25_31 != 0x00)
                return ALU_UNKNOWN;
            return SLL32;
        case 0x5:
            switch(ctrl.inst32_25_31)
            {
            case 0x00: return SRL32;
            case 0x20: return SRA32;
            default:   return ALU_UNKNOWN;
            }
        default: return ALU_UNKNOWN;
        }
    default:
        return ALU_UNKNOWN;
    }
}

void EX2::print()
{
    printf("  EX2:\n");
    if (get_available())
    {
        printf("    Activated!\n");
        printf("    status = %s\n", (err_no==NOTHING?"NTH":"STH"));
        printf("    PC = %llx\n", pc);
        printf("    inst type = %x", (unsigned char)type);
        printf("    ALU output = %llx\n", get_ALU_output());
        printf("    rd = %x", rd);
    }
    else
        printf("    Inactivated!\n");
}

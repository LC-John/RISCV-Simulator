#include "ex.h"

static unsigned long long sext64(unsigned long long num, int sbit)
{
    unsigned long long mask = 0x0;
    if ((num & ((unsigned long long)0x1 << sbit)) != 0)
        mask = (unsigned long long)((long long)0x8000000000000000 >> (63 - sbit));
    return num | mask;
}

EX::EX()
{
    rd = 0;
    rs1 = 0;
    rs2 = 0;
    pc = 0;
    imm64 = 0;
    ALU_output = 0;
    ctrl.BrEq = false;
    ctrl.BrLT = false;
    ctrl.inst32_25_31 = 0;
    ctrl.inst32_2_6 = 0;
    ctrl.inst32_12_14 = 0;
    err_no = NOTHING;
}

void EX::set_RegImm(unsigned int arg_rd,
                    REG arg_rs1,
                    REG arg_rs2,
                    unsigned long long arg_imm64)
{
    rd = arg_rd;
    rs1 = arg_rs1;
    rs2 = arg_rs2;
    imm64 = arg_imm64;
}

void EX::set_pc(REG arg_pc)
{
    pc = arg_pc;
}

void EX::set_ctrl(INST32_CTRL_BIT arg_ctrl)
{
    ctrl.BrEq = arg_ctrl.BrEq;
    ctrl.BrLT = arg_ctrl.BrLT;
    ctrl.inst32_25_31 = arg_ctrl.inst32_25_31;
    ctrl.inst32_2_6 = arg_ctrl.inst32_2_6;
    ctrl.inst32_12_14 = arg_ctrl.inst32_12_14;
}

BRUN_CTRL EX::get_BrUn()
{
    if (ctrl.inst32_2_6 == (0x63 >> 2))
        switch(ctrl.inst32_12_14)
        {
        case 0x0: return BR_DONT_CARE;
        case 0x1: return BR_DONT_CARE;
        case 0x4: return BR_SIGNED;
        case 0x5: return BR_SIGNED;
        case 0x6: return BR_UNSIGNED;
        case 0x7: return BR_UNSIGNED;
        default:  return BR_UNKNOWN;
        }
    else
        return BR_DONT_CARE;
}

BSEL_CTRL EX::get_BSel()
{
    switch(ctrl.inst32_2_6)
    {
    case (0x37 >> 2):
    case (0x17 >> 2):
    case (0x6f >> 2):
    case (0x63 >> 2):
    case (0x23 >> 2):
    case (0x67 >> 2):
    case (0x03 >> 2):
    case (0x13 >> 2):
    case (0x1b >> 2):
        return BSEL_IMM;
    case (0x33 >> 2):
    case (0x3b >> 2):
        return BSEL_RS2;
    default:
        return BSEL_UNKNOWN;
    }
}

ASEL_CTRL EX::get_ASel()
{
    switch(ctrl.inst32_2_6)
    {
    case (0x37 >> 2):
    case (0x17 >> 2):
        return ASEL_PC;
    case (0x6f >> 2):
        return ASEL_PC;
    case (0x63 >> 2):
        return ASEL_PC;
    case (0x23 >> 2):
        return ASEL_RS1;
    case (0x33 >> 2):
    case (0x3b >> 2):
        return ASEL_RS1;
    case (0x67 >> 2):
    case (0x03 >> 2):
    case (0x13 >> 2):
    case (0x1b >> 2):
        return ASEL_RS1;
    default:
        return ASEL_UNKNOWN;
    }
}

PCSEL_CTRL EX::get_PCSel()
{
    if (ctrl.inst32_2_6 == (0x63 >> 2))
        switch(ctrl.inst32_12_14)
        {
        case 0x0:
            if (ctrl.BrEq) return PC_ALU;
            else return PC_PLUS_4;
        case 0x1:
            if (!ctrl.BrEq) return PC_ALU;
            else return PC_PLUS_4;
        case 0x4:
            if (ctrl.BrLT) return PC_ALU;
            else return PC_PLUS_4;
        case 0x5:
            if (!ctrl.BrLT) return PC_ALU;
            else return PC_PLUS_4;
        case 0x6:
            if (ctrl.BrLT) return PC_ALU;
            else return PC_PLUS_4;
        case 0x7:
            if (!ctrl.BrLT) return PC_ALU;
            else return PC_PLUS_4;
        default:
            return PC_UNKNOWN;
        }
    else if (ctrl.inst32_2_6 == (0x6f >> 2))
        return PC_ALU;
    else if (ctrl.inst32_2_6 == (0x67 >> 2))
        return PC_ALU;
    else return PC_PLUS_4;
}

ALUSEL_CTRL EX::get_ALUSel()
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

ERROR_NUM EX::BranchCmp()
{
    ctrl.BrEq = (rs1 == rs2);
    switch(get_BrUn())
    {
    case BR_SIGNED:
        ctrl.BrLT = ((long long)rs1 < (long long)rs2);
        return NOTHING;
    case BR_UNSIGNED:
        ctrl.BrLT = ((unsigned long long)rs1 < (unsigned long long)rs2);
        return NOTHING;
    case BR_DONT_CARE:
        ctrl.BrLT = false;
        return NOTHING;
    default:
        return INVALID_INST;
    }
}

ERROR_NUM EX::ALU()
{
    switch(get_ASel())
    {
    case ASEL_PC: A = pc; break;
    case ASEL_RS1: A = rs1; break;
    default: A = 0; return INVALID_INST;
    }
    switch(get_BSel())
    {
    case BSEL_IMM: B = imm64; break;
    case BSEL_RS2: B = rs2; break;
    default: B = 0; return INVALID_INST;
    }
    switch(get_ALUSel())
    {
    case ADD64: ALU_output = (unsigned long long)((long long)A + (long long)B); break;
    case SUB64: ALU_output = (unsigned long long)((long long)A - (long long)B); break;
    case SLT64: ALU_output = (unsigned long long)(((long long)A < (long long)B) ? 1 : 0); break;
    case SLT64U: ALU_output = (unsigned long long)(((unsigned long long)A < (unsigned long long)B) ? 1 : 0); break;
    case XOR64: ALU_output = (unsigned long long)A ^ (unsigned long long)B; break;
    case OR64: ALU_output = (unsigned long long)A | (unsigned long long)B; break;
    case AND64: ALU_output = (unsigned long long)A & (unsigned long long)B; break;
    case SLL64: ALU_output = (unsigned long long)A << (unsigned long long)B; break;
    case SRL64: ALU_output = ((unsigned long long)A >> (unsigned long long)B); break;
    case SRA64: ALU_output = (unsigned long long)((long long)A >> (long long)B); break;
    case MUL64: ALU_output = (unsigned long long)A * (unsigned long long)B; break;
    case MUL64H: ALU_output = (unsigned long long)0; break;     // to be continued
    case MUL64HSU: ALU_output = (unsigned long long)0; break;   // to be continued
    case MUL64HU: ALU_output = (unsigned long long)0; break;    // to be continued
    case DIV64: ALU_output = (unsigned long long)((long long)A / (long long)B); break;
    case DIV64U: ALU_output = (unsigned long long)A / (unsigned long long)B; break;
    case REM64: ALU_output = (unsigned long long)((long long)A % (long long)B); break;
    case REM64U: ALU_output = (unsigned long long)A % (unsigned long long)B; break;
    case ADD32: ALU_output = sext64((unsigned long long)(unsigned int)((int)A + (int)B), 31); break;
    case SUB32: ALU_output = sext64((unsigned long long)(unsigned int)((int)A - (int)B), 31); break;
    case SLL32: ALU_output = sext64((unsigned long long)(unsigned int)((int)A << ((int)B & 0x1f)), 31); break;
    case SRL32: ALU_output = sext64((unsigned long long)(unsigned int)((unsigned int)A >> ((unsigned int)B & 0x1f)), 31); break;
    case SRA32: ALU_output = sext64((unsigned long long)(unsigned int)((int)A >> ((int)B & 0x1f)), 31); break;
    case MUL32: ALU_output = sext64((unsigned long long)(unsigned int)((int)A * (int)B), 31); break;
    case DIV32: ALU_output = sext64((unsigned long long)(unsigned int)((int)A / (int)B), 31); break;
    case DIV32U: ALU_output = sext64((unsigned long long)(unsigned int)((unsigned int)A / (unsigned int)B), 31); break;
    case REM32: ALU_output = sext64((unsigned long long)(unsigned int)((int)A % (int)B), 31); break;
    case REM32U: ALU_output = sext64((unsigned long long)(unsigned int)((unsigned int)A % (unsigned int)B), 31); break;
    case PASSB: ALU_output = (unsigned long long)B; break;
    case ADD64ALIGN: ALU_output = (((unsigned long long)((unsigned long long)A + (unsigned long long)B)) >> 1) << 1; break;
    case ALU_UNKNOWN: return INVALID_INST;
    default: return INVALID_INST;
    }
    //printf ("%llx op %llx\n", A, B);
    //printf ("%llx\n", ALU_output);
    return NOTHING;
}

REG EX::get_pc()
{
    return pc;
}

REG EX::get_next_pc()
{
    return next_pc;
}

unsigned long long EX::get_ALU_output()
{
    return ALU_output;
}

INST32_CTRL_BIT EX::get_ctrl()
{
    return ctrl;
}

REG EX::get_rs2()
{
    return rs2;
}

unsigned int EX::get_rd()
{
    return rd;
}

ERROR_NUM EX::set_next_pc()
{
    switch(get_PCSel())
    {
    case PC_ALU: next_pc = ALU_output; return NOTHING;
    case PC_PLUS_4: next_pc = pc + 4; return NOTHING;
    case PC_UNKNOWN: return INVALID_INST;
    default: return INVALID_INST;
    }
}

void EX::print()
{
    printf("  EX:\n");
    printf("    status = %s\n", (err_no==NOTHING?"NTH":"STH"));
    printf("    PC = %llx\n", pc);
    printf("    inst type = %x\n", (unsigned char)type);
    printf("    ALU in A = %llx\n", A);
    printf("    ALU in B = %llx\n", B);
    printf("    ALU output = %llx\n", ALU_output);
    printf("    rd = %x\n", rd);
    printf("    next pc = %llx\n", next_pc);
}

unsigned long long EX::get_cycles()
{
    switch(get_ALUSel())
    {
    case MUL64:
    case MUL64H:
    case MUL64HSU:
    case MUL64HU:
        return 2;
    case DIV32:
    case DIV32U:
    case DIV64:
    case DIV64U:
    case REM32:
    case REM32U:
    case REM64:
    case REM64U:
        return 40;
    default:
        return 1;
    }
}

bool EX::is_mul64()
{
    switch(get_ALUSel())
    {
    case MUL64:
    case MUL64H:
    case MUL64HSU:
    case MUL64HU:
        return true;
    default: return false;
    }
}

bool EX::is_divrem()
{
    switch(get_ALUSel())
    {
    case DIV64:
    case DIV64U:
    case REM64:
    case REM64U:
    case DIV32:
    case DIV32U:
    case REM32:
    case REM32U:
        return true;
    default: return false;
    }
}

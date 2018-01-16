#include "mem.h"

static unsigned long long sext64(unsigned long long num, int sbit)
{
    unsigned long long mask = 0x0;
    if ((num & ((unsigned long long)0x1 << sbit)) != 0)
        mask = (unsigned long long)((long long)0x8000000000000000 >> (63 - sbit));
    return num | mask;
}

MEM::MEM()
{
    rd = 0;
    pc = 0;
    alu = 0;
    data = 0;
    mem_content = 0;
    ctrl.BrEq = false;
    ctrl.BrLT = false;
    ctrl.inst32_25_31 = 0;
    ctrl.inst32_2_6 = 0;
    ctrl.inst32_12_14 = 0;
    err_no = NOTHING;
}

void MEM::set_Reg(unsigned int arg_rd,
                  unsigned long long arg_alu,
                  unsigned long long arg_data)
{
    rd = arg_rd;
    alu = arg_alu;
    data = arg_data;
}

void MEM::set_pc(REG arg_pc)
{
    pc = arg_pc;
}

void MEM::set_ctrl(INST32_CTRL_BIT arg_ctrl)
{
    ctrl.BrEq = arg_ctrl.BrEq;
    ctrl.BrLT = arg_ctrl.BrLT;
    ctrl.inst32_25_31 = arg_ctrl.inst32_25_31;
    ctrl.inst32_2_6 = arg_ctrl.inst32_2_6;
    ctrl.inst32_12_14 = arg_ctrl.inst32_12_14;
}

MEMRW_CTRL MEM::get_MemRW()
{
    switch (ctrl.inst32_2_6)
    {
    case (0x03 >> 2):
        switch(ctrl.inst32_12_14)
        {
        case 0x0: return MEM_READ_B;
        case 0x1: return MEM_READ_H;
        case 0x2: return MEM_READ_W;
        case 0x4: return MEM_READ_BU;
        case 0x5: return MEM_READ_HU;
        case 0x6: return MEM_READ_WU;
        case 0x3: return MEM_READ_D;
        default:  return MEM_UNKNOWN;
        }
    case (0x23 >> 2):
        switch(ctrl.inst32_12_14)
        {
        case 0x0: return MEM_WRITE_B;
        case 0x1: return MEM_WRITE_H;
        case 0x2: return MEM_WRITE_W;
        case 0x3: return MEM_WRITE_D;
        default:  return MEM_UNKNOWN;
        }
    default: return MEM_DONT_CARE;
    }
}

ERROR_NUM MEM::MemRW(char* memory)
{
    switch(get_MemRW())
    {
    case MEM_READ_B:
        mem_content = sext64((unsigned long long)(*((unsigned char*)(memory + alu))), 7);
        break;
    case MEM_READ_H:
        if (alu % 2 != 0)
            return INVALID_ADDR_ALIGNMENT;
        mem_content = sext64((unsigned long long)(*((unsigned short*)(memory + alu))), 15);
        break;
    case MEM_READ_W:
        if (alu % 4 != 0)
            return INVALID_ADDR_ALIGNMENT;
        mem_content = sext64((unsigned long long)(*((unsigned int*)(memory + alu))), 31);
        break;
    case MEM_READ_BU:
        mem_content = (unsigned long long)(*((unsigned char*)(memory + alu)));
        break;
    case MEM_READ_HU:
        if (alu % 2 != 0)
            return INVALID_ADDR_ALIGNMENT;
        mem_content = (unsigned long long)(*((unsigned short*)(memory + alu)));
        break;
    case MEM_READ_WU:
        if (alu % 4 != 0)
            return INVALID_ADDR_ALIGNMENT;
        mem_content = (unsigned long long)(*((unsigned int*)(memory + alu)));
        break;
    case MEM_READ_D:
        if (alu % 8 != 0)
            return INVALID_ADDR_ALIGNMENT;
        mem_content = sext64((unsigned long long)(*((unsigned long long*)(memory + alu))), 63);
        break;
    case MEM_WRITE_B:
        *((unsigned char*)(memory + alu)) = (unsigned char)(data & 0xff);
        mem_content = 0;
        break;
    case MEM_WRITE_H:
        if (alu % 2 != 0)
            return INVALID_ADDR_ALIGNMENT;
        *((unsigned short*)(memory + alu)) = (unsigned short)(data & 0xffff);
        mem_content = 0;
        break;
    case MEM_WRITE_W:
        if (alu % 4 != 0)
            return INVALID_ADDR_ALIGNMENT;
        *((unsigned int*)(memory + alu)) = (unsigned int)(data & 0xffffffff);
        mem_content = 0;
        break;
    case MEM_WRITE_D:
        if (alu % 8 != 0)
            return INVALID_ADDR_ALIGNMENT;
        *((unsigned long long*)(memory + alu)) = (unsigned long long)(data & 0xffffffffffffffff);
        mem_content = 0;
        break;
    case MEM_DONT_CARE: mem_content = 0; break;
    case MEM_UNKNOWN: return INVALID_INST;
    default: return INVALID_INST;
    }
    return NOTHING;
}

void MEM::get_meminfo(unsigned long long &addr, int &read, int &size)
{
    switch(get_MemRW())
    {
    case MEM_READ_B:        addr = alu; read = 1; size = 1; break;
    case MEM_READ_H:        addr = alu; read = 1; size = 2; break;
    case MEM_READ_W:        addr = alu; read = 1; size = 4; break;
    case MEM_READ_BU:       addr = alu; read = 1; size = 1; break;
    case MEM_READ_HU:       addr = alu; read = 1; size = 2; break;
    case MEM_READ_WU:       addr = alu; read = 1; size = 4; break;
    case MEM_READ_D:        addr = alu; read = 1; size = 8; break;
    case MEM_WRITE_B:       addr = alu; read = 0; size = 1; break;
    case MEM_WRITE_H:       addr = alu; read = 0; size = 2; break;
    case MEM_WRITE_W:       addr = alu; read = 0; size = 4; break;
    case MEM_WRITE_D:       addr = alu; read = 0; size = 8; break;
    case MEM_DONT_CARE:     addr = alu; read = -1; size = -1; break;
    case MEM_UNKNOWN:       addr = alu; read = -1; size = -1; break;
    default:                addr = alu; read = -1; size = -1; break;
    }
}

unsigned long long MEM::get_mem_content()
{
    return mem_content;
}

REG MEM::get_pc()
{
    return pc;
}

INST32_CTRL_BIT MEM::get_ctrl()
{
    return ctrl;
}

unsigned long long MEM::get_alu()
{
    return alu;
}

unsigned int MEM::get_rd()
{
    return rd;
}

void MEM::print()
{
    printf("  MEM:\n");
    printf("    status = %s\n", (err_no==NOTHING?"NTH":"STH"));
    printf("    PC = %llx\n", pc);
    printf("    inst type = %x\n", (unsigned char)type);
    switch (get_MemRW())
    {
    case MEM_READ_B:
        printf("    Read unsigned byte at %llx\n", alu);
        printf("    Mem content = %llx\n", mem_content); break;
    case MEM_READ_BU:
        printf("    Read byte at %llx\n", alu);
        printf("    Mem content = %llx\n", mem_content); break;
    case MEM_READ_H:
        printf("    Read half-word at %llx\n", alu);
        printf("    Mem content = %llx\n", mem_content); break;
    case MEM_READ_HU:
        printf("    Read unsigned half-word at %llx\n", alu);
        printf("    Mem content = %llx\n", mem_content); break;
    case MEM_READ_W:
        printf("    Read word at %llx\n", alu);
        printf("    Mem content = %llx\n", mem_content); break;
    case MEM_READ_WU:
        printf("    Read unsigned word at %llx\n", alu);
        printf("    Mem content = %llx\n", mem_content); break;
    case MEM_READ_D:
        printf("    Read double-word at %llx\n", alu);
        printf("    Mem content = %llx\n", mem_content); break;
    case MEM_WRITE_B:
        printf("    Write byte at %llx\n", alu);
        printf("    data = %llx\n", data); break;
    case MEM_WRITE_H:
        printf("    Write half-word at %llx\n", alu);
        printf("    data = %llx\n", data); break;
    case MEM_WRITE_W:
        printf("    Write word at %llx\n", alu);
        printf("    data = %llx\n", data); break;
    case MEM_WRITE_D:
        printf("    Write double-word at %llx\n", alu);
        printf("    data = %llx\n", data); break;
    case MEM_DONT_CARE:
        printf("    Do nothing to mem.\n"); break;
    case MEM_UNKNOWN: break;
    default: break;
    }
}

unsigned long long MEM::get_cycles()
{
    return 1;
}

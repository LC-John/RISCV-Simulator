#include "pipeline_sim.h"
#include <assert.h>

#define USE_BYPASS
//#define STATIC_PRED

Pipeline_Sim::Pipeline_Sim():
    elf_reader(), inst_fetcher(), inst_identifier(),
    inst_executor(), inst_multiplier(), inst_memory(),
    inst_writor()
{
#ifdef STATIC_PRED
#ifdef DYNAMIC_PRED
    assert(false);
#endif
#endif
    err_no = NOTHING;
    inst_cnt = 0;
    cycle_cnt = 0;
    control_hazard_cnt = 0;
    wrong_branch_cnt = 0;
    load_use_hazard_cnt =0;
    divrem_timer = 0;
    ex2_active = false;
    memset(sigNope, 0, sizeof(sigNope));
    memset(sigStall, 0, sizeof(sigStall));
    memset(memory, 0, sizeof(memory));
    memset(regfile, 0, sizeof(regfile));
}

bool Pipeline_Sim::readelf(char* filename, char* elfname, char** vals, int n_val)
{
    return elf_reader.read_elf(filename, elfname, vals, n_val);
}

bool Pipeline_Sim::init(char* filename, char *elfname, char **vals, int n_val)
{
    if (!readelf(filename, elfname, vals, n_val))
        return false;
    init_RegMem();
    load_memory(filename);
    inst_fetcher.set_next_pc((REG)elf_reader.madr);
    inst_fetcher.set_pc((REG)elf_reader.madr);
    regfile[3].in = (REG)elf_reader.gp;
    regfile[3].out = (REG)elf_reader.gp;
    regfile[2].out = (REG)FS_SP_DEFAULT;
    regfile[2].in = (REG)FS_SP_DEFAULT;
    regfile[0].in = 0;
    regfile[0].out = 0;
    inst_cnt = 0;
    cycle_cnt = 0;
    control_hazard_cnt = 0;
    b_control_cnt = 0;
    jalr_control_cnt = 0;
    wrong_branch_cnt = 0;
    load_use_hazard_cnt =0;
    ex2_active = false;
    divrem_timer = 0;
    return true;
}

void Pipeline_Sim::one_step(unsigned int verbose)
{
    if (verbose > 0)
        printf("cycle %llu, inst %llu\n", cycle_cnt, inst_cnt);
    update_latch();
    regfile[0].in = 0;
    regfile[0].out = 0;
    update_component(verbose);
    if (verbose > 1)
    {
        printf("NOPE: ");
        for (int i = 0; i < 6; i++)
            if (sigNope[i])
                printf("NOP ");
            else
                printf("NTH ");
        printf("\nSTAL: ");
        for (int i = 0; i < 6; i++)
            if (sigStall[i])
                printf("STL ");
            else
                printf("NTH ");
        printf("\n");
    }
    update_regfile();
    regfile[0].in = 0;
    regfile[0].out = 0;
    sigStall[STAGE_IF] = false;
    sigStall[STAGE_ID] = false;
    sigStall[STAGE_EX] = false;
    sigStall[STAGE_EX2] = false;
    sigStall[STAGE_MEM] = false;
    sigStall[STAGE_WB] = false;
    check_status();
    wrong_branch(verbose);
    control_hazard(verbose);
    load_use_hazard(verbose);
    multiply_conflict(verbose);
    divrem(verbose);
    nope_transfer();
    cycle_cnt++;
}

void Pipeline_Sim::sim(unsigned int verbose)
{
    while(true)
    {
        one_step(verbose);
        if (err_no != NOTHING)
            return;
    }
}

ERROR_NUM Pipeline_Sim::get_errno()
{
    return err_no;
}

REG Pipeline_Sim::read_regfile(int idx)
{
    if (idx < 0 || idx > 31)
        return 0xDEADBEEF;
    return regfile[idx].out;
}

char Pipeline_Sim::read_memory(long long addr)
{
    if (addr < 0 || addr > FS_MAX_MEMORY)
        return 0xFA;
    return memory[addr];
}

void Pipeline_Sim::print_res()
{
    printf("\nPipeline-sim Mode Done!\n");
#ifdef USE_BYPASS
    printf("Bypass technique applied.\n");
#else
    printf("Do nothing about data hazard.\n");
#endif
#ifdef DYNAMIC_PRED
    printf("Dynamic branch prediction technique applied.\n");
#else
#ifdef STATIC_PRED
    printf("Static branch prediction technique applied.\n");
#else
    printf("Do nothing about control hazard.\n");
#endif
#endif

    if (err_no == INVALID_PC)
    {
        printf("\nInvalid PC: 0x%016llx\n", err_loc);
        return;
    }
    if (err_no == INVALID_INST)
    {
        printf("\nInvalid inst: [%llxx] %08x\n", err_loc, inst_fetcher.get_inst32());
        return;
    }
    if (err_no == INVALID_ADDR_ALIGNMENT)
    {
        printf("\nInvalid address alignment: [%llxx] %08x\n", err_loc, inst_fetcher.get_inst32());
        return;
    }
    if (err_no == HALT)
        printf("\nMachine halt!\n\n");

    printf("CPI = %f\n", (float)cycle_cnt/(float)inst_cnt);
    printf("  Instructions = %llu\n", inst_cnt);
    printf("  Cycles = %llu\n", cycle_cnt);
    printf("Ctrl hazard = %llu\n", control_hazard_cnt);
    printf("  Branch caused = %llu\n", b_control_cnt);
    printf("  Jalr caused = %llu\n", jalr_control_cnt);
    printf("  Wrong pred = %llu\n", wrong_branch_cnt);
    printf("Data hazard = %llu\n", load_use_hazard_cnt);

    for (unsigned long long val_i = 0; val_i < elf_reader.val_n; val_i++)
    {
        printf("memory block %llu, %s:\n", val_i, elf_reader.val_name[val_i]);
        for (unsigned long long a = 0; a < elf_reader.val_size[val_i] / 4; a++)
            printf("0x%llx - 0x%llx: %02x %02x %02x %02x\n",
                   elf_reader.val_adr[val_i] + 4 * a, elf_reader.val_adr[val_i] + 4 * a + 3,
                   (unsigned char)memory[elf_reader.val_adr[val_i] + 4 * a],
                   (unsigned char)memory[elf_reader.val_adr[val_i] + 4 * a + 1],
                   (unsigned char)memory[elf_reader.val_adr[val_i] + 4 * a + 2],
                   (unsigned char)memory[elf_reader.val_adr[val_i] + 4 * a + 3]);
    }
}

void Pipeline_Sim::init_RegMem()
{
    sigNope[0] = false;
    for (int i = 1; i < 6; i++)
        sigNope[i] = true;
    for (int j = 0; j < 6; j++)
        sigStall[j] = false;
    memset(regfile, 0, sizeof(regfile));
    memset(memory, 0, sizeof(memory));
}

void Pipeline_Sim::load_memory(char* filename)
{
    FILE* file = fopen(filename, "r");

    fseek(file, elf_reader.cadr, 0);
    fread(&memory[elf_reader.cvadr], 1, elf_reader.csize, file);
    fseek(file, elf_reader.dadr, 0);
    fread(&memory[elf_reader.dvadr], 1, elf_reader.dsize, file);

    fclose(file);
}

void Pipeline_Sim::update_regfile()
{
    for (int i = 0; i < 32; i++)
        regfile[i].update();
}

void Pipeline_Sim::update_latch()
{
    if (!sigStall[STAGE_WB])
    {
        inst_writor.set_pc(inst_memory.get_pc());
        inst_writor.set_Reg(inst_memory.get_rd(),
                            inst_memory.get_alu(),
                            inst_memory.get_mem_content());
        inst_writor.set_ctrl(inst_memory.get_ctrl());
        inst_writor.err_no = inst_memory.err_no;
        inst_writor.type = inst_memory.type;
    }
    if (!sigStall[STAGE_MEM])
    {
        if (ex2_active)
        {
            inst_memory.set_pc(inst_multiplier.get_pc());
            inst_memory.set_Reg(inst_multiplier.get_rd(),
                                inst_multiplier.get_ALU_output(),
                                0);
            inst_memory.set_ctrl(inst_multiplier.get_ctrl());
            inst_memory.err_no = inst_multiplier.err_no;
            inst_memory.type = inst_multiplier.type;
        }
        else
        {
            inst_memory.set_pc(inst_executor.get_pc());
            inst_memory.set_Reg(inst_executor.get_rd(),
                                inst_executor.get_ALU_output(),
                                inst_executor.get_rs2());
            inst_memory.set_ctrl(inst_executor.get_ctrl());
            inst_memory.err_no = inst_executor.err_no;
            inst_memory.type = inst_executor.type;
        }
    }
    if (!sigStall[STAGE_EX2] && !sigNope[STAGE_EX])
    {
        inst_multiplier.set_ctrl(inst_executor.get_ctrl());
        inst_multiplier.set_pc(inst_executor.get_pc());
        inst_multiplier.set_Reg(inst_executor.get_rd(),
                                inst_executor.get_ALU_output());
        inst_multiplier.err_no = inst_executor.err_no;
        inst_multiplier.type = inst_executor.type;
    }
    if (!sigStall[STAGE_EX])
    {
        inst_executor.set_pc(inst_identifier.get_pc());
        inst_executor.set_RegImm(inst_identifier.get_rd(),
                                 inst_identifier.get_rs1_content(),
                                 inst_identifier.get_rs2_content(),
                                 inst_identifier.get_imm64());
        inst_executor.set_ctrl(inst_identifier.get_ctrl());
        inst_executor.err_no = inst_identifier.err_no;
        inst_executor.type = inst_identifier.type;
    }
    if (!sigStall[STAGE_ID])
    {
        inst_identifier.set_pc(inst_fetcher.get_pc());
        inst_identifier.set_RegImm(inst_fetcher.get_imm_set(),
                                   inst_fetcher.get_rd(),
                                   inst_fetcher.get_rs1(),
                                   inst_fetcher.get_rs2());
        inst_identifier.set_ctrl(inst_fetcher.get_ctrl_bit());
        inst_identifier.err_no = inst_fetcher.err_no;
        inst_identifier.type = inst_fetcher.type;
    }
    if (!sigStall[STAGE_IF])
    {
        inst_fetcher.set_inst32(memory);
        inst_fetcher.err_no = NOTHING;
    }
}

void Pipeline_Sim::update_component(int verbose)
{
    if (!sigNope[STAGE_IF])
    {
        inst_fetcher.split32();
        inst_fetcher.set_type();
        if (verbose > 1)
            inst_fetcher.print();
    }
    else if (verbose > 1)
        printf("  IF:\n    NOP\n");
    if (!sigNope[STAGE_ID])
    {
        inst_identifier.read_regfile(regfile);
        inst_identifier.err_no = inst_identifier.imm_extension();
        if (verbose > 1)
            inst_identifier.print();
    }
    else if (verbose > 1)
        printf("  ID:\n    NOP\n");
    if (!sigNope[STAGE_EX])
    {
        if (inst_executor.err_no == NOTHING)
        {
            inst_executor.err_no = inst_executor.BranchCmp();
            if (inst_executor.err_no == NOTHING)
            {
                inst_executor.err_no = inst_executor.ALU();
                if (inst_executor.err_no == NOTHING)
                    inst_executor.err_no = inst_executor.set_next_pc();
            }
        }
        if (verbose > 1)
            inst_executor.print();
    }
    else if (verbose > 1)
        printf("  EX:\n    NOP\n");
    if (!sigNope[STAGE_EX2])
    {
        if (inst_multiplier.err_no == NOTHING)
            inst_multiplier.set_available();
        if (verbose > 1)
            inst_multiplier.print();
    }
    else if (verbose > 1)
        printf("  EX2:\n    NOP\n    %s\n", ex2_active?"Activated!":"Inactivated!");
    if (!sigNope[STAGE_MEM])
    {
        if (inst_memory.err_no == NOTHING)
            inst_memory.err_no = inst_memory.MemRW(memory);
        if (verbose > 1)
            inst_memory.print();
    }
    else if (verbose > 1)
        printf("  MEM:\n    NOP\n");
    if (!sigNope[STAGE_WB])
    {
        if (inst_writor.err_no == NOTHING)
            err_no = inst_writor.WriteBack2Regfile(regfile);
        if(verbose > 1)
            inst_writor.print();
    }
    else if (verbose > 1)
        printf("  WB:\n    NOP\n");
}

void Pipeline_Sim::wrong_branch(int verbose)
{
    if (!sigNope[STAGE_EX] && (inst_executor.get_next_pc() != inst_identifier.get_pc()))
    {
        if (verbose > 1)
            printf("  Wrong branch detected!\n");
        wrong_branch_cnt++;
        sigNope[STAGE_IF] = true;
        sigNope[STAGE_ID] = true;
        inst_fetcher.set_next_pc(inst_executor.get_next_pc());
    }
    else if (verbose > 1)
        printf("  No wrong branch detected!\n");
}

void Pipeline_Sim::control_hazard(int verbose)
{
    if (inst_fetcher.type == 'B')
    {
        if (verbose > 1)
            printf("  Control hazard detected!\n");
        control_hazard_cnt++;
        b_control_cnt++;
#ifdef DYNAMIC_PRED
        inst_fetcher.set_next_pc(inst_fetcher.get_pc() + 4);
#else
#ifdef STATIC_PRED
        if ((long long)sext64((unsigned long long)inst_fetcher.get_SB_imm(), 12) < 0)
            inst_fetcher.set_next_pc((long long)sext64((unsigned long long)inst_fetcher.get_SB_imm(), 12) + inst_fetcher.get_pc());
        else
            inst_fetcher.set_next_pc(inst_fetcher.get_pc() + 4);
#else
        inst_fetcher.set_next_pc(inst_fetcher.get_pc() + 4);
#endif
#endif
    }
    else if (inst_fetcher.get_opcode() == 0x6f)
    {
        if (verbose > 1)
            printf("  Control hazard detected!\n");
        inst_fetcher.set_next_pc(inst_fetcher.get_pc()
                                 + sext64(inst_fetcher.get_UJ_imm(), 20));
    }
    else if (inst_fetcher.get_opcode() == 0x67)
    {
        if (verbose > 1)
            printf("  Control hazard detected!\n");
        control_hazard_cnt++;
        jalr_control_cnt++;
#ifdef DYNAMIC_PRED
        inst_fetcher.set_next_pc(inst_fetcher.get_pc() + 4);
#else
#ifdef STATIC_PRED
        inst_fetcher.set_next_pc(inst_fetcher.get_pc() + 4);
#else
        inst_fetcher.set_next_pc(inst_fetcher.get_pc() + 4);
#endif
#endif
    }
    else if (verbose > 1)
        printf ("  No cotrol hazard detected!\n");
}

bool Pipeline_Sim::check_load_use_hazard()
{
    switch(inst_identifier.type)
    {
    case 'R':
    case 'S':
    case 'B':
#ifdef USE_BYPASS
        if ((!sigNope[STAGE_WB]) && RegWEn(inst_writor.get_ctrl()) && (inst_writor.get_rd() == inst_identifier.get_rs1() || inst_writor.get_rd() == inst_identifier.get_rs2()))
        {
            if (inst_writor.get_rd() == inst_identifier.get_rs1())
                inst_identifier.forward_rs1(inst_writor.get_wb());
            else
                inst_identifier.forward_rs2(inst_writor.get_wb());
        }
        if ((!sigNope[STAGE_MEM]) && RegWEn(inst_memory.get_ctrl()) && (inst_memory.get_rd() == inst_identifier.get_rs1() || inst_memory.get_rd() == inst_identifier.get_rs2()))
        {
            if (inst_memory.type == 'L')
            {
                if (inst_memory.get_rd() == inst_identifier.get_rs1())
                    inst_identifier.forward_rs1(inst_memory.get_mem_content());
                else
                    inst_identifier.forward_rs2(inst_memory.get_mem_content());
            }
            else
            {
                if (inst_memory.get_rd() == inst_identifier.get_rs1())
                    inst_identifier.forward_rs1(inst_memory.get_alu());
                else
                    inst_identifier.forward_rs2(inst_memory.get_alu());
            }
        }
        if ((!sigNope[STAGE_EX2]) && ex2_active && RegWEn(inst_multiplier.get_ctrl()) && (inst_multiplier.get_rd() == inst_identifier.get_rs1() || inst_multiplier.get_rd() == inst_identifier.get_rs2()))
        {
            if (inst_multiplier.get_rd() == inst_identifier.get_rs1())
                inst_identifier.forward_rs1(inst_multiplier.get_ALU_output());
            else
                inst_identifier.forward_rs2(inst_multiplier.get_ALU_output());
        }
        if ((!sigNope[STAGE_EX]) && RegWEn(inst_executor.get_ctrl()) && (inst_executor.get_rd() == inst_identifier.get_rs1() || inst_executor.get_rd() == inst_identifier.get_rs2()))
        {
            if (inst_executor.type == 'L')
                return true;
            else
            {
                if (inst_executor.get_rd() == inst_identifier.get_rs1())
                    inst_identifier.forward_rs1(inst_executor.get_ALU_output());
                else
                    inst_identifier.forward_rs2(inst_executor.get_ALU_output());
            }
        }
#else
        if (((!sigNope[STAGE_EX]) && RegWEn(inst_executor.get_ctrl()) && (inst_executor.get_rd() == inst_identifier.get_rs1() || inst_executor.get_rd() == inst_identifier.get_rs2()))
                || ((!sigNope[STAGE_EX2]) && ex2_active && RegWEn(inst_multiplier.get_ctrl()) && (inst_multiplier.get_rd() == inst_identifier.get_rs1() || inst_multiplier.get_rd() == inst_identifier.get_rs2()))
                || ((!sigNope[STAGE_MEM]) && RegWEn(inst_memory.get_ctrl()) && (inst_memory.get_rd() == inst_identifier.get_rs1() || inst_memory.get_rd() == inst_identifier.get_rs2()))
                || ((!sigNope[STAGE_WB]) && RegWEn(inst_writor.get_ctrl()) && (inst_writor.get_rd() == inst_identifier.get_rs1() || inst_writor.get_rd() == inst_identifier.get_rs2())))
            return true;
#endif
        return false;
    case 'I':
    case 'L':
#ifdef USE_BYPASS
        if ((!sigNope[STAGE_WB]) && RegWEn(inst_writor.get_ctrl()) && inst_writor.get_rd() == inst_identifier.get_rs1())
            inst_identifier.forward_rs1(inst_writor.get_wb());
        if ((!sigNope[STAGE_MEM]) && RegWEn(inst_memory.get_ctrl()) && inst_memory.get_rd() == inst_identifier.get_rs1())
        {
            if (inst_memory.type == 'L')
                inst_identifier.forward_rs1(inst_memory.get_mem_content());
            else
                inst_identifier.forward_rs1(inst_memory.get_alu());
        }
        if ((!sigNope[STAGE_EX2]) && ex2_active && RegWEn(inst_multiplier.get_ctrl()) && inst_multiplier.get_rd() == inst_identifier.get_rs1())
            inst_identifier.forward_rs1(inst_multiplier.get_ALU_output());
        if ((!sigNope[STAGE_EX]) && RegWEn(inst_executor.get_ctrl()) && inst_executor.get_rd() == inst_identifier.get_rs1())
        {
            if (inst_executor.type == 'L')
                return true;
            else
                inst_identifier.forward_rs1(inst_executor.get_ALU_output());
        }
#else
        if (((!sigNope[STAGE_EX]) && RegWEn(inst_executor.get_ctrl()) && inst_executor.get_rd() == inst_identifier.get_rs1())
                || ((!sigNope[STAGE_EX2]) && ex2_active && RegWEn(inst_multiplier.get_ctrl()) && inst_multiplier.get_rd() == inst_identifier.get_rs1())
                || ((!sigNope[STAGE_MEM]) && RegWEn(inst_memory.get_ctrl()) && inst_memory.get_rd() == inst_identifier.get_rs1())
                || ((!sigNope[STAGE_WB]) && RegWEn(inst_writor.get_ctrl()) && inst_writor.get_rd() == inst_identifier.get_rs1()))
            return true;
#endif
        return false;
    case 'U':
    case 'J':
        return false;
    default:
        return false;
    }
}

void Pipeline_Sim::load_use_hazard(int verbose)
{
    if (check_load_use_hazard())
    {
        if (verbose > 1)
            printf("  Load-use hazard detected!\n");
        load_use_hazard_cnt++;
        sigStall[STAGE_IF] = true;
        sigStall[STAGE_ID] = true;
    }
    else if (verbose > 1)
        printf("  No load-use hazard detected!\n");
}

bool Pipeline_Sim::check_multiply_conflict()
{
    if (!ex2_active || sigNope[STAGE_EX2])
        return false;
    if (inst_executor.is_mul64() || inst_executor.is_divrem())
        return false;
    return true;
}

void Pipeline_Sim::multiply_conflict(int verbose)
{
    if (check_multiply_conflict())
    {
        if (verbose > 1)
            printf("  Multiply conflict detected!\n");
        sigStall[STAGE_ID] = true;
        sigStall[STAGE_IF] = true;
        sigStall[STAGE_EX] = true;
    }
    else if (verbose > 1)
        printf("  No multiply conflict detected!\n");
}

void Pipeline_Sim::divrem(int verbose)
{
    if (!sigNope[STAGE_EX] && inst_executor.is_divrem())
    {
        if (divrem_timer == 0)
            divrem_timer = 39;
        divrem_timer--;
        if (divrem_timer > 0)
        {
            if (verbose > 1)
                printf("  Div-rem stall detected!\n");
            sigStall[STAGE_ID] = true;
            sigStall[STAGE_IF] = true;
            sigStall[STAGE_EX] = true;
        }
        else if (verbose > 1)
            printf("  No div-rem stall detected!\n");
    }
    else if (verbose > 1)
        printf("  No div-rem stall detected!\n");
}

void Pipeline_Sim::nope_transfer()
{
    int idx;
    for (idx = 0; idx < 6 && sigStall[idx]; idx++);
    if (idx < 5)
        out_nope = true;
    else
        out_nope = sigNope[STAGE_WB];
    for (int i = 5; i > idx; i--)
        if (i == STAGE_MEM && ex2_active)
            sigNope[i] = sigNope[STAGE_EX2];
        else if (i == STAGE_MEM)
            sigNope[i] = sigNope[STAGE_EX];
        else
            sigNope[i] = sigNope[i - 1];
    if (idx < 6)
        sigNope[idx] = true;
    if (!sigStall[STAGE_IF])
        sigNope[STAGE_IF] = false;
}

void Pipeline_Sim::check_status()
{
    if (!sigNope[STAGE_WB] && !sigStall[STAGE_WB])
    {
        REG pc = inst_writor.get_pc();
        err_no = inst_writor.err_no;
        err_loc = inst_writor.get_pc();
        if (pc < (unsigned long long)elf_reader.cvadr || pc > (unsigned long long)(elf_reader.cvadr + elf_reader.csize))
            err_no = INVALID_PC;
        if (pc % 4 != 0)
            err_no = INVALID_PC;
        if (pc == elf_reader.mend - 4)
            err_no = HALT;
        if (err_no != NOTHING)
            return;
        inst_cnt++;
    }
}

unsigned long long Pipeline_Sim::sext64(unsigned long long num, int sbit)
{
    unsigned long long mask = 0x0;
    if ((num & ((unsigned long long)0x1 << sbit)) != 0)
        mask = (unsigned long long)((long long)0x8000000000000000 >> (63 - sbit));
    return num | mask;
}

bool Pipeline_Sim::RegWEn(INST32_CTRL_BIT ctrl)
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
        return true;
    case (0x63 >> 2):
    case (0x23 >> 2):
        return false;
    default:
        return false;
    }
}

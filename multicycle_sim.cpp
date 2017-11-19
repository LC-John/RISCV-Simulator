#include "multicycle_sim.h"
#include <string.h>

Multicycle_Sim::Multicycle_Sim():
    elf_reader(), inst_fetcher(), inst_identifier(),
    inst_executor(), inst_memory(), inst_writor()
{
    err_no = NOTHING;
    inst_cnt = 0;
    cycle_cnt = 0;
    memset(memory, 0, sizeof(memory));
    memset(regfile, 0, sizeof(regfile));
}

bool Multicycle_Sim::readelf(char* filename, char* elfname, char** vals, int n_val)
{
    return elf_reader.read_elf(filename, elfname, vals, n_val);
}

bool Multicycle_Sim::init(char* filename, char *elfname, char **vals, int n_val)
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
    return true;
}

void Multicycle_Sim::IFID_PROCEDURE(unsigned int verbose)
{
    inst_fetcher.set_inst32(memory);
    inst_fetcher.split32();
    inst_fetcher.set_type();
    type = inst_fetcher.get_type();
    if (verbose > 0)
        inst_fetcher.print_assembly();
    cycle_cnt += inst_fetcher.get_cycles();
    if (verbose > 1)
        inst_fetcher.print();

    inst_identifier.set_pc(inst_fetcher.get_pc());
    inst_identifier.set_RegImm(inst_fetcher.get_imm_set(),
                               inst_fetcher.get_rd(),
                               inst_fetcher.get_rs1(),
                               inst_fetcher.get_rs2());
    inst_identifier.set_ctrl(inst_fetcher.get_ctrl_bit());
    inst_identifier.read_regfile(regfile);
    err_no = inst_identifier.imm_extension();
    if (err_no != NOTHING)
    {
        err_loc = inst_identifier.get_pc();
        return;
    }
    cycle_cnt += inst_identifier.get_cycles();
    if (verbose > 1)
        inst_identifier.print();
}

void Multicycle_Sim::EX_PROCEDURE(unsigned int verbose)
{
    inst_executor.set_pc(inst_identifier.get_pc());
    inst_executor.set_RegImm(inst_identifier.get_rd(),
                             inst_identifier.get_rs1_content(),
                             inst_identifier.get_rs2_content(),
                             inst_identifier.get_imm64());
    inst_executor.set_ctrl(inst_identifier.get_ctrl());
    err_no = inst_executor.BranchCmp();
    if (err_no != NOTHING)
    {
        err_loc = inst_executor.get_pc();
        return;
    }
    err_no = inst_executor.ALU();
    if (err_no != NOTHING)
    {
        err_loc = inst_executor.get_pc();
        return;
    }
    err_no = inst_executor.set_next_pc();
    if (err_no != NOTHING)
    {
        err_loc = inst_executor.get_pc();
        return;
    }
    inst_fetcher.set_next_pc(inst_executor.get_next_pc());
    cycle_cnt += inst_executor.get_cycles();
    if (verbose > 1)
        inst_executor.print();
}

void Multicycle_Sim::EXMEM_PROCEDURE(unsigned int verbose)
{
    inst_executor.set_pc(inst_identifier.get_pc());
    inst_executor.set_RegImm(inst_identifier.get_rd(),
                             inst_identifier.get_rs1_content(),
                             inst_identifier.get_rs2_content(),
                             inst_identifier.get_imm64());
    inst_executor.set_ctrl(inst_identifier.get_ctrl());
    err_no = inst_executor.BranchCmp();
    if (err_no != NOTHING)
    {
        err_loc = inst_executor.get_pc();
        return;
    }
    err_no = inst_executor.ALU();
    if (err_no != NOTHING)
    {
        err_loc = inst_executor.get_pc();
        return;
    }
    err_no = inst_executor.set_next_pc();
    if (err_no != NOTHING)
    {
        err_loc = inst_executor.get_pc();
        return;
    }
    inst_fetcher.set_next_pc(inst_executor.get_next_pc());
    cycle_cnt += inst_executor.get_cycles();
    if (verbose > 1)
        inst_executor.print();
    inst_memory.set_pc(inst_executor.get_pc());
    inst_memory.set_Reg(inst_executor.get_rd(),
                        inst_executor.get_ALU_output(),
                        inst_executor.get_rs2());
    inst_memory.set_ctrl(inst_executor.get_ctrl());
    err_no = inst_memory.MemRW(memory);
    if (err_no != NOTHING)
    {
        err_loc = inst_memory.get_pc();
        return;
    }
    cycle_cnt += inst_memory.get_cycles();
    if (verbose > 1)
        inst_memory.print();
}

void Multicycle_Sim::EXWB_PROCEDURE(unsigned int verbose)
{
    inst_executor.set_pc(inst_identifier.get_pc());
    inst_executor.set_RegImm(inst_identifier.get_rd(),
                             inst_identifier.get_rs1_content(),
                             inst_identifier.get_rs2_content(),
                             inst_identifier.get_imm64());
    inst_executor.set_ctrl(inst_identifier.get_ctrl());
    err_no = inst_executor.BranchCmp();
    if (err_no != NOTHING)
    {
        err_loc = inst_executor.get_pc();
        return;
    }
    err_no = inst_executor.ALU();
    if (err_no != NOTHING)
    {
        err_loc = inst_executor.get_pc();
        return;
    }
    err_no = inst_executor.set_next_pc();
    if (err_no != NOTHING)
    {
        err_loc = inst_executor.get_pc();
        return;
    }
    inst_fetcher.set_next_pc(inst_executor.get_next_pc());
    cycle_cnt += inst_executor.get_cycles();
    if (verbose > 1)
        inst_executor.print();
    inst_writor.set_pc(inst_executor.get_pc());
    inst_writor.set_Reg(inst_executor.get_rd(),
                        inst_executor.get_ALU_output(), 0);
    inst_writor.set_ctrl(inst_executor.get_ctrl());
    err_no = inst_writor.WriteBack2Regfile(regfile);
    if (err_no != NOTHING)
    {
        err_loc = inst_writor.get_pc();
        return;
    }
    cycle_cnt += inst_writor.get_cycles();
    if (verbose > 1)
        inst_writor.print();
}

void Multicycle_Sim::EXMEMWB_PROCEDURE(unsigned int verbose)
{
    inst_executor.set_pc(inst_identifier.get_pc());
    inst_executor.set_RegImm(inst_identifier.get_rd(),
                             inst_identifier.get_rs1_content(),
                             inst_identifier.get_rs2_content(),
                             inst_identifier.get_imm64());
    inst_executor.set_ctrl(inst_identifier.get_ctrl());
    err_no = inst_executor.BranchCmp();
    if (err_no != NOTHING)
    {
        err_loc = inst_executor.get_pc();
        return;
    }
    err_no = inst_executor.ALU();
    if (err_no != NOTHING)
    {
        err_loc = inst_executor.get_pc();
        return;
    }
    err_no = inst_executor.set_next_pc();
    if (err_no != NOTHING)
    {
        err_loc = inst_executor.get_pc();
        return;
    }
    inst_fetcher.set_next_pc(inst_executor.get_next_pc());
    cycle_cnt += inst_executor.get_cycles();
    if (verbose > 1)
        inst_executor.print();
    inst_memory.set_pc(inst_executor.get_pc());
    inst_memory.set_Reg(inst_executor.get_rd(),
                        inst_executor.get_ALU_output(),
                        inst_executor.get_rs2());
    inst_memory.set_ctrl(inst_executor.get_ctrl());
    err_no = inst_memory.MemRW(memory);
    if (err_no != NOTHING)
    {
        err_loc = inst_memory.get_pc();
        return;
    }
    cycle_cnt += inst_memory.get_cycles();
    if (verbose > 1)
        inst_memory.print();
    inst_writor.set_pc(inst_memory.get_pc());
    inst_writor.set_Reg(inst_memory.get_rd(),
                        inst_memory.get_alu(),
                        inst_memory.get_mem_content());
    inst_writor.set_ctrl(inst_memory.get_ctrl());
    err_no = inst_writor.WriteBack2Regfile(regfile);
    if (err_no != NOTHING)
    {
        err_loc = inst_writor.get_pc();
        return;
    }
    cycle_cnt += inst_writor.get_cycles();
    if (verbose > 1)
        inst_writor.print();
}

void Multicycle_Sim::one_step(unsigned int verbose)
{
    REG pc = inst_fetcher.get_pc();
    if (pc < (unsigned long long)elf_reader.cvadr || pc > (unsigned long long)(elf_reader.cvadr + elf_reader.csize))
        err_no = INVALID_PC;
    if (pc % 4 != 0)
        err_no = INVALID_PC;
    if (pc == elf_reader.mend - 4)
        err_no = HALT;
    if (err_no != NOTHING)
        return;

    if (verbose > 0)
        printf("[%lld] %llx: ", inst_cnt, pc);

    regfile[0].out = 0;

    IFID_PROCEDURE(verbose);
    switch(type)
    {
    case 'U':
    case 'I':
    case 'J':
    case 'R':
        EXWB_PROCEDURE(verbose); break;
    case 'B':
        EX_PROCEDURE(verbose); break;
    case 'S':
        EXMEM_PROCEDURE(verbose); break;
    case 'L':
        EXMEMWB_PROCEDURE(verbose); break;
    default: break;
    }

    regfile[0].out = 0;
    regfile[0].in = 0;

    update_regfile();
    inst_cnt++;
}

void Multicycle_Sim::sim(unsigned int verbose)
{
    while(true)
    {
        one_step(verbose);
        if (err_no != NOTHING)
            return;
    }
}

ERROR_NUM Multicycle_Sim::get_errno()
{
    return err_no;
}

REG Multicycle_Sim::read_regfile(int idx)
{
    if (idx < 0 || idx > 31)
        return 0xDEADBEEF;
    return regfile[idx].out;
}

char Multicycle_Sim::read_memory(long long addr)
{
    if (addr < 0 || addr > FS_MAX_MEMORY)
        return 0xFA;
    return memory[addr];
}

void Multicycle_Sim::print_res()
{
    printf("\nMulticycle-sim Mode Done!\n");

    if (err_no == INVALID_PC)
    {
        printf("Invalid PC: 0x%016llx\n", err_loc);
        return;
    }
    if (err_no == INVALID_INST)
    {
        printf("Invalid inst: [%llxx] %08x\n", err_loc, inst_fetcher.get_inst32());
        return;
    }
    if (err_no == INVALID_ADDR_ALIGNMENT)
    {
        printf("Invalid address alignment: [%llxx] %08x\n", err_loc, inst_fetcher.get_inst32());
        return;
    }
    if (err_no == HALT)
        printf("Machine halt!\n");

    printf("Instructions = %llu\n", inst_cnt);
    printf("Cycles = %llu\n", cycle_cnt);
    printf("CPI = %f\n", (float)cycle_cnt/(float)inst_cnt);

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

void Multicycle_Sim::init_RegMem()
{
    memset(regfile, 0, sizeof(regfile));
    memset(memory, 0, sizeof(memory));
}

void Multicycle_Sim::load_memory(char* filename)
{
    FILE* file = fopen(filename, "r");

    fseek(file, elf_reader.cadr, 0);
    fread(&memory[elf_reader.cvadr], 1, elf_reader.csize, file);
    fseek(file, elf_reader.dadr, 0);
    fread(&memory[elf_reader.dvadr], 1, elf_reader.dsize, file);

    fclose(file);
}

void Multicycle_Sim::update_regfile()
{
    for (int i = 0; i < 32; i++)
        regfile[i].update();
}

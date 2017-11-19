#include "func_sim.h"
#include "reg_def.h"

Func_Sim::Func_Sim():
    elf_reader(), inst_fetcher()
{
    err_no = NOTHING;
    memset(memory, 0, sizeof(memory));
    memset(regfile, 0, sizeof(regfile));
    memset(fregfile, 0, sizeof(fregfile));
}

bool Func_Sim::readelf(char *filename, char *elfname, char **vals, int n_val)
{
    return elf_reader.read_elf(filename, elfname, vals, n_val);
}

void Func_Sim::init_RegMem()
{
    memset(fregfile, 0, sizeof(fregfile));
    memset(regfile, 0, sizeof(regfile));
    memset(memory, 0, sizeof(memory));
}

bool Func_Sim::init(char* filename, char *elfname, char **vals, int n_val)
{
    if (!readelf(filename, elfname, vals, n_val))
        return false;
    init_RegMem();
    load_memory(filename);
    inst_fetcher.set_next_pc((REG)elf_reader.madr);
    inst_fetcher.set_pc((REG)elf_reader.madr);
    regfile[3] = (REG)elf_reader.gp;
    regfile[2] = (REG)FS_SP_DEFAULT;
    return true;
}

void Func_Sim::load_memory(char* filename)
{
    FILE* file = fopen(filename, "r");

    fseek(file, elf_reader.cadr, 0);
    fread(&memory[elf_reader.cvadr], 1, elf_reader.csize, file);
    fseek(file, elf_reader.dadr, 0);
    fread(&memory[elf_reader.dvadr], 1, elf_reader.dsize, file);

    fclose(file);
}

void Func_Sim::one_step(bool verbose)
{
    unsigned long long pc = inst_fetcher.get_pc();
    if (pc < (unsigned long long)elf_reader.cvadr || pc > (unsigned long long)(elf_reader.cvadr + elf_reader.csize))
        err_no = INVALID_PC;
    if (pc % 4 != 0)
        err_no = INVALID_PC;
    if (pc == elf_reader.mend - 4)
        err_no = HALT;
    if (err_no != NOTHING)
        return;

    regfile[0] = 0;
    inst_fetcher.set_inst32(memory);
    inst_fetcher.split32();
    if (verbose)
        inst_fetcher.print_assembly();
    err_no = inst_fetcher.just_sim(regfile, fregfile, memory);

    if (err_no != NOTHING)
        return;
    regfile[0] = 0;
}

void Func_Sim::sim(bool verbose)
{
    while(true)
    {
        one_step(verbose);
        if (err_no != NOTHING)
            return;
    }
}

ERROR_NUM Func_Sim::get_errno()
{
    return err_no;
}

REG Func_Sim::read_regfile(int idx)
{
    if (idx < 0 || idx > 31)
        return 0xDEADBEEF;
    return regfile[idx];
}

char Func_Sim::read_memory(long long addr)
{
    if (addr < 0 || addr > FS_MAX_MEMORY)
        return 0xFA;
    return memory[addr];
}

void Func_Sim::print_res()
{
    printf("\nFunction-sim Mode Done!\n");
    if (err_no == INVALID_PC)
    {
        printf("Invalid PC: 0x%016llx\n", inst_fetcher.get_pc());
        return;
    }
    if (err_no == INVALID_INST)
    {
        printf("Invalid inst: [%llx] %08x\n", inst_fetcher.get_pc(), inst_fetcher.get_inst32());
        return;
    }
    if (err_no == INVALID_ADDR_ALIGNMENT)
    {
        printf("Invalid address alignment: [%llx] %08x\n", inst_fetcher.get_pc(), inst_fetcher.get_inst32());
        return;
    }
    if (err_no == HALT)
        printf("Machine halt!\n");
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

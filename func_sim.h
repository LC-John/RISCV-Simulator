#ifndef FUNC_SIM_H
#define FUNC_SIM_H

#include "elf_reader.h"
#include "reg_def.h"
#include "if.h"
#include <cstdio>
#include <iostream>

#define FS_MAX_MEMORY (4 * 1024 * 1024)
#define FS_SP_DEFAULT (FS_MAX_MEMORY - 10000)

class Func_Sim
{
public:
    Func_Sim();
    bool readelf(char* filename, char* elfname, char** vals, int n_val);
    bool init(char* filename, char *elfname, char **vals, int n_val);
    void one_step(bool verbose = false);
    void sim(bool verbose = false);
    ERROR_NUM get_errno();
    REG read_regfile(int idx);
    char read_memory(long long addr);
    void print_res();

private:
    char* interested[20];
    int n_interested;
    ELF_Reader elf_reader;
    IF inst_fetcher;
    ERROR_NUM err_no;
    char memory[FS_MAX_MEMORY];
    REG regfile[32];
    FREG fregfile[32];

    void init_RegMem();
    void load_memory(char* filename);
};

#endif // FUNC_SIM_H

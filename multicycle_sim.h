#ifndef MULTICYCLE_SIM_H
#define MULTICYCLE_SIM_H

#include "elf_reader.h"
#include "reg_def.h"
#include "if.h"
#include "id.h"
#include "ex.h"
#include "mem.h"
#include "wb.h"
#include <cstdio>
#include <iostream>

#define FS_MAX_MEMORY (4 * 1024 * 1024)
#define FS_SP_DEFAULT (FS_MAX_MEMORY - 10000)

class Multicycle_Sim
{
public:
    Multicycle_Sim();
    bool readelf(char* filename, char* elfname, char** vals, int n_val);
    bool init(char* filename, char *elfname, char **vals, int n_val);
    void one_step(unsigned int verbose = 1);
    void sim(unsigned int verbose = 1);
    ERROR_NUM get_errno();
    REG read_regfile(int idx);
    char read_memory(long long addr);
    void print_res();

private:
    unsigned long long inst_cnt;
    unsigned long long cycle_cnt;
    unsigned long long err_loc;
    char* interested[20];
    int n_interested;
    ELF_Reader elf_reader;
    IF inst_fetcher;
    ID inst_identifier;
    EX inst_executor;
    MEM inst_memory;
    WB inst_writor;
    char type;
    ERROR_NUM err_no;
    char memory[FS_MAX_MEMORY];
    TEMPORAL_REG regfile[32];
    //FREG fregfile[32];

    void init_RegMem();
    void load_memory(char* filename);
    void update_regfile();

    void IFID_PROCEDURE(unsigned int verbose);
    void EX_PROCEDURE(unsigned int verbose);
    void EXWB_PROCEDURE(unsigned int verbose);
    void EXMEM_PROCEDURE(unsigned int verbose);
    void EXMEMWB_PROCEDURE(unsigned int verbose);
};

#endif // MULTICYCLE_SIM_H

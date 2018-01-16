#ifndef PIPELINE_SIM_H
#define PIPELINE_SIM_H

#include "elf_reader.h"
#include "reg_def.h"
#include "if.h"
#include "id.h"
#include "ex.h"
#include "ex2.h"
#include "mem.h"
#include "wb.h"
#include "memory.h"
#include "cache.h"
#include <cstdio>
#include <iostream>

#define USE_CACHE

#define FS_MAX_MEMORY (4 * 1024 * 1024)
#define FS_SP_DEFAULT (FS_MAX_MEMORY - 10000)

#define STAGE_IF 0
#define STAGE_ID 1
#define STAGE_EX 2
#define STAGE_EX2 3
#define STAGE_MEM 4
#define STAGE_WB 5

class Pipeline_Sim
{
public:
    Pipeline_Sim();
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
    unsigned long long control_hazard_cnt, b_control_cnt, jalr_control_cnt;
    unsigned long long wrong_branch_cnt;
    unsigned long long load_use_hazard_cnt;
    char* interested[20];
    int n_interested;
    ELF_Reader elf_reader;
    IF inst_fetcher;
    ID inst_identifier;
    EX inst_executor;
    EX2 inst_multiplier;
    MEM inst_memory;
    WB inst_writor;
    char type;
    ERROR_NUM err_no;
    char memory[FS_MAX_MEMORY];
    TEMPORAL_REG regfile[32];

    bool sigNope[6], sigStall[6];
    bool ex2_active;
    bool out_nope;
    int divrem_timer;

    void init_RegMem();
    void load_memory(char* filename);
    void update_regfile();

    void update_latch();
    void update_component(int verbose);
    void wrong_branch(int verbose);
    void control_hazard(int verbose);
    bool check_load_use_hazard();
    void load_use_hazard(int verbose);
    bool check_multiply_conflict();
    void multiply_conflict(int verbose);
    void divrem(int verbose);
    void check_status();
    void nope_transfer();

    unsigned long long sext64(unsigned long long num, int sbit);
    bool RegWEn(INST32_CTRL_BIT ctrl);

#ifdef USE_CACHE
    Memory m;
    Cache l1, l2, llc;
    int wait_cache_time;

    void wait_cache(int verbose);
#endif
};

#endif // PIPELINE_SIM_H

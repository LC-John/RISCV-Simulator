#ifndef REG_DEF_H
#define REG_DEF_H

#include <string.h>

typedef unsigned long long REG;
typedef union __FREG
{
    double fp64;
    float fp32;
} FREG;

typedef union __FLOAT2UINT
{
    float fp32;
    unsigned int uint;
} FLOAT2UINT;

typedef union __DOUBLE2ULL
{
    double fp64;
    unsigned long long ull;
} DOUBLE2ULL;

typedef struct __TEMPORAL_REG
{
    REG in;
    REG out;
    void update()
    {
        out = in;
        in = out;
    }
} TEMPORAL_REG;

typedef union __INST
{
    unsigned short inst16;
    unsigned int inst32;
} INST;

typedef union __IMM_SET
{
    unsigned int imm_S;
    unsigned int imm_SB;
    unsigned int imm_U;
    unsigned int imm_UJ;
    unsigned int imm_I;
} IMM_SET;

typedef struct __INST32_CTRL_BIT
{
    unsigned inst32_2_6;
    unsigned inst32_12_14;
    unsigned inst32_25_31;
    bool BrEq;
    bool BrLT;
} INST32_CTRL_BIT;

typedef enum __IMMSEL_CTRL
{
    R_IMM,
    I_IMM,
    S_IMM,
    SB_IMM,
    U_IMM,
    UJ_IMM,
    UNKNOWN
} IMMSEL_CTRL;

typedef enum __BRUN_CTRL
{
    BR_UNSIGNED,
    BR_SIGNED,
    BR_DONT_CARE,
    BR_UNKNOWN
} BRUN_CTRL;

typedef enum __BSEL_CTRL
{
    BSEL_RS2,
    BSEL_IMM,
    BSEL_UNKNOWN
} BSEL_CTRL;

typedef enum __ASEL_CTRL
{
    ASEL_RS1,
    ASEL_PC,
    ASEL_UNKNOWN
} ASEL_CTRL;

typedef enum __ALUSEL_CTRL
{
    ADD64, SUB64, SLT64, SLT64U,
    XOR64, OR64,  AND64, SLL64,
    SRL64, SRA64, MUL64, MUL64H,
    MUL64HSU, MUL64HU, DIV64, DIV64U,
    REM64, REM64U,
    ADD32, SUB32, SLL32, SRL32,
    SRA32, MUL32, DIV32, DIV32U,
    REM32, REM32U,
    PASSB, ALU_UNKNOWN, ADD64ALIGN
} ALUSEL_CTRL;

typedef enum __MEMRW_CTRL
{
    MEM_READ_B,
    MEM_READ_H,
    MEM_READ_W,
    MEM_READ_D,
    MEM_READ_BU,
    MEM_READ_HU,
    MEM_READ_WU,
    MEM_WRITE_B,
    MEM_WRITE_H,
    MEM_WRITE_W,
    MEM_WRITE_D,
    MEM_DONT_CARE,
    MEM_UNKNOWN
} MEMRW_CTRL;

typedef enum __WBSEL_CTRL
{
    WB_ALU,
    WB_MEM,
    WB_PC_PLUS_4,
    WB_DONT_CARE,
    WB_UNKNOWN
} WBSEL_CTRL;

typedef enum __REGWEN_CTRL
{
    REGW_Y,
    REGW_N,
    REGW_UNKNOWN
} REGWEN_CTRL;

typedef enum __PCSEL_CTRL
{
    PC_PLUS_4,
    PC_ALU,
    PC_UNKNOWN
} PCSEL_CTRL;

typedef enum __STAGE_STATUS
{
    STAGE_NORMAL,
    STAGE_STALL,
    STAGE_NOPE
} STAGE_STATUS;

typedef enum __ERROR_NO
{
    NOTHING,
    INVALID_INST,
    INVALID_PC,
    INVALID_ADDR_ALIGNMENT,
    UNAUTHORIZED_ACCESS,
    HALT
} ERROR_NUM;

#endif // REG_DEF_H

#ifndef ELF_READER_H
#define ELF_READER_H

#include <stdio.h>
#include <string.h>

typedef struct{
    unsigned char b[8];
    operator unsigned long long()
    {
        //printf("ulonglong: %02x%02x%02x%02x%02x%02x%02x%02x %016llx\n", b[0], b[1], b[2], b[3], b[4], b[5], b[6], b[7], *((unsigned long long *)b));
        return *((unsigned long long *)b);
    }
}int64;

typedef struct{
    unsigned char b[4];
    operator unsigned int()
    {
        //printf("uint: %02x%02x%02x%02x %08x\n", b[0], b[1], b[2], b[3], *((unsigned int *)b));
        return *((unsigned int *)b);
    }
}int32;

typedef struct{
    unsigned char b[2];
    operator unsigned short()
    {
        //printf("ushort: %02x%02x %04x\n", b[0], b[1], *((unsigned short *)b));
        return *((unsigned short *)b);
    }
}int16;

typedef struct{
    unsigned char b;
}int8;

typedef int64 Elf64_Addr;
typedef int64 Elf64_Off;
typedef int64 Elf64_Xword;
typedef int64 Elf64_Sxword;
typedef int32 Elf64_Word;
typedef int32 Elf64_Sword;
typedef int16 Elf64_Half;

#define ET_NONE		0		//No file type
#define ET_REL		1 		//Relocatable file
#define	ET_EXEC		2		//Executable file
#define	ET_DYN		3		//Shared object file
#define ET_CORE		4		//Core file
#define ET_LOPROC	0xff00		//Processor-specific
#define	ET_HIPROC	0xffff		//Processor-specofic

#define	EI_CLASS	4
#define	EI_DATA		5
#define	EI_VERSION 	6
#define	EI_OSABI 	7
#define	EI_ABIVERSION 	8
#define	EI_PAD 		9
#define	EI_NIDENT 	16

#define	SHN_UNDEF 	0
#define	SHN_LOPROC 	0xFF00
#define	SHN_HIPROC 	0xFF1F
#define	SHN_LOOS 	0xFF20
#define	SHN_HIOS 	0xFF3F
#define	SHN_ABS 	0xFFF1
#define	SHN_COMMON 	0xFFF2

#define SHT_NULL		0
#define SHT_PROGBITS		1
#define SHT_SYMTAB		2
#define SHT_STRTAB		3
#define SHT_RELA		4
#define SHT_HASH		5
#define SHT_DYNAMIC		6
#define SHT_NOTE		7
#define SHT_NOBITS 		8
#define SHT_REL 		9
#define SHT_SHLIB		10
#define SHT_DYNSYM		11
#define SHT_LOPROC		0x70000000
#define SHT_HIPROC		0x7fffffff
#define SHT_LOUSER		0x80000000
#define SHT_HIUSER		0xffffffff

#define SHF_WRITE		0x1
#define SHF_ALLOC		0x2
#define SHF_EXECINSTR		0x4
#define SHF_RELA_LIVEPATCH	0x00100000
#define SHF_RO_AFTER_INIT	0x00200000
#define SHF_MASKPROC		0xf0000000

#define PT_NULL		0
#define PT_LOAD		1
#define PT_DYNAMIC	2
#define PT_INTERP	3
#define PT_NOTE		4
#define PT_SHLIB	5
#define PT_PHDR		6
#define PT_TLS    	7
#define PT_NUM    	8
#define PT_LOOS   	0x60000000
#define PT_HIOS   	0x6fffffff
#define PT_LOPROC 	0x70000000
#define PT_HIPROC	0x7fffffff

#define STB_LOCAL	0
#define STB_GLOBAL	1
#define STB_WEAK	2
#define STB_LOPROC	13
#define STB_HIPROC	15

#define STT_NOTYPE	0
#define STT_OBJECT	1
#define STT_FUNC	2
#define STT_SECTION	3
#define STT_FILE	4
#define STT_LOPROC	13
#define STT_HIPROC	15

typedef struct
{
    unsigned char e_ident[EI_NIDENT]; /* ELF identification */
    Elf64_Half e_type; /* Object file type */
    Elf64_Half e_machine; /* Machine type */
    Elf64_Word e_version; /* Object file version */
    Elf64_Addr e_entry; /* Entry point address */
    Elf64_Off e_phoff; /* Program header offset */
    Elf64_Off e_shoff; /* Section header offset */
    Elf64_Word e_flags; /* Processor-specific flags */
    Elf64_Half e_ehsize; /* ELF header size */
    Elf64_Half e_phentsize; /* Size of program header entry */
    Elf64_Half e_phnum; /* Number of program header entries */
    Elf64_Half e_shentsize; /* Size of section header entry */
    Elf64_Half e_shnum; /* Number of section header entries */
    Elf64_Half e_shstrndx; /* Section name string table index */
} Elf64_Ehdr;

typedef struct
{
    Elf64_Word sh_name; /* Section name */
    Elf64_Word sh_type; /* Section type */
    Elf64_Xword sh_flags; /* Section attributes */
    Elf64_Addr sh_addr; /* Virtual address in memory */
    Elf64_Off sh_offset; /* Offset in file */
    Elf64_Xword sh_size; /* Size of section */
    Elf64_Word sh_link; /* Link to other section */
    Elf64_Word sh_info; /* Miscellaneous information */
    Elf64_Xword sh_addralign; /* Address alignment boundary */
    Elf64_Xword sh_entsize; /* Size of entries, if section has table */
} Elf64_Shdr;

typedef struct
{
    Elf64_Word st_name; /* Symbol name */
    unsigned char st_info; /* Type and Binding attributes */
    unsigned char st_other; /* Reserved */
    Elf64_Half st_shndx; /* Section table index */
    Elf64_Addr st_value; /* Symbol value */
    Elf64_Xword st_size; /* Size of object (e.g., common) */
} Elf64_Sym;


typedef struct
{
    Elf64_Word p_type; /* Type of segment */
    Elf64_Word p_flags; /* Segment attributes */
    Elf64_Off p_offset; /* Offset in file */
    Elf64_Addr p_vaddr; /* Virtual address in memory */
    Elf64_Addr p_paddr; /* Reserved */
    Elf64_Xword p_filesz; /* Size of segment in file */
    Elf64_Xword p_memsz; /* Size of segment in memory */
    Elf64_Xword p_align; /* Alignment of segment */
} Elf64_Phdr;

class ELF_Reader
{
public:
    unsigned long long cadr;
    unsigned long long csize;
    unsigned long long cvadr;
    unsigned long long gp;
    unsigned long long dadr;
    unsigned long long dsize;
    unsigned long long dvadr;
    unsigned long long madr;
    unsigned long long mend;
    unsigned long long endPC;
    unsigned long long entry;
    char val_name[20][100];
    unsigned long long val_adr[20];
    unsigned long long val_size[20];
    unsigned long long val_n;

    ELF_Reader();
    bool read_elf(const char *filename, const char *elfname,
        char **interested, int n_interested);
    void print_all();

private:
    FILE *file;
    FILE *elf;
    Elf64_Ehdr elf64_hdr;
    unsigned int padr;
    unsigned int psize;
    unsigned int pnum;
    unsigned int sadr;
    unsigned int ssize;
    unsigned int snum;
    unsigned int symnum;
    unsigned int symadr;
    unsigned int symsize;
    char *name_table;
    char *symname_table;
    char** intere;
    int n_intere;

    bool open_file(const char *filename, const char *elfname);
    void read_Elf_header();
    void read_elf_sections();
    void read_symtable();
    void read_Phdr();
};

#endif // ELF_READER_H

#include "elf_reader.h"
#include <stdlib.h>

ELF_Reader::ELF_Reader()
{
    cadr = 0; csize = 0; cvadr = 0;
    dadr = 0; dsize = 0; dvadr = 0;
    gp = 0;
    madr = 0; mend = 0;
    endPC = 0; entry = 0;
    val_n = 0;

    padr = 0; psize = 0; pnum = 0;
    sadr = 0; ssize = 0; snum = 0;
    symadr = 0; symsize = 0; symnum = 0;
    name_table = NULL; symname_table = NULL;
    intere = NULL; n_intere = 0;
    file = NULL; elf = NULL;
}

bool ELF_Reader::open_file(const char *filename, const char *elfname)
{
    file = fopen(filename, "r");
    elf = fopen(elfname, "w+");
    if (file != NULL && elf != NULL)
        return true;
    return false;
}

bool ELF_Reader::read_elf(const char *filename, const char *elfname,
    char **interested, int n_interested)
{
    if(!open_file(filename, elfname))
        return false;

    intere = interested;
    n_intere = n_interested;

    fprintf(elf,"ELF Header:\n");
    read_Elf_header();

    fprintf(elf,"\n\nSection Headers:\n");
    read_elf_sections();

    fprintf(elf,"\n\nProgram Headers:\n");
    read_Phdr();

    fprintf(elf,"\n\nSymbol table:\n");
    read_symtable();

    fclose(elf);
    elf = NULL;
    fclose(file);
    file = NULL;

    return true;
}

void ELF_Reader::read_Elf_header()
{
    //file should be relocated
    fseek(file, 0, 0);
    fread(&elf64_hdr, 1, sizeof(elf64_hdr), file);

    fprintf(elf," magic number:\t");
    for (int i = 0; i < EI_NIDENT; i++)
        fprintf(elf, "%02x ", elf64_hdr.e_ident[i]);
    fprintf(elf, "\n");

    switch(elf64_hdr.e_ident[EI_CLASS])
    {
    case 0:
        fprintf(elf," Class:\t\tELFCLASSNONE\n");
        break;
    case 1:
        fprintf(elf," Class:\t\tELFCLASS32\n");
        break;
    case 2:
        fprintf(elf," Class:\t\tELFCLASS64\n");
        break;
    default:
        fprintf(elf," Class:\t\tUNKNOWN\n");
        break;
    }

    switch(elf64_hdr.e_ident[EI_DATA])
    {
    case 0:
        fprintf(elf," Data:\t\tinvalid data encoding\n");
        break;
    case 1:
        fprintf(elf," Data:\t\tlittle-endian\n");
        break;
    case 2:
        fprintf(elf," Data:\t\tbig-endian\n");
        break;
    default:
        fprintf(elf," Data:\t\tUNKNOWN\n");
        break;
    }

    fprintf(elf," Version:\t%x\n", elf64_hdr.e_ident[EI_VERSION]);

    switch (elf64_hdr.e_ident[EI_OSABI])
    {
    case 0x00:
        fprintf(elf," OS/ABI:\tUNIX Syetem V\n");
        break;
    case 0x03:
        fprintf(elf," OS/ABI:\tLINUX\n");
        break;
    case 0x06:
        fprintf(elf," OS/ABI:\tSolaris\n");
        break;
    case 0x09:
        fprintf(elf," OS/ABI:\tFreeBSD\n");
        break;
    default:
        fprintf(elf," OS/ABI:\tUnknown\n");
        break;
    }

    fprintf(elf," ABI Version:\t%x\n", elf64_hdr.e_ident[EI_ABIVERSION]);

    switch ((unsigned short)elf64_hdr.e_type)
    {
    case ET_NONE:
        fprintf(elf," Type:\t\tInvalid\n");
        break;
    case ET_REL:
        fprintf(elf," Type:\t\tRelocatable\n");
        break;
    case ET_EXEC:
        fprintf(elf," Type:\t\tExecutable\n");
        break;
    case ET_DYN:
        fprintf(elf," Type:\t\tShared-obj\n");
        break;
    case ET_CORE:
        fprintf(elf," Type:\t\tCore\n");
        break;
    case ET_LOPROC:
    case ET_HIPROC:
        fprintf(elf," Type:\t\tProcessor-specific\n");
        break;
    default:
        fprintf(elf," Type:\t\tUnknown\n");
        break;
    }
    fprintf(elf," Type:\t\t%hu\n", (unsigned short)elf64_hdr.e_type);

    fprintf(elf," Machine:\t%hu\n", (unsigned short)elf64_hdr.e_machine);

    fprintf(elf," Version:\t%hu\n", (unsigned int)elf64_hdr.e_version);

    fprintf(elf," Entry point address:\t\t0x%llx\n", (unsigned long long)elf64_hdr.e_entry);
    entry = (unsigned long long)elf64_hdr.e_entry;

    fprintf(elf," Start of program headers:\t%llu (bytes into  file)\n", (unsigned long long)elf64_hdr.e_phoff);
    padr = (unsigned long long)elf64_hdr.e_phoff;

    fprintf(elf," Start of section headers:\t%llu (bytes into  file)\n", (unsigned long long)elf64_hdr.e_shoff);
    sadr = (unsigned long long)elf64_hdr.e_shoff;

    fprintf(elf," Flags:\t\t\t\t0x%x\n", (unsigned int)elf64_hdr.e_flags);

    fprintf(elf," Size of this header:\t\t%hu Bytes\n", (unsigned short)elf64_hdr.e_ehsize);

    fprintf(elf," Size of program headers:\t%hu Bytes\n", (unsigned short)elf64_hdr.e_phentsize);
    psize = (unsigned short)elf64_hdr.e_phentsize;

    fprintf(elf," Number of program headers:\t%hu\n", (unsigned short)elf64_hdr.e_phnum);
    pnum = (unsigned short)elf64_hdr.e_phnum;

    fprintf(elf," Size of section headers:\t%hu Bytes\n", (unsigned short)elf64_hdr.e_shentsize);
    ssize = (unsigned short)elf64_hdr.e_shentsize;

    fprintf(elf," Number of section headers:\t%hu\n", (unsigned short)elf64_hdr.e_shnum);
    snum = (unsigned short)elf64_hdr.e_shnum;

    fprintf(elf," Section header string table index:\t%hu\n", (unsigned short)elf64_hdr.e_shstrndx);
}

void ELF_Reader::read_elf_sections()
{

    Elf64_Shdr elf64_shdr, name_shdr;

    fseek(file, 0, 0);
    fread(&elf64_hdr, 1, sizeof(elf64_hdr), file);
    fseek(file, sadr+elf64_hdr.e_shstrndx*sizeof(Elf64_Shdr), 0);
    fread(&name_shdr,1,sizeof(Elf64_Shdr),file);
    name_table = new char[(unsigned long long)name_shdr.sh_size + 10];
    fseek(file, name_shdr.sh_offset, 0);
    fread(name_table, 1, (unsigned long long)name_shdr.sh_size, file);
    fseek(file, sadr, 0);

    for(unsigned int c=0;c<snum;c++)
    {
        fprintf(elf," [%3d]\n",c);

        //file should be relocated
        fread(&elf64_shdr,1,sizeof(elf64_shdr),file);

        fprintf(elf,"  Name: %-13s", name_table+(unsigned int)elf64_shdr.sh_name);

        switch ((unsigned int)elf64_shdr.sh_type)
        {
        case SHT_NULL:
            fprintf(elf,"  Type: %-8s", "NULL");
            break;
        case SHT_PROGBITS:
            fprintf(elf,"  Type: %-8s", "PROGBITS");
            break;
        case SHT_SYMTAB:
            fprintf(elf,"  Type: %-8s", "SYMTAB");
            break;
        case SHT_STRTAB:
            fprintf(elf,"  Type: %-8s", "STRTAB");
            break;
        case SHT_RELA:
            fprintf(elf,"  Type: %-8s", "RELA");
            break;
        case SHT_HASH:
            fprintf(elf,"  Type: %-8s", "HASH");
            break;
        case SHT_DYNAMIC:
            fprintf(elf,"  Type: %-8s", "DYNAMIC");
            break;
        case SHT_NOTE:
            fprintf(elf,"  Type: %-8s", "NOTE");
            break;
        case SHT_NOBITS:
            fprintf(elf,"  Type: %-8s", "NOBITS");
            break;
        case SHT_REL:
            fprintf(elf,"  Type: %-8s", "REL");
            break;
        case SHT_SHLIB:
            fprintf(elf,"  Type: %-8s", "SHLIB");
            break;
        case SHT_DYNSYM:
            fprintf(elf,"  Type: %-8s", "DYNSYM");
            break;
        case SHT_LOPROC:
            fprintf(elf,"  Type: %-8s", "LOPROC");
            break;
        case SHT_HIPROC:
            fprintf(elf,"  Type: %-8s", "HIPROC");
            break;
        case SHT_LOUSER:
            fprintf(elf,"  Type: %-8s", "LOUSER");
            break;
        case SHT_HIUSER:
            fprintf(elf,"  Type: %-8s", "HIUSER");
            break;
        default:
            fprintf(elf,"  Type: %-8s", "UNKNOWN");
            break;
        }

        fprintf(elf,"  Address: %016llx", (unsigned long long)elf64_shdr.sh_addr);

        fprintf(elf,"  Offest: %016llx\n", (unsigned long long)elf64_shdr.sh_offset);

        fprintf(elf,"  Size: %016llx", (unsigned long long)elf64_shdr.sh_size);

        fprintf(elf,"  Entsize: %016llx", (unsigned long long)elf64_shdr.sh_entsize);

        switch ((unsigned long long)elf64_shdr.sh_flags)
        {
        case SHF_WRITE:
            fprintf(elf,"  Flags: %-4s", "W");
            break;
        case SHF_ALLOC:
            fprintf(elf,"  Flags: %-4s", "A");
            break;
        case SHF_EXECINSTR:
            fprintf(elf,"  Flags: %-4s", "X");
            break;
        case SHF_MASKPROC:
            fprintf(elf,"  Flags: %-4s", "M");
            break;
        case SHF_WRITE | SHF_ALLOC:
            fprintf(elf,"  Flags: %-4s", "WA");
            break;
        case SHF_WRITE | SHF_EXECINSTR:
            fprintf(elf,"  Flags: %-4s", "WX");
            break;
        case SHF_ALLOC | SHF_EXECINSTR:
            fprintf(elf,"  Flags: %-4s", "AX");
            break;
        case SHF_WRITE | SHF_ALLOC | SHF_EXECINSTR:
            fprintf(elf,"  Flags: %-4s", "WAX");
            break;
        default:
            fprintf(elf,"  Flags: %-4s", "U");
            break;
        }

        fprintf(elf,"  Link: %u", (unsigned int)elf64_shdr.sh_link);

        fprintf(elf,"  Info: %u", (unsigned int)elf64_shdr.sh_info);

        fprintf(elf,"  Align: %llu\n", (unsigned long long)elf64_shdr.sh_addralign);

        if ((unsigned int)elf64_shdr.sh_type == SHT_SYMTAB)
        {
            symadr = (unsigned long long)elf64_shdr.sh_offset;
            symsize = (unsigned long long)elf64_shdr.sh_size;
            symnum = symsize / sizeof(Elf64_Sym);
        }
    }

    delete []name_table;
}

void ELF_Reader::read_Phdr()
{
    Elf64_Phdr elf64_phdr;

    fseek(file, 0, 0);
    fread(&elf64_hdr, 1, sizeof(elf64_hdr), file);
    fseek(file, padr, 0);

    for(unsigned int c=0;c<pnum;c++)
    {
        fprintf(elf," [%3d]\n",c);

        //file should be relocated
        fread(&elf64_phdr,1,sizeof(elf64_phdr),file);

        switch((unsigned int)elf64_phdr.p_type)
        {
        case PT_NULL:
            fprintf(elf," Type: %-9s", "Invalid");
            break;
        case PT_LOAD:
            fprintf(elf," Type: %-9s", "LOAD");
            break;
        case PT_DYNAMIC:
            fprintf(elf," Type: %-9s", "DYNAMIC");
            break;
        case PT_INTERP:
            fprintf(elf," Type: %-9s", "INTERP");
            break;
        case PT_NOTE:
            fprintf(elf," Type: %-9s", "NOTE");
            break;
        case PT_SHLIB:
            fprintf(elf," Type: %-9s", "SHLIB");
            break;
        case PT_PHDR:
            fprintf(elf," Type: %-9s", "PHDR");
            break;
        case PT_TLS:
            fprintf(elf," Type: %-9s", "TLS");
            break;
        case PT_NUM:
            fprintf(elf," Type: %-9s", "NUM");
            break;
        case PT_LOOS:
            fprintf(elf," Type: %-9s", "LOOS");
            break;
        default:
            fprintf(elf," Type: %-9s", "Unknown");
            break;
        }

        fprintf(elf," Offset: 0x%016llx", (unsigned long long)elf64_phdr.p_offset);

        fprintf(elf," VirtAddr: 0x%016llx", (unsigned long long)elf64_phdr.p_vaddr);

        fprintf(elf," PhysAddr: 0x%016llx\n", (unsigned long long)elf64_phdr.p_paddr);

        fprintf(elf," FileSiz: 0x%016llx", (unsigned long long)elf64_phdr.p_filesz);

        fprintf(elf," MemSiz: 0x%016llx", (unsigned long long)elf64_phdr.p_memsz);

        switch ((unsigned int)elf64_phdr.p_flags)
        {
        case 0x01:
            fprintf(elf," Flags: %-4s", "X");
            break;
        case 0x02:
            fprintf(elf," Flags: %-4s", "W");
            break;
        case 0x03:
            fprintf(elf," Flags: %-4s", "WX");
            break;
        case 0x04:
            fprintf(elf," Flags: %-4s", "R");
            break;
        case 0x05:
            fprintf(elf," Flags: %-4s", "RX");
            break;
        case 0x06:
            fprintf(elf," Flags: %-4s", "RW");
            break;
        case 0x07:
            fprintf(elf," Flags: %-4s", "RWX");
            break;
        default:
            fprintf(elf," Flags: %-4s", "U");
            break;
        }

        fprintf(elf," Align: 0x%llx\n", (unsigned long long)elf64_phdr.p_align);

        if ((unsigned int)elf64_phdr.p_flags == 0x05 && (unsigned int)elf64_phdr.p_type == PT_LOAD)
        {
            cadr = (unsigned long long)elf64_phdr.p_offset;
            csize = (unsigned long long)elf64_phdr.p_filesz;
            cvadr = (unsigned long long)elf64_phdr.p_vaddr;
        }
        if ((unsigned int)elf64_phdr.p_flags == 0x06 && (unsigned int)elf64_phdr.p_type == PT_LOAD)
        {
            dadr = (unsigned long long)elf64_phdr.p_offset;
            dsize = (unsigned long long)elf64_phdr.p_filesz;
            dvadr = (unsigned long long)elf64_phdr.p_vaddr;
        }
    }
}

void ELF_Reader::read_symtable()
{
    Elf64_Sym elf64_sym;
    Elf64_Shdr elf64_shdr, name_shdr;

    fseek(file, 0, 0);
    fread(&elf64_hdr, 1, sizeof(elf64_hdr), file);
    fseek(file, sadr+elf64_hdr.e_shstrndx*sizeof(Elf64_Shdr), 0);
    fread(&name_shdr,1,sizeof(Elf64_Shdr),file);
    name_table = new char[(unsigned long long)name_shdr.sh_size + 10];
    fseek(file, (unsigned long long)name_shdr.sh_offset, 0);
    fread(name_table, 1, (unsigned long long)name_shdr.sh_size, file);
    fseek(file, sadr, 0);
    for(unsigned int c=0;c<snum;c++)
    {
        fread(&elf64_shdr,1,sizeof(elf64_shdr),file);
        if (strcmp(name_table+(unsigned int)elf64_shdr.sh_name, ".strtab") == 0)
        {
            symname_table = new char[(unsigned long long)elf64_shdr.sh_size + 10];
            fseek(file, (unsigned long long)elf64_shdr.sh_offset, 0);
            fread(symname_table, 1, (unsigned long long)elf64_shdr.sh_size, file);
            break;
        }
    }
    delete []name_table;

    fseek(file, symadr, 0);

    for(unsigned int c=0;c<symnum;c++)
    {
        fprintf(elf," [%3d]   ",c);

        //file should be relocated
        fread(&elf64_sym,1,sizeof(elf64_sym),file);

        fprintf(elf,"  Name: %-30s", symname_table+(unsigned int)elf64_sym.st_name);

        switch(elf64_sym.st_info & 0xf)
        {
        case STT_NOTYPE:
            fprintf(elf,"  Type: %-9s", "NOTYPE");
            break;
        case STT_OBJECT:
            fprintf(elf,"  Type: %-9s", "OBJECT");
            break;
        case STT_FUNC:
            fprintf(elf,"  Type: %-9s", "FUNC");
            break;
        case STT_SECTION:
            fprintf(elf,"  Type: %-9s", "SECTION");
            break;
        case STT_FILE:
            fprintf(elf,"  Type: %-9s", "FILE");
            break;
        case STT_LOPROC:
            fprintf(elf,"  Type: %-9s", "LOPROC");
            break;
        case STT_HIPROC:
            fprintf(elf,"  Type: %-9s", "HIPROC");
            break;
        default:
            fprintf(elf,"  Type: %-9s", "Unknown");
            break;
        }

        switch(elf64_sym.st_info >> 4)
        {
        case STB_LOCAL:
            fprintf(elf,"  Bind: %-9s", "LOCAL");
            break;
        case STB_GLOBAL:
            fprintf(elf,"  Bind: %-9s", "GLOBAL");
            break;
        case STB_WEAK:
            fprintf(elf,"  Bind: %-9s", "WEAK");
            break;
        case STB_LOPROC:
            fprintf(elf,"  Bind: %-9s", "LOPROC");
            break;
        case STB_HIPROC:
            fprintf(elf,"  Bind: %-9s", "HIPROC");
            break;
        default:
            fprintf(elf,"  Bind: %-9s", "Unknown");
            break;
        }

        if ((unsigned short)elf64_sym.st_shndx == 0xfff1)
            fprintf(elf,"  NDX: %-4s", "ABS");
        else
            fprintf(elf,"  NDX: %-4hu", (unsigned short)elf64_sym.st_shndx);

        fprintf(elf,"  Size: %-16llu", (unsigned long long)elf64_sym.st_size);

        fprintf(elf,"  Value: %016llx\n", (unsigned long long)elf64_sym.st_value);

        if (strcmp(symname_table+(unsigned int)elf64_sym.st_name, "main") == 0)
        {
            madr = (unsigned long long)elf64_sym.st_value;
            mend = madr + (unsigned long long)elf64_sym.st_size + 1;
        }
        if (strcmp(symname_table+(unsigned int)elf64_sym.st_name, "__global_pointer$") == 0)
        {
            gp = (unsigned long long)elf64_sym.st_value;
        }
        if (strcmp(symname_table+(unsigned int)elf64_sym.st_name, "atexit") == 0)
        {
            endPC = (unsigned long long)elf64_sym.st_value;
        }
        for (int i = 0; i < n_intere; i++)
        {
            if (strcmp(symname_table+(unsigned int)elf64_sym.st_name,
                intere[i]) == 0)
            {
                val_adr[val_n] = (unsigned long long)elf64_sym.st_value;
                val_size[val_n] = (unsigned long long)elf64_sym.st_size;
                strcpy(val_name[val_n], symname_table+(unsigned int)elf64_sym.st_name);
                val_n++;
            }
        }
    }
}

void ELF_Reader::print_all()
{
    printf("code: 0x%llx 0x%llx 0x%llx\n", cadr, csize, cvadr);
    printf("data: 0x%llx 0x%llx 0x%llx\n", dadr, dsize, dvadr);
    printf("main: 0x%llx 0x%llx\n", madr, mend);
    printf("PC:   0x%llx 0x%llx\n", entry, endPC);
    printf("val: %lld\n", val_n);
    for (unsigned int i = 0; i < val_n; i++)
        printf("\t%s 0x%llx 0x%llx\n", val_name[i], val_adr[i], val_size[i]);
}

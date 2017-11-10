#ifndef LISENCE_H
#define LISENCE_H

#define LC_RISCV_SIMULATOR
#define THIS_IS_A_RISCV_SIMULATOR
#define IF_YOU_HAVE_ANY_TROUBLE
#define PLEASE_CONTECT_ME_THROUGH_THE_MEANS_BELOW

void help_print(char* arg_name)
{
    printf("\n");
    printf("\n");
    printf("This is a risc-v simulator.\n");
    printf("  If you are facing any trouble, please contect me.\n");
    printf("  My name: Zhang Huangzhao\n");
    printf("  PKU ID:  1400017707\n");
    printf("  Email:   zhang_hz@pku.edu.cn\n");
    printf("\n");
    printf("Arguments:\n");
    printf("  -v: set verbose mode (print some messages)\n");
    printf("  -d: set debug mode (print much more messages)\n");
    printf("  variable: add a global variable to the val list. When the simulator halts, print the memory values of each variable in the val list.\n");
    printf("  riscv-executable-file: always goes first!");
    printf("\n");
    printf("Command: %s [riscv-executable-file] (-v) (-d) variable1 variable2 ...\n", arg_name);
    printf("  eg. %s add -v result\n", arg_name);
    printf("\n");
    printf("\n");
}

#endif // LISENCE_H

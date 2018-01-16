#include <cstdio>
#include "memory.h"
#include "def.h"

Memory::~Memory()
{
    //printf("Memory destructor.\n");
}

void Memory::HandleRequest(unsigned long long addr, int bytes,
                           int read, char *content,
                           int &hit, int &time)
{
#ifdef DEBUG
    printf("  Mem always hit!\n");
#endif

    hit = 1;
    time = latency_.hit_latency + latency_.bus_latency;
    stats_.access_time += time;
    stats_.access_counter += 1;

    if (read == 1)
    {
        for (int i = 0; i < bytes; i++)
            content[i] = (char)0x0;
    }
}


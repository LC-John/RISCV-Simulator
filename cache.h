#ifndef CACHE_CACHE_H_
#define CACHE_CACHE_H_

#include "storage.h"

typedef struct CacheConfig_ {
    int size;
    int associativity;
    int set_num; // Number of cache sets
    int write_through; // 0|1 for back|through
    int write_allocate; // 0|1 for no-alc|alc
} CacheConfig;

class Cache: public Storage {
public:
    Cache() {}
    ~Cache();

    // Sets & Gets
    void SetConfig(CacheConfig cc);
    void GetConfig(CacheConfig &cc) { cc = config_; }
    void SetLower(Storage *ll) { lower_ = ll; }
    // Main access process
    void HandleRequest(unsigned long long addr, int arg_bytes,
                       int read, char *content,
                       int &hit, int &time);
    void print();

private:
    // Bypassing
    int BypassDecision();
    // Partitioning
    void PartitionAlgorithm();
    // Replacement
    int ReplaceDecision();
    void ReplaceAlgorithm();
    // Prefetching
    int PrefetchDecision();
    void PrefetchAlgorithm();

    int tag_bits, idx_bits, offset_bits;
    int set_num, line_num, line_size;
    bool **valid, **dirty;
    unsigned long long **tag;
    int **counter;
    char ***bytes;

    unsigned long long get_tag(unsigned long long addr);
    unsigned long long get_idx(unsigned long long addr);
    unsigned long long get_offset(unsigned long long addr);

    int find_line(unsigned long long arg_tag, int set, int &line);
    void read_hit(unsigned long long addr, int arg_set, int arg_line, int arg_offset, int arg_bytes, char *content, int &time);
    void read_miss(unsigned long long addr, int arg_set, int arg_offset, int arg_bytes, char *content, int &time);
    void write_hit(unsigned long long addr, int arg_set, int arg_line, int arg_offset, int arg_bytes, char *content, int &time);
    void write_miss(unsigned long long addr, int arg_set, int arg_offset, int arg_bytes, char *content, int &time);

    CacheConfig config_;
    Storage *lower_;
    DISALLOW_COPY_AND_ASSIGN(Cache);
};

#endif //CACHE_CACHE_H_ 

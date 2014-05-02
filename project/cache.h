#ifndef CACHE_H
#define CACHE_H
#include<stdlib.h>
#include<math.h>
#include<stdio.h>

struct Block {
  int valid;
  int dirty;
  unsigned long long tag;
  struct Block *lru;
};

struct Cache {
  int cachesize, blocksize, ways;
  int setlength, indexsize, bytesize;
  unsigned long long hits, misses, wrefs, rrefs;
  unsigned long long dko, ko;

  struct Block **lruarray;
  struct Block **bloarr;
};

struct Cache *icache;
struct Cache *dcache;
struct Cache *l2cache;

struct Cache * initcache(int cachesize, int blocksize, int ways);
int readd(struct Cache* cache,unsigned long long  t, unsigned long long index, char op);
void printcache(struct Cache *cache);
void printstuff(struct Cache *icache, struct Cache* dcache, struct Cache* l2cache);

#endif

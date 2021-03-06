#ifndef CACHE_H
#define CACHE_H
#include<stdlib.h>
#include<math.h>
#include<stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

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
  unsigned long long itime, rtime, wtime;
  int hittime, misstime;
  int cost;

  struct Block **lruarray;
  struct Block **bloarr;
};

struct Cache *icache;
struct Cache *dcache;
struct Cache *l2cache;

struct Cache * initcache(int cachesize, int blocksize, int ways, int hittime, int misstime);
unsigned long long readd(struct Cache* cache,unsigned long long  t, unsigned long long index, char op);
void printcache(struct Cache *cache);
void printstuff(struct Cache *icache, struct Cache* dcache, struct Cache* l2cache, int mready, int mchunks, int mchunkt, unsigned long long refnum, unsigned long long misallignment, int memcost);
int calcost(int level, int size, int associativity);

#endif

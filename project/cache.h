#ifndef CACHE_H
#define CACHE_H
#include<stdlib.h>
#include<math.h>
#include<stdio.h>
#include<time.h>

struct Block {
  int valid;
  int dirty;
  struct Block *next;
  struct Block *nextset;
  unsigned int *tag;
};
struct Cache {
  int hits;
  int misses;
  int reads;
  int writes;

  int cachesize;
  int blocksize;
  int ways;

  int hittime;
  int misstime;

  int assoc;

  int numblocks;
  int indexsize;
  int bytesize;
  int tagsize;

  int irefs;
  int drefs;

  int itime;
  int rtime;
  int wtime;

  int cost;

  struct Block *block;
};

struct Cache *icache;
struct Cache *dcache;
struct Cache *l2cache;

struct Cache * initcache(int cachesize, int blocksize, int hittime, int misstime, int assoc);
int reead(struct Cache *, unsigned int tag, unsigned int index, unsigned int byte, int size, int level, unsigned long addr, char type);
void freecache();

#endif

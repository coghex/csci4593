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
  unsigned long tag;
};

struct Cache {
  int hits;
  int misses;
  int reads;
  int writes;

  int cachesize;
  int blocksize;
  int bussize;

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
int printcache(struct Cache *cache);
int moveblock(struct Block *src, struct Block *dest, struct Cache *cache, int index);
int reead(struct Cache *, unsigned long addr, int size, int verbose);
void freecache();

#endif

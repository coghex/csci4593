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
  struct Block *prevset;
  unsigned long tag;
};

struct Cache {
  int hits;
  int misses;
  int reads;
  int writes;
  int dirtykickouts;
  int kickouts;

  int cachesize;
  int blocksize;
  int buswidth;

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

  int data;
  struct Block *block;
};

struct Cache *icache;
struct Cache *dcache;
struct Cache *l2cache;

struct Cache * initcache(int cachesize, int blocksize, int hittime, int misstime, int assoc, int data, int buswidth);
int printcache(struct Cache *cache);
int moveblock(struct Block *src, struct Block *dest, struct Cache *cache, int index);
int reead(struct Cache *, unsigned long long addr, int size, int verbose, int level, char type, int s);
void freecache();

#endif

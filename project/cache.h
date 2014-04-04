#ifndef CACHE_H
#define CACHE_H
#include<stdlib.h>
#include<math.h>
#include<stdio.h>

struct Block {
  int valid;
  int dirty;
  struct Block *next;
  unsigned long *tag;
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

  struct Block *block;
};

struct Cache * initcache(int cachesize, int blocksize, int hittime, int misstime, int assoc, int ways);
int reead(struct Cache *, unsigned int tag, unsigned int index, unsigned int byte, int size);

#endif

#include "cache.h"

struct Cache * initcache(int cachesize, int blocksize, int hittime, int misstime, int assoc, int ways) {
  struct Cache *endcache;
  struct Block *block;
  int i,j;
  printf("hello\n");

  endcache = (struct Cache *)malloc(sizeof(struct Cache));

  endcache->hits=0;
  endcache->misses=0;
  endcache->reads=0;
  endcache->writes=0;
  endcache->cachesize=cachesize;
  endcache->blocksize=blocksize;
  endcache->ways=1;
  endcache->hittime=hittime;
  endcache->misstime=misstime;
  endcache->assoc=assoc;
  endcache->numblocks=cachesize/blocksize;
  endcache->indexsize=log(endcache->numblocks)/log(2);
  endcache->bytesize=log(endcache->blocksize/8)/log(2);
  endcache->tagsize=38-endcache->bytesize-endcache->indexsize;
  endcache->block=(struct Block*)malloc(sizeof(struct Block));
  block=endcache->block;
  block->tag=malloc((endcache->blocksize/8)*sizeof(unsigned int));
  for (i=1;i<endcache->numblocks;i++){
    block->next=(struct Block*)malloc(sizeof(struct Block));
    block->tag=malloc((endcache->blocksize/8)*sizeof(unsigned int));
    block=block->next;
  }

  return endcache;
}

int reead(struct Cache * cache, unsigned int tag, unsigned int index, unsigned int byte, int size) {
  int i, j, ref, tempbyte, t;
  unsigned int readtag;
  struct Block* block;
  if ((size+byte)%(cache->blocksize/8)) {
    ref = (int)(size+byte)/(cache->blocksize/8)+1;
  }
  else {
    ref = (int)(size+byte)/(cache->blocksize/8);
  }
  block = cache->block;
  for (i=0;i<(int)index;i++) {
    block = block->next;
  }
  tempbyte=byte;
  for (j=0;j<ref;j++){
    for (i=tempbyte;i<cache->blocksize/8;i++) {
      if (block->tag[i] == tag) {
        cache->hits++;
        i=cache->blocksize/8;
      }
      else {
        cache->misses++;
        for (i=tempbyte;i<cache->blocksize/8;i++){
          block->tag[i] = tag;
        }
      }
    }
    tempbyte=0;
    block=block->next;
  }

  return 1;
}

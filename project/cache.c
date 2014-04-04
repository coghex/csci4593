#include "cache.h"

struct Cache * initcache(int cachesize, int blocksize, int hittime, int misstime, int assoc, int ways) {
  struct Cache *endcache;
  struct Block *block;
  int i,j;
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
  for (i=1;i<=endcache->numblocks;i++){
    block->next=(struct Block*)malloc(sizeof(struct Block));
    block=block->next;
    block->tag=(unsigned int *)malloc((endcache->blocksize/8)*sizeof(unsigned int));
  }

  return endcache;
}

int reead(struct Cache * cache, unsigned int tag, unsigned int index, unsigned int byte, int size, int level, unsigned long addr) {
  cache->reads++;
  int i, j, ref, tempbyte, t;
  unsigned int l2tag, l2index, l2byte;
  unsigned int readtag;
  struct Block* block;
  if ((size+byte)%(cache->blocksize/8)) {
    ref = (int)(size+byte)/(cache->blocksize/8)+1;
  }
  else {
    ref = (int)(size+byte)/(cache->blocksize/8);
  }

  block = cache->block;
  for (i=0;i<((int)index%cache->numblocks);i++) {
    block = block->next;
  }
  tempbyte=byte;
  //printf("  level: %d numblocks: %d tag: %X index: %X byte: %X\n", level, cache->numblocks, tag, index, byte);
  for (j=0;j<ref;j++){
    for (i=tempbyte;i<cache->blocksize/8;i++) {
      if (block->tag[i] == tag) {
        cache->hits++;
        i=cache->blocksize/8;
      }
      else {
        cache->misses++;
        if (level==1){
          l2tag = (((~0)<<(l2cache->indexsize+l2cache->bytesize))&addr)>>(l2cache->indexsize+l2cache->bytesize);
          l2index = (((~((~0)<<l2cache->indexsize))<<l2cache->bytesize)&addr)>>(l2cache->bytesize);
          l2byte = (~((~0)<<l2cache->bytesize))&addr;

          reead(l2cache, l2tag, l2index, l2byte, size, 2, addr);
          for (i=tempbyte;i<cache->blocksize/8;i++){
            block->tag[i] = tag;
          }
        }
        else if (level==2) {
          for (i=tempbyte;i<cache->blocksize/8;i++){
            block->tag[i] = tag;
          }
        }
      }
    }
    tempbyte=0;
    block=block->next;
  }

  return 1;
}

void freecache() {
  int i;
  struct Block *temp1, *temp2;

  temp1=icache->block;
  for (i=0;i<=icache->numblocks;i++) {
    free(temp1->tag);
    temp2=temp1->next;
    free(temp1);
    temp1=temp2;
  }
  free(icache);
  temp1=dcache->block;
  for (i=0;i<=dcache->numblocks;i++) {
    free(temp1->tag);
    temp2=temp1->next;
    free(temp1);
    temp1=temp2;
  }
  free(dcache);
  temp1=l2cache->block;
  for (i=0;i<=l2cache->numblocks;i++) {
    free(temp1->tag);
    temp2=temp1->next;
    free(temp1);
    temp1=temp2;
  }
  free(l2cache);
}

#include "cache.h"

struct Cache * initcache(int cachesize, int blocksize, int hittime, int misstime, int assoc) {
  struct Cache *endcache;
  struct Block *block;
  struct Block *temp;
  int i,j,k;
  endcache = (struct Cache *)malloc(sizeof(struct Cache));

  endcache->hits=0;
  endcache->misses=0;
  endcache->reads=0;
  endcache->writes=0;
  endcache->irefs=0;
  endcache->drefs=0;
  endcache->itime=0;
  endcache->rtime=0;
  endcache->wtime=0;
  endcache->cachesize=cachesize;
  endcache->blocksize=(blocksize);
  endcache->hittime=hittime;
  endcache->misstime=misstime;
  endcache->assoc=assoc;
  endcache->numblocks=(cachesize/blocksize)/assoc;
  endcache->indexsize=log(endcache->numblocks)/log(2.0);
  endcache->bytesize=log((endcache->blocksize))/log(2.0);
  endcache->tagsize=38-endcache->bytesize-endcache->indexsize;
  endcache->block=(struct Block*)malloc(sizeof(struct Block));
  block=endcache->block;
  block->tag=malloc((endcache->blocksize)*sizeof(unsigned long));
  for (i=1;i<=(endcache->numblocks)*(assoc);i++){
    block->next=(struct Block*)malloc(sizeof(struct Block));
    block=block->next;
    block->tag=(unsigned long *)malloc((endcache->blocksize)*sizeof(unsigned long));
  }

  block = endcache->block;
  for (j=0; j<assoc; j++) {
    for (i=0; i<endcache->numblocks;i++) {
      if (j==assoc-1) {
        block->nextset=NULL;
      }
      else {
        temp = block;
        for (k=i;k<endcache->numblocks;k++) {
          temp=temp->next;
        }
        //printf("setting set %d, block %d's next set's block to %d.  numblocks: %d\n", j, i, k, endcache->numblocks);
        block->nextset=temp;
      }
    }
  }


  return endcache;
}

int reead(struct Cache * cache, unsigned long tag, unsigned long index, unsigned long byte, int size, int level, unsigned long addr, char type, int verbose) {
  cache->reads++;
  int i, j, ref, tempbyte, k;
  unsigned long l2tag, l2index, l2byte;
  struct Block *block, *temp;
  time_t begin, end;
  begin = clock();
  if ((int)(size+(byte%4))%4){
    ref = (int)((size+(byte%4))/4);
  }
  else {
    ref=(int)((size+(byte%4))/4)-1;
  }
  if (type=='I'&&level==1) {
    cache->irefs++;
  }

  block = cache->block;
  for (i=0;i<((int)index%cache->numblocks);i++) {
    block = block->next;
  }
  tempbyte=byte;
  for (j=0;j<=ref;j++){
    for (i=tempbyte;i<cache->blocksize;i++) {
      temp=block;
      for (k=0;k<cache->assoc;k++) {
        if (block->tag[i]==tag) {
          cache->hits++;
          i=cache->blocksize;
          k=cache->assoc;
          if (verbose) {
            if (level==1){
              printf("L1 HIT\n");
            }
            else {
              printf("L2 HIT\n");
            }
          }
        }
        else {
          if (block->nextset==NULL) {
            cache->misses++;
            if (level==1){
              if (verbose) {
                printf("L1 MISS\n");
              }
              l2tag = (((~0)<<(l2cache->indexsize+l2cache->bytesize))&addr)>>(l2cache->indexsize+l2cache->bytesize);
              l2index = (((~((~0)<<l2cache->indexsize))<<l2cache->bytesize)&addr)>>(l2cache->bytesize);
              l2byte = (~((~0)<<l2cache->bytesize))&addr;

              reead(l2cache, l2tag, l2index, l2byte, size, 2, addr, type, verbose);
              for (i=0;i<cache->blocksize;i++){
                temp->tag[i] = tag;
              }
            }
            else if (level==2) {
              if (verbose) {
                printf("L2 MISS\n");
              }
              for (i=0;i<cache->blocksize;i++){
                temp->tag[i] = tag;
              }
            }
            k=cache->assoc;
          }
          else {
            block = block->nextset;
          }
        }
        if ((int)(size+byte)>cache->blocksize) {
          tempbyte=0;
          block=block->next;
        }
      }
    }
  }
  end = clock();
  if (type=='I'){
    cache->itime+=(end-begin);
  }
  else if (type=='R'){
    cache->rtime+=(end-begin);
  }
  else if (type=='W'){
    cache->wtime+=(end-begin);
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

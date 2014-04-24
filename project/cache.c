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
  block->tag=0;
  for (i=1;i<=(endcache->numblocks)*(assoc);i++){
    block->next=(struct Block*)malloc(sizeof(struct Block));
    block=block->next;
    block->tag=0;
  }

  block = endcache->block;
  for (j=0; j<assoc; j++) {
    for (i=0; i<endcache->numblocks;i++) {
      endcache->LRUqueue[j*endcache->numblocks+i]=block;
      if (j==assoc-1) {
        block->nextset=NULL;
      }
      else {
        temp = block;
        for (k=i;k<endcache->numblocks;k++) {
          temp=temp->next;
        }
        block->nextset=temp;
        block=block->next;
      }
    }
  }
  return endcache;
}

int reead(struct Cache * cache, unsigned long tag, unsigned long index, unsigned long byte, int size, int level, unsigned long addr, char type, int verbose) {
  cache->reads++;
  int i, j, k, ref;
  struct Block *block, *temp, *node;
  verbose = (int) addr;

  if ((int)(size+(byte%4))%4){
    ref = (int)((size+(byte%4))/4)+1;
  }
  else {
    ref=(int)((size+(byte%4))/4);
  }
  if (type=='I'&&level==1) {
    cache->irefs++;
  }

  block = cache->block;
  if (cache->numblocks>1) {
    for (i=0;i<((int)index%cache->numblocks);i++) {
      block = block->next;
    }
  }

  temp=block;
  for (i=0;i<ref;i++) {
    if (((int)((4*i*(size/4))+byte)>cache->blocksize)) {
      if (temp->nextset==NULL) {
        temp=temp->next;
        index++;
      }
      else {
        tag++;
      }
    }

    for (j=0;j<cache->assoc;j++) {
      if (temp->tag==tag) {
        cache->hits++;
        printf("L1 HIT\n");
        break;
      }
      else {
        if (temp->nextset!=NULL) {
          temp=temp->nextset;
          if (temp->valid==0) {
            cache->misses++;
            printf("L1 MISS\n");
            temp->tag=tag;
            temp->valid=1;
            break;
          }
        }
        else {
          cache->misses++;
          printf("L1 MISS\n");

          temp->tag=tag;
          temp->valid=1;
        }
      }
    }
  }


  return 1;
}

void freecache() {
  int i;
  struct Block *temp1, *temp2;

  temp1=icache->block;
  for (i=0;i<=icache->numblocks;i++) {
    temp2=temp1->next;
    free(temp1);
    temp1=temp2;
  }
  free(icache);
  temp1=dcache->block;
  for (i=0;i<=dcache->numblocks;i++) {
    temp2=temp1->next;
    free(temp1);
    temp1=temp2;
  }
  free(dcache);
  temp1=l2cache->block;
  for (i=0;i<=l2cache->numblocks;i++) {
    temp2=temp1->next;
    free(temp1);
    temp1=temp2;
  }
  free(l2cache);
}

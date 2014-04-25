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
      if (j==assoc-1) {
        block->nextset=NULL;
      }
      else {
        temp = block;
        for (k=0;k<endcache->numblocks;k++) {
          temp=temp->next;
        }
        block->nextset=temp;
        block=block->next;
      }
    }
  }
  return endcache;
}

int printcache(struct Cache *cache) {
  int i,j;
  struct Block *block, *temp;
  block = cache->block;
  temp=block;
  for (i=0;i<cache->numblocks;i++) {
    block=temp;
    for (j=0;j<cache->assoc;j++) {
      printf("-----------");
    }
    printf("\n");
    for (j=0;j<cache->assoc;j++) {
      if (block->tag==0) {
        printf("|    x    |");
      }
      else {
        printf("| %lX |",block->tag);
      }
      if (block->nextset!=NULL) {
        block=block->nextset;
      }
      else {
        break;
      }
    }
    printf("\n");
    for (j=0;j<cache->assoc;j++) {
      printf("-----------");
    }
    printf("\n");
    temp=temp->next;
  }
  return 1;
}

int moveblock(struct Block *src, struct Block *dest, struct Cache *cache, int index) {
  struct Block *tempdest;
  int i;

  if (index==0&&cache->numblocks==1) {
    src->nextset=dest;
    for (i=0;i<cache->assoc-2;i++) {
      dest=dest->nextset;
    }
    dest->nextset=NULL;
    cache->block=src;
  }
  else if (index==0) {
    src->nextset=dest;
    src->next=dest->next;
    for (i=0;i<cache->assoc-1;i++) {
      if (i==cache->assoc-2) {
        dest->nextset=NULL;
        dest->next=dest->next->nextset;
      }
      else {
        dest->next=dest->nextset->next;
        dest=dest->nextset;
      }
    }
    cache->block=src;
  }
  else {
    tempdest=cache->block;
    for (i=0;i<index-1;i++) {
      if (index!=1) {
        tempdest=tempdest->next;
      }
    }
    src->nextset=dest;
    src->next=dest->next;
    tempdest->next=src;

    for (i=0;i<cache->assoc-1;i++) {
      tempdest=tempdest->nextset;
      tempdest->next=dest;
      if (i==cache->assoc-2) {
        dest->nextset=NULL;
        dest->next=dest->next->nextset;
      }
      else {
        dest->next=dest->nextset->next;
        dest=dest->nextset;
      }
    }
  }

  return 1;
}

int reead(struct Cache * cache, unsigned long addr, int size, int verbose) {
  int i,j;
  unsigned long tag;
  int index;
  struct Block *block, *temp, *head;

  for (i=0;i<size;i++) {
    //Calculate tag and index
    tag = (((~0)<<(cache->indexsize+cache->bytesize))&addr)>>(cache->indexsize+cache->bytesize);
    index = (((~((~0)<<cache->indexsize))<<cache->bytesize)&addr)>>(cache->bytesize);

    //Find the right index
    head=cache->block;
    block=head;
    for (j=0;j<index;j++) {
      if (index!=0) {
        block=block->next;
      }
    }

    printf("index = 0x%x, tag = 0x%lx  ", index, tag);
    //Check for tag in each set
    temp=block;
    for (j=0;j<cache->assoc;j++) {
      if (temp->tag==tag && temp->valid==1) {
        cache->hits++;
        if (verbose) {
          printf("HIT\n");
        }
        break;
      }
      else {
        if (temp->nextset!=NULL) {
          temp=temp->nextset;
        }
        else {
          cache->misses++;
          if (verbose) {
            printf("MISS\n");
          }
          //Write to the right block
          temp->tag=tag;
          temp->valid=1;
          //move block to beginning of LRU queue
          if (cache->assoc>1){
            moveblock(temp, block, cache, index);
          }
          break;
        }
      }
    }
  }
  //printcache(cache);
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

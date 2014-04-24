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

void switchinset(struct Block *src, struct Block *dest, struct Block *head, int assoc, int index) {
  int i;

  if (index==0) {
    src->nextset=dest;
    if (dest->next!=NULL) {
      src->next=dest->next;
    }
    for (i=0;i<assoc-1;i++) {
      if (dest->nextset->next!=NULL) {
        dest->next=dest->nextset->next;
      }
      dest=dest->nextset;
      if (dest->nextset==src) {
          dest->nextset=NULL;
          return;
      }
    }
  }
  else {
    if (src!=dest) {
      src->nextset=dest;
      src->next=dest->next;

      for (i=0;i<((int)index-1);i++) {
        head = head->next;
      }
      head->next=src;
      for (i=0;i<assoc-1;i++) {
        dest->next=dest->nextset->next;
        head=head->nextset;
        head->next=dest;
        dest=dest->nextset;
        if (dest->nextset==src) {
          dest->nextset=NULL;
          return;
        }
      }
    }
  }
}

int reead(struct Cache * cache, unsigned long tag, unsigned long index, unsigned long byte, int size, int level, unsigned long addr, char type, int verbose) {
  cache->reads++;
  int i, j, ref, wierdcondition=0;
  struct Block *block, *temp;
  verbose = (int) addr;

  if ((int)(size+(byte%4))%4){
    ref = (int)((size+(byte%4))/4)+1;
  }
  else {
    ref=(int)((size+(byte%4))/4);
  }
  printf("This will take %d references\n",ref);
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
    if (size<4) {
      wierdcondition=1;
    }
    if (((int)((4*i*((wierdcondition+size/4)))+byte)>=cache->blocksize) && wierdcondition < 2) {
      printf("Mah spoon is too big\n");
      if (temp->nextset==NULL) {
        temp=temp->next;
        block=block->next;
        index++;
       }
      else {
        tag++;
        temp=block;
      }
      wierdcondition=2;
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
        }
        else {
          cache->misses++;
          printf("L1 MISS\n");

          temp->tag=tag;
          temp->valid=1;

          switchinset(temp, block, cache->block, cache->assoc, (int)index%cache->numblocks);
          if (block==cache->block) {
            cache->block=temp;
          }
          break;
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

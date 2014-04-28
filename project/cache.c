#include "cache.h"

struct Cache * initcache(int cachesize, int blocksize, int hittime, int misstime, int assoc, int data, int buswidth, int sendaddr, int ready, int chunktime) {
  struct Cache *endcache;
  struct Block *block;
  struct Block *temp;
  int i,j,k;
  endcache = (struct Cache *)malloc(sizeof(struct Cache));

  endcache->hits=0;
  endcache->data=data;
  endcache->misses=0;
  endcache->reads=0;
  endcache->writes=0;
  endcache->irefs=0;
  endcache->drefs=0;
  endcache->itime=0;
  endcache->rtime=0;
  endcache->wtime=0;
  endcache->buswidth=buswidth;
  endcache->sendaddr=sendaddr;
  endcache->ready=ready;
  endcache->chunktime=chunktime;
  endcache->cachesize=cachesize;
  endcache->blocksize=(blocksize);
  endcache->hittime=hittime;
  endcache->misstime=misstime;
  endcache->assoc=assoc;
  endcache->numblocks=(cachesize/blocksize)/assoc;
  endcache->indexsize=log(endcache->numblocks)/log(2.0);
  endcache->bytesize=log((endcache->blocksize))/log(2.0);
  endcache->tagsize=data*9+38-endcache->bytesize-endcache->indexsize;
  endcache->block=(struct Block*)malloc(sizeof(struct Block));
  block=endcache->block;
  block->tag=0;
  for (i=1;i<(endcache->numblocks)*(assoc);i++){
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
        temp->prevset=block;
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

  printf("\n");
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
  struct Block *tempdest, *temp;
  int i;

  if (src!=dest) {
    if (index==0&&cache->numblocks==1) {
      if (src->nextset!=NULL) {
        src->prevset->nextset=src->nextset;
        src->nextset->prevset=src->prevset;
      }
      else {
        src->prevset->nextset=NULL;
      }
      src->nextset=dest;
      src->prevset=NULL;
      dest->prevset=src;
      cache->block=src;
    }
    else if (index==0) {
      //tempdest=cache->block;
      //for (i=0;i<cache->numblocks;i++) {
      //  tempdest=tempdest->next;
      //}
      //tempdest->next=src->prevset;
      temp=src->prevset;
      if (src->nextset!=NULL) {
        src->prevset->nextset=src->nextset;
        src->nextset->prevset=src->prevset;
      }
      else {
        src->prevset->nextset=NULL;
      }
      src->nextset=dest;
      dest->prevset=src;
      src->next=dest->next;
      src->prevset=NULL;

      while (temp->prevset!=NULL) {
        temp->next=temp->next->nextset;
        temp=temp->prevset;
      }
      cache->block=src;
    }
    else if (index==cache->numblocks-1) {
      tempdest=cache->block;
      for (i=0;i<index-1;i++) {
        if (index!=1) {
          tempdest=tempdest->next;
        }
      }
      temp=src->prevset;
      if (src->nextset!=NULL) {
        src->prevset->nextset=src->nextset;
        src->nextset->prevset=src->prevset;
      }
      else {
        src->prevset->nextset=NULL;
      }
      src->nextset=dest;
      dest->prevset=src;
      src->next=dest->next;
      src->prevset=NULL;

      for (i=0;i<cache->assoc;i++) {
        if (tempdest->next==src) {
          tempdest->next=tempdest->prevset->next->nextset;
          break;
        }
        tempdest->next=tempdest->next->prevset;
        tempdest=tempdest->nextset;
      }
    }
    else {
      tempdest=cache->block;
      for (i=0;i<index-1;i++) {
        if (index!=1) {
          tempdest=tempdest->next;
        }
      }
      temp=src->prevset;
      if (src->nextset!=NULL) {
        src->prevset->nextset=src->nextset;
        src->nextset->prevset=src->prevset;
      }
      else {
        src->prevset->nextset=NULL;
      }
      src->nextset=dest;
      dest->prevset=src;
      src->next=dest->next;
      src->prevset=NULL;

      for (i=0;i<cache->assoc;i++) {
        if (tempdest->next==src) {
          tempdest->next=tempdest->prevset->next->nextset;
          break;
        }
        tempdest->next=tempdest->next->prevset;
        tempdest=tempdest->nextset;
      }

      while (temp->prevset!=NULL) {
        temp->next=temp->next->nextset;
        temp=temp->prevset;
      }
    }
  }
  return 1;
}

int reead(struct Cache * cache, unsigned long long addr, int size, int verbose, int level, char type, int s) {
  int i,j;
  unsigned long long tag, l2addr;
  int index, l2byte, length;
  struct Block *block, *temp, *head;

  for (i=0;i<size;i++) {
    //Calculate tag and index
    tag = (((~0)<<(cache->indexsize+cache->bytesize))&addr)>>(cache->indexsize+cache->bytesize);
    index = (((~((~0)<<cache->indexsize))<<cache->bytesize)&addr)>>(cache->bytesize);

    //Find the right index
    head=cache->block;
    block=head;
    for (j=0;j<(index);j++) {
      if (index!=0) {
        block=block->next;
      }
    }

    if (verbose) {
      printf("index = 0x%x, tag = 0x%llx  ", index, tag);
    }
    //Check for tag in each set
    temp=block;
    for (j=0;j<cache->assoc;j++) {
      if (temp->tag==tag && temp->valid==1) {
        cache->hits++;
        if (type=='I') {
          cache->itime+=cache->hittime;
        }
        else if (type=='R') {
          cache->rtime+=cache->hittime;
        }
        else {
          cache->wtime+=cache->hittime;
        }
        if (verbose) {
          if (level==1) {
            printf("L1 HIT\n");
          }
          else {
            printf("L2 HIT\n");
          }
        }
        if (level==1 && type=='I' && verbose) {
          printf("Add L1i hit time (+ %d)\n", cache->misstime);
        }
        else if (level==1 && verbose) {
          printf("Add L1d hit time (+ %d)\n", cache->misstime);
        }
        else {
          if (verbose) {
            printf("Add L2 hit time (+ %d)\n", cache->misstime);
          }
        }

        if (cache->assoc>1){
          moveblock(temp, block, cache, index);
        }
        if (type=='W') {
          temp->dirty=1;
        }
        break;
      }
      else {
        if (temp->nextset!=NULL) {
          temp=temp->nextset;
        }
        else {
          cache->misses++;
          if (type=='I') {
            if (level==2) {
              cache->itime+=cache->sendaddr+cache->ready+(cache->chunktime*(cache->blocksize/cache->buswidth))+cache->hittime;
              cache->itime+=cache->misstime;
            }
          }
          else if (type=='R') {
            if (level==2) {
              cache->rtime+=cache->sendaddr+cache->ready+(cache->chunktime*(cache->blocksize/cache->buswidth))+cache->hittime;
              cache->rtime+=cache->misstime;
            }
          }
          else {
            if (level==2) {
              cache->wtime+=cache->sendaddr+cache->ready+(cache->chunktime*(cache->blocksize/cache->buswidth))+cache->hittime;
              cache->wtime+=cache->misstime;
            }
          }

          if (verbose) {
            if (level==1) {
              printf("L1 MISS\n");
            }
            else {
              printf("L2 MISS\n");
            }
          }

          if (level==1 && type=='I' && verbose) {
            printf("Add L1i miss time (+ %d)\n", cache->misstime);
            cache->itime+=cache->misstime;
          }
          else if (level==1 && type=='R' && verbose) {
            printf("Add L1d miss time (+ %d)\n", cache->misstime);
            cache->rtime+=cache->misstime;
          }
          else if (level==1 && verbose) {
            printf("Add L1d miss time (+ %d)\n", cache->misstime);
            cache->wtime+=cache->misstime;
          }
          else {
            if (verbose) {
              printf("Add L2 miss time (+ %d)\n", cache->misstime);
              printf("Add memory to L2 transfer time (+ %d)\n", cache->sendaddr+cache->ready+(cache->chunktime*(cache->blocksize/cache->buswidth)));
              printf("Add L2 hit replay time (+ %d)\n", cache->hittime);
            }
          }
          if (level==2 && verbose && type=='I') {
            printf("Bringing line into L1i.\n");
            printf("Add L2 to L1 transfer time (+ %d)\n", icache->sendaddr*(32/l2cache->buswidth));
            cache->itime+=icache->sendaddr*(32/l2cache->buswidth);
            printf("Add L1i hit replay time (+ %d)\n", icache->hittime);
            cache->itime+=icache->hittime;

          }
          else if (level==2 && verbose) {
            printf("Bringing line into L1d.");
            printf("Add L2 to L1 transfer time (+ %d)\n", dcache->sendaddr*(32/l2cache->buswidth));
            printf("Add L1d hit replay time (+ %d)\n", dcache->hittime);

            if (type=='R') {
              cache->rtime+=dcache->sendaddr*(32/l2cache->buswidth);
              cache->rtime+=dcache->hittime;
            }
            else {
              cache->wtime+=dcache->sendaddr*(32/l2cache->buswidth);
              cache->wtime+=dcache->hittime;
            }
          }

          if (temp->dirty) {
            cache->dirtykickouts++;
            temp->dirty=0;
          }
          if (temp->valid) {
            cache->kickouts++;
          }

          //Write to the right block
          temp->tag=tag;
          temp->valid=1;
          if (type=='W') {
            temp->dirty=1;
          }
          if (type=='R') {
            temp->dirty=0;
          }


          //Going to L2
          if (level==1) {
            l2byte = (~((~0)<<l2cache->bytesize))&addr;
            l2addr = addr;
            if ((s+(l2byte%l2cache->buswidth))%l2cache->buswidth) {
              length = ((s+(l2byte%l2cache->buswidth))/l2cache->buswidth)+1;
            }
            else {
              length = ((s+(l2byte%l2cache->buswidth))/l2cache->buswidth);
            }

            if (type=='W') {
              l2cache->writes++;
            }
            else {
              l2cache->reads++;
            }

            if ((int)(l2byte+s)>l2cache->blocksize) {
              reead(l2cache, l2addr, ((l2cache->blocksize)-(l2byte))/l2cache->buswidth+1, verbose, 2, type, s);
              l2addr = l2addr+(1<<(l2cache->bytesize));
              length=length-((l2cache->blocksize)-(l2byte))/8-1;
              reead(l2cache, l2addr, length, verbose, 2, type, s);
            }
            else {
              reead(l2cache, l2addr, length, verbose, 2, type, s);
            }

          }
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

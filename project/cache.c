#include "cache.h"


void printcache(struct Cache *cache) {
  int i, j;
  for (j=0;j<cache->setlength;j++) {
    printf("----------------------------------------------------\n");
    for (i=0;i<cache->ways;i++) {
      if (cache->bloarr[j][i].tag) {
        printf("|  %llX  |", cache->bloarr[j][i].tag);
      }
      else {
        printf("|           |");
      }
    }
    printf("\n");
  }
  printf("-----------------------------------------------------\n");
}

struct Cache * initcache(int cachesize, int blocksize, int ways) {
  struct Cache *cache;
  struct Block *temp;
  int i, j;
  cache=(struct Cache*)malloc(sizeof(struct Cache));

  cache->cachesize=cachesize;
  cache->blocksize=blocksize/8;
  cache->ways=ways;
  cache->setlength=(cachesize/blocksize)/ways;
  cache->indexsize=log(cache->setlength)/log(2);
  cache->bytesize=log(8*cache->blocksize)/log(2);
  cache->hits=0;
  cache->misses=0;

  cache->bloarr=(struct Block **)malloc((cache->setlength*sizeof(struct Block *)));
  for (i=0;i<cache->setlength;i++) {
    cache->bloarr[i]=(struct Block *)malloc(ways*sizeof(struct Block));
  }

  cache->lruarray=(struct Block **)malloc(cache->setlength*sizeof(struct Block *));
  for (i=0;i<cache->setlength;i++) {
    cache->lruarray[i]=&cache->bloarr[i][0];
    temp=cache->lruarray[i];
    for (j=1;j<cache->ways;j++) {
      temp->lru=&cache->bloarr[i][j];
      temp=temp->lru;
    }
    cache->bloarr[cache->setlength-1][j].lru = NULL;
  }

  return cache;
}

int readd(struct Cache* cache,unsigned long long  t, unsigned long long index){
    int j;
    struct Block *t1, *t2;

    for (j=0;j<cache->ways;j++) {
      if (cache->bloarr[index][j].tag==t) {
        cache->hits++;
        if (&cache->bloarr[index][j]==cache->lruarray[index]) {
          return 1;
        }
        t1=cache->lruarray[index];
        while (t1!=&cache->bloarr[index][j]) {
          t1 = t1->lru;
        }
        t2=cache->lruarray[index];
        while ((t2->lru)!=&cache->bloarr[index][j]) {
          t2 = t2->lru;
        }
        if (t1->lru!=NULL) {
          t2->lru=t1->lru;
        }
        t1->lru=cache->lruarray[index];
        cache->lruarray[index]=t1;

        return 1;
      }
    }

    cache->misses++;
    t1=cache->lruarray[index];
    while (t1->lru!=NULL) {
      t1=t1->lru;
    }
    t1->tag=t;
    t1->lru=cache->lruarray[index];
    cache->lruarray[index]=t1;
    while (t1->lru!=cache->lruarray[index]) {
      t1=t1->lru;
    }
    t1->lru=NULL;

    return 0;
}

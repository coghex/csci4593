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

void printstuff(struct Cache *icache, struct Cache* dcache, struct Cache* l2cache){
  printf("Memory System:\n");
  printf("    Dcache Size = %d : ways = %d : block size = %d\n", dcache->cachesize,dcache->ways,dcache->blocksize);
  printf("    Icache Size = %d : ways = %d : block size = %d\n", icache->cachesize,icache->ways,icache->blocksize);
  printf("    L2-cache Size = %d : ways = %d : block size = %d\n", l2cache->cachesize, l2cache->ways,l2cache->blocksize);
  printf("    Memory ready  time = %d : chunksize = %d : chunktime = %d\n\n", 0,0,0);
  printf("Execute Time = %d; Total References = %d\n", 0, 0);
  printf("Instruction Refereneces = %d; Data refs = %d\n",0,0);
  printf("Number of references types: [Percentage]\n");
  printf("    Reads  =     %d    [%f%%]\n",0,0.0);
  printf("    Writes =     %d    [%f%%]\n",0,0.0);
  printf("    Inst.  =     %d    [%f%%]\n",0,0.0);
  printf("    Total  =     %d\n",0);
  printf("Total cycles for activiites: [Percentage]\n");
  printf("    Reads  =     %d    [%f%%]\n",0,0.0);
  printf("    Writes =     %d    [%f%%]\n",0,0.0);
  printf("    Inst.  =     %d    [%f%%]\n",0,0.0);
  printf("    Total  =     %d\n",0);
  printf("Average cycles for activiites: [Percentage]\n");
  printf("  Read =  %f; Write =  %f; Inst. =  %f\n",0.0,0.0,0.0);
  printf("Ideal: Exec. Time = %d; CPI =  %f\n",0,0.0);
  printf("Ideal mis-aligned: Exec. Time = %d; CPI =  %f\n\n",0,0.0);
  printf("Memory Level: L1i\n");
  printf("    Hit Count = %lld  Miss Count = %lld\n",icache->hits,icache->misses);
  printf("    Total Requests = %lld\n",icache->hits+icache->misses);
  printf("    Hit Rate = %f%%   Miss Rate = %f%%\n",((float)(icache->hits)/(icache->hits+icache->misses)),((float)(icache->misses)/(icache->misses+icache->hits)));
  printf("    Kickouts = %d; Dirty Kickouts = %d; Transfers = %d\n",0,0,0);
  printf("\n");
  printf("Memory Level: L1d\n");
  printf("    Hit Count = %lld  Miss Count = %lld\n",dcache->hits,dcache->misses);
  printf("    Total Requests = %lld\n",dcache->hits+dcache->misses);
  printf("    Hit Rate = %f%%   Miss Rate = %f%%\n",((float)(dcache->hits)/(dcache->hits+dcache->misses)),((float)(dcache->misses)/(dcache->misses+dcache->hits)));
  printf("    Kickouts = %d; Dirty Kickouts = %d; Transfers = %d\n",0,0,0);
  printf("\n");
  printf("Memory Level: L2\n");
  printf("    Hit Count = %lld  Miss Count = %lld\n",l2cache->hits,l2cache->misses);
  printf("    Total Requests = %lld\n",l2cache->hits+l2cache->misses);
  printf("    Hit Rate = %f%%   Miss Rate = %f%%\n",(float)(l2cache->hits/(l2cache->hits+l2cache->misses)),(float)(l2cache->misses/(l2cache->misses+l2cache->hits)));
  printf("    Kickouts = %d; Dirty Kickouts = %d; Transfers = %d\n",0,0,0);
  printf("\n");
  printf("L1 cache cost (Icache $1800) + (Dcache $1800) = $3600\n");
  printf("L2 cache cost = $500;   Memory Cost = $75\n");
  printf("Total cost = $4175\n");
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
    cache->lruarray[i]=&cache->bloarr[i][cache->ways-1];
    temp=cache->lruarray[i];
    for (j=cache->ways-1;j>=0;j--) {
      temp->lru=&cache->bloarr[i][j];
      temp=temp->lru;
    }
    cache->bloarr[i][0].lru = NULL;
  }

  return cache;
}

int readd(struct Cache* cache,unsigned long long  t, unsigned long long index, char op){
    int i, j;
    struct Block *t1, *t2;
    for (j=0;j<cache->ways;j++) {
      if (cache->bloarr[index][j].tag==t) {
        cache->hits++;
        if (&cache->bloarr[index][j]==cache->lruarray[index]) {
          return 0;
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
        else {
          t2->lru=NULL;
        }
        t1->lru=cache->lruarray[index];
        cache->lruarray[index]=t1;

        return 0;
      }
    }
    cache->misses++;
    t1=cache->lruarray[index];

    while (t1->lru!=NULL) {
      t1=t1->lru;
    }
    t1->tag=t;
    t1->valid=1;
    if (op=='W') {
      t1->dirty=1;
    }
    t1->lru=cache->lruarray[index];
    cache->lruarray[index]=t1;


    while (t1->lru!=cache->lruarray[index]) {
      t1=t1->lru;
    }
    t1->lru=NULL;

    for (i=0;i<cache->ways;i++) {
      if (cache->lruarray[index]==&cache->bloarr[index][i]) {
        return 1;
      }
    }

    return 1;
}

#include <stdio.h>
#include <getopt.h>
#include <math.h>
#include <stdlib.h>

struct Block {
  int valid;
  int dirty;
  unsigned long tag[];
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


struct Cache * initcache(int cachesize, int blocksize, int hittime, int misstime, int assoc, int ways) {
  struct Cache *endcache;

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
  endcache->block=(struct Block*)malloc(sizeof(endcache->numblocks*sizeof(struct Block)));

  return endcache;
}

int main(int argc, char* argv[]){
  char op;
  unsigned long addr;
  int size;
  int i, j, incache;
  int c;
  int argvar=2;
  int isc=0;
  int l2buswidth=16;
  int memsendaddrt=10;
  int memreadyt=50;
  int memchunkt=20;
  int memchunk=16;
  int verbose = 0;
  unsigned int tag, index, byte, miss=0, hit=0;
  FILE *f;
  static char usage[] = "usage: cat <traces> | ./cachesim [-hc] <config_file>\n";

  struct Cache *icache;
  struct Cache *dcache;
  struct Cache *l2cache;

  // code to retrive command flags
  while ((c=getopt(argc, argv, "hcv"))!=-1) {
    switch (c) {
      case 'h':
        printf("%s",usage);
        argvar++;
        return 0;
        break;
      case 'c':
        argvar++;
        isc=1;
        break;
      case 'v':
        argvar++;
        verbose=1;
        break;
      default:
        printf("%s",usage);
        argvar++;
    }
  }

  // This will initialize the caches with their default values
  icache = initcache(8192, 32, 1, 1, 1, 1);
  dcache = initcache(8192, 32, 1, 1, 1, 1);
  l2cache = initcache(32768, 64, 5, 8, 1, 1);


  // This will handle all the various inputs
  if (argc > argvar) {
    printf("Too many arguments, -h for usage\n");
    return 0;
  }

  f = fopen(argv[argvar-1], "r");
  if (isc) {
    if (verbose) {
      printf("-c specified, using default parameters\n");
    }
  }
  else if (argc < argvar) {
    if (verbose) {
      printf("No config file specified, default parameters being used...\n");
    }
  }
  else if (f == NULL) {
    if (verbose) {
      printf("Cant open config file, using default parameters...\n");
    }
  }
  else {
    if (verbose) {
      printf("Using provided config file... (Tempararily Out of Order)\n");
    }
  }
  fclose(f);

  printf("Memory System:\n");
  printf("  Dcache size = %d : ways = %d : block size = %d\n", dcache->cachesize, dcache->ways, dcache->blocksize);
  printf("  Icache size = %d : ways = %d : block size = %d\n", icache->cachesize, icache->ways, icache->blocksize);
  printf("  L2 - cache size = %d : ways = %d : block size = %d\n", l2cache->cachesize, l2cache->ways, l2cache->blocksize);
  printf("  Memory ready time = %d : chunksize = %d : chunktime = %d\n\n", memreadyt, memchunk, memchunkt);

  if (verbose) {
    printf("Icache block index = %d : number of blocks = 2^%d : addr size = %d\n\n", icache->bytesize, icache->indexsize, icache->tagsize);
  }

  while (scanf("%c %lX %d\n",&op,&addr,&size)==3) {
    if (verbose) {
      printf("%c,%lX,%d\n",op,addr,size);
    }
    if (op == 'I') {
      tag = (((~0)<<(icache->indexsize+icache->bytesize))&addr)>>(icache->indexsize+icache->bytesize);
      index = (((~((~0)<<icache->indexsize))<<icache->bytesize)&addr)>>(icache->bytesize);
      byte = (~((~0)<<icache->bytesize))&addr;

      int oldbyte = byte;
      incache=1;
      for (j=0;j<=(int)size/4;j++) {
        for (i=byte;i<icache->numblocks;i++) {
          if ((icache->block+(index+j)*sizeof(struct Block))->tag[i] != tag) {
            incache=0;
          }
        }
        byte=0;
      }

      byte=oldbyte;
      if (!incache) {
        for (j=0;j<=(int)size/4;j++) {
          for (i=byte;i<icache->numblocks;i++){
            ((icache->block+(index+j)*sizeof(struct Block)))->tag[i]= tag;
          }
          miss++;
          byte=0;
        }
      }
      else {
        hit++;
      }
      byte=oldbyte;

      if (verbose) {
        printf("  tag: %X index: %X byte: %X\n", tag, index, byte);
        printf("  misses: %d, hits: %d\n", miss, hit);
      }
    }
  }

  free(icache->block);
  free(icache);
  free(dcache->block);
  free(dcache);
  free(l2cache->block);
  free(l2cache);
  return 0;
}








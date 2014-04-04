#include <stdio.h>
#include <getopt.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "cache.h"


int main(int argc, char* argv[]){
  char op;
  unsigned long addr;
  int size;
  int i, j, oldbyte, ref;
  int c;
  int incache[100];
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
  time_t begin, end;
  int totalrefs;

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

  begin=clock();
  while (scanf("%c %lX %d\n",&op,&addr,&size)==3) {
    if (verbose) {
      printf("%c,%lX,%d\n",op,addr,size);
    }
    if (op == 'I') {
      tag = (((~0)<<(icache->indexsize+icache->bytesize))&addr)>>(icache->indexsize+icache->bytesize);
      index = (((~((~0)<<icache->indexsize))<<icache->bytesize)&addr)>>(icache->bytesize);
      byte = (~((~0)<<icache->bytesize))&addr;
      reead(icache, tag, index, byte, size, 1, addr);
      if (verbose) {
        printf("  L1misses: %d, L1hits: %d\n", icache->misses, icache->hits);
        printf("  L2misses: %d, L2hits: %d\n", l2cache->misses, l2cache->hits);
      }
    }
  }
  end=clock();

  totalrefs=(icache->reads+dcache->reads+dcache->writes+l2cache->reads+l2cache->writes);
  printf("Execute time: %d; Total Refs: %d\n", (int)(end-begin), totalrefs);
  //This needs to be fixed, doesnt distiguish between inst and data l2 refs
  printf("Instruction Refs: %d; Data Refs: %d\n\n", icache->reads, totalrefs-icache->reads);


  freecache();
  return 0;
}








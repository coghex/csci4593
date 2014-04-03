#include <stdio.h>
#include <getopt.h>
#include<math.h>

struct Block {
  int valid;
  unsigned int tag;
  int dirty;
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
  struct Block blocks[];
};

int main(int argc, char* argv[]){
  char op;
  unsigned long addr;
  struct Cache icache;
  struct Cache dcache;
  struct Cache l2cache;
  int size;
  int i, incache;
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
  icache.cachesize = 8192;
  icache.blocksize = 32;
  icache.assoc = 1;
  icache.hittime = 1;
  icache.misstime = 1;
  icache.hits = 0;
  icache.misses = 0;
  icache.reads = 0;
  icache.writes = 0;
  icache.ways = 1;
  dcache.cachesize = 8192;
  dcache.blocksize = 32;
  dcache.assoc = 1;
  dcache.hittime = 1;
  dcache.misstime = 1;
  dcache.hits = 0;
  dcache.misses = 0;
  dcache.reads = 0;
  dcache.writes = 0;
  dcache.ways = 1;
  l2cache.cachesize = 32768;
  l2cache.blocksize = 64;
  l2cache.assoc = 1;
  l2cache.hittime = 5;
  l2cache.misstime = 8;
  l2cache.hits = 0;
  l2cache.misses = 0;
  l2cache.reads = 0;
  l2cache.writes = 0;
  l2cache.ways = 1;

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
  printf("  Dcache size = %d : ways = %d : block size = %d\n", dcache.cachesize, dcache.ways, dcache.blocksize);
  printf("  Icache size = %d : ways = %d : block size = %d\n", icache.cachesize, icache.ways, icache.blocksize);
  printf("  L2 - cache size = %d : ways = %d : block size = %d\n", l2cache.cachesize, l2cache.ways, l2cache.blocksize);
  printf("  Memory ready time = %d : chunksize = %d : chunktime = %d\n\n", memreadyt, memchunk, memchunkt);

  icache.numblocks = icache.cachesize/icache.blocksize;
  icache.bytesize = log(icache.blocksize/8)/log(2);
  icache.indexsize = log(icache.numblocks)/log(2);
  icache.tagsize = 38-icache.bytesize-icache.indexsize;

  if (verbose) {
    printf("Icache block index = %d : number of blocks = 2^%d : addr size = %d\n\n", icache.bytesize, icache.indexsize, icache.tagsize);
  }

  while (scanf("%c %lX %d\n",&op,&addr,&size)==3) {
    if (verbose) {
      printf("%c,%lX,%d\n",op,addr,size);
    }
    if (op == 'I') {
      tag = (((~0)<<(icache.indexsize+icache.bytesize))&addr)>>(icache.indexsize+icache.bytesize);
      index = (((~((~0)<<icache.indexsize))<<icache.bytesize)&addr)>>(icache.bytesize);
      byte = (~((~0)<<icache.bytesize))&addr;

      for (i=0;i<size;i++) {
        if ((icache.blocks[index/(icache.blocksize/8)+i].tag != tag) || (0)) {
          incache=0;
          break;
        }
        incache=1;
      }
      if (incache) {
        hit++;
      }
      else {
        miss++;
        for (i=byte;i<(icache.blocksize/8);i++) {
          icache.blocks[index/(icache.blocksize/8)+i].tag = tag;
          icache.blocks[index/(icache.blocksize/8)+i].valid = 1;
        }
      }

      if (verbose) {
        printf("  tag: %X index: %X byte: %X\n", tag, index, byte);
        printf("  misses: %d, hits: %d\n", miss, hit);
      }
    }
  }

  return 0;
}








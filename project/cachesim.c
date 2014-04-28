#include <stdio.h>
#include <getopt.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include "cache.h"



int main(int argc, char* argv[]){
  char op;
  unsigned long long addr;
  int size, length;
  int i;
  int c;
  int argvar=2;
  int isc=0;
  //int l2buswidth=16;
  //int memsendaddrt=10;
  int memreadyt=50;
  int memchunkt=20;
  int memchunk=16;
  int memcost;
  unsigned long long tag, index, byte;
  FILE *f;
  struct winsize w;
  int width;
  static char usage[] = "usage: cat <traces> | ./cachesim [-hc] <config_file>\n";
  time_t begin, end;
  int totalrefs, totaltime;
  int ind;
  char ins[32];
  int verbose=0;

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
  icache = initcache(8192, 32, 1, 1, 1, 0, 4);
  dcache = initcache(8192, 32, 1, 1, 1, 1, 4);
  l2cache = initcache(32768, 64, 5, 8, 1, 1, 16);

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
   while(fscanf(f, "%s %d\n", ins, &ind) != EOF) {
    /*printf("%s %d\n",ins,ind);*/
      /*if (strcmp(ins, "L1BlockSize")) {*/
        /*icache->blocksize=ind;*/
        /*dcache->blocksize=ind;*/
      /*}*/
      /*else if (strcmp(ins, "L1CacheSize")) {*/
        /*icache->cachesize=ind;*/
        /*dcache->cachesize=ind;*/
      /*}*/
      /*else if (strcmp(ins, "L1Assoc")) {*/
        /*icache->assoc=ind;*/
        /*dcache->assoc=ind;*/
      /*}*/
      /*else if (strcmp(ins, "L1HitTime")) {*/
        /*icache->hittime=ind;*/
        /*dcache->hittime=ind;*/
      /*}*/
      /*else if (strcmp(ins, "L1MissTime")) {*/
        /*icache->misstime=ind;*/
        /*dcache->misstime=ind;*/
      /*}*/
      /*else if (strcmp(ins, "L2BlockSize")) {*/
        /*l2cache->blocksize=ind;*/
      /*}*/
    }

  }
  fclose(f);

  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  width=w.ws_col;
  for(i=0;i<width;i++) {
    printf("-");
  }
  printf("\n");

  for(i=0;i<(width/2-9);i++) {
    printf(" ");
  }
  printf("Simulation Results");
  for(i=0;i<(width/2-9);i++) {
    printf(" ");
  }
  printf("\n");

  for(i=0;i<width;i++) {
    printf("-");
  }
  printf("\n\n");

  printf("Memory System:\n");
  printf("  Dcache size = %d : ways = %d : block size = %d\n", dcache->cachesize, dcache->assoc, dcache->blocksize);
  printf("  Icache size = %d : ways = %d : block size = %d\n", icache->cachesize, icache->assoc, icache->blocksize);
  printf("  L2 - cache size = %d : ways = %d : block size = %d\n", l2cache->cachesize, l2cache->assoc, l2cache->blocksize);
  printf("  Memory ready time = %d : chunksize = %d : chunktime = %d\n\n", memreadyt, memchunk, memchunkt);

  if (verbose) {
    printf("Icache block index = %d : number of blocks = 2^%d : tag size = %d\n\n", icache->bytesize, icache->indexsize, icache->tagsize);
  }

  memcost = (int)((200/(memreadyt/50.0)-150) + (100*(memchunk/16.0)-75));

  begin=clock();
  while (scanf("%c %llX %d\n",&op,&addr,&size)==3) {
    if (verbose) {
      printf("%c,%llX,%d\n",op,addr,size);
    }
    if (op == 'I') {
      tag = (((~0)<<(icache->indexsize+icache->bytesize))&addr)>>(icache->indexsize+icache->bytesize);
      index = (((~((~0)<<icache->indexsize))<<icache->bytesize)&addr)>>(icache->bytesize);
      byte = (~((~0)<<icache->bytesize))&addr;
      if (verbose) {
        printf("tag: %llX index: %llX byte: %llX\n", tag, index, byte);
      }
      //addr = addr&((~0)<<(int)(log(icache->bytesize)/log(2)));

      if ((size+(byte%4))%4) {
        length = ((size+(byte%4))/4)+1;
      }
      else {
        length = ((size+(byte%4))/4);
      }
      icache->irefs+=length;
      icache->reads++;
      if ((int)(byte+size)>icache->blocksize) {
        if (verbose) {
          printf("address %llX length %d\n", addr, (int)((icache->blocksize)-(byte))/4+1);
        }
        reead(icache, addr, ((icache->blocksize)-(byte))/4+1, verbose, 1, op, size);
        addr = addr+(1<<(icache->bytesize));
        length=length-((icache->blocksize)-(byte))/4-1;
        if (verbose) {
          printf("address %llX length %d\n", addr, length);
        }

        reead(icache, addr, length, verbose, 1, op, size);
      }
      else {
        if (verbose) {
          printf("address %llX length %d\n", addr, length);
        }
        reead(icache, addr, length, verbose, 1, op, size);
      }
      if (verbose){
        printf("\n");
      }
    }
    if (op == 'R') {
      tag = (((~0)<<(dcache->indexsize+dcache->bytesize))&addr)>>(dcache->indexsize+dcache->bytesize);
      index = (((~((~0)<<dcache->indexsize))<<dcache->bytesize)&addr)>>(dcache->bytesize);
      byte = (~((~0)<<dcache->bytesize))&addr;
      if (verbose) {
        printf("tag: %llX index: %llX byte: %llX\n", tag, index, byte);
      }
      //addr = addr&((~0)<<(int)(log(dcache->bytesize)/log(2)));

      if ((size+(byte%4))%4) {
        length = ((size+(byte%4))/4)+1;
      }
      else {
        length = ((size+(byte%4))/4);
      }
      dcache->drefs+=length;
      dcache->reads++;
      if ((int)(byte+size)>dcache->blocksize) {
        if (verbose) {
          printf("address %llX length %d\n", addr, (int)((dcache->blocksize)-(byte))/4+1);
        }
        reead(dcache, addr, ((dcache->blocksize)-(byte))/4+1, verbose, 1, op, size);
        addr = addr+(1<<(dcache->bytesize));
        length=length-((dcache->blocksize)-(byte))/4-1;
        if (verbose) {
          printf("address %llX length %d\n", addr, length);
        }

        reead(dcache, addr, length, verbose, 1, op, size);
      }
      else {
        if (verbose) {
          printf("address %llX length %d\n", addr, length);
        }
        reead(dcache, addr, length, verbose, 1, op, size);
      }
      if (verbose){
        printf("\n");
      }
    }
    if (op == 'W') {
      tag = (((~0)<<(dcache->indexsize+dcache->bytesize))&addr)>>(dcache->indexsize+dcache->bytesize);
      index = (((~((~0)<<dcache->indexsize))<<dcache->bytesize)&addr)>>(dcache->bytesize);
      byte = (~((~0)<<dcache->bytesize))&addr;
      if (verbose) {
        printf("tag: %llX index: %llX byte: %llX\n", tag, index, byte);
      }
      //addr = addr&((~0)<<(int)(log(dcache->bytesize)/log(2)));

      if ((size+(byte%4))%4) {
        length = ((size+(byte%4))/4)+1;
      }
      else {
        length = ((size+(byte%4))/4);
      }
      dcache->drefs+=length;
      dcache->writes++;
      if ((int)(byte+size)>dcache->blocksize) {
        if (verbose) {
          printf("address %llX length %d\n", addr, (int)((dcache->blocksize)-(byte))/4+1);
        }
        reead(dcache, addr, ((dcache->blocksize)-(byte))/4+1, verbose, 1, op, size);
        addr = addr+(1<<(dcache->bytesize));
        length=length-((dcache->blocksize)-(byte))/4-1;
        if (verbose) {
          printf("address %llX length %d\n", addr, length);
        }

        reead(dcache, addr, length, verbose, 1, op, size);
      }
      else {
        if (verbose) {
          printf("address %llX length %d\n", addr, length);
        }
        reead(dcache, addr, length, verbose, 1, op, size);
      }
      if (verbose){
        printf("\n");
      }
    }


  }
  end=clock();

  totaltime=dcache->rtime+l2cache->rtime+dcache->wtime+l2cache->wtime+icache->itime+l2cache->itime;
  totalrefs=(icache->reads+dcache->reads+dcache->writes);
  printf("Execute time: %d; Total Refs: %d\n", icache->itime, totalrefs);
  printf("Instruction Refs: %d; Data Refs: %d\n\n", icache->reads, dcache->reads+dcache->writes);
  printf("Number of reference types:  [Percentage]\n");
  printf("  Reads  = %-15d    [%d%%]\n", dcache->reads, 100*(dcache->reads)/totalrefs);
  printf("  Writes = %-15d    [%d%%]\n", dcache->writes, 100*(dcache->writes)/totalrefs);
  printf("  Inst.  = %-15d    [%d%%]\n", icache->reads, 100*(icache->reads)/totalrefs);
  printf("  Total  = %-d\n\n", totalrefs);
  printf("Total cycles for activities:  [Percentage]\n");
  printf("  Reads  = %-17d    [%d%%]\n", dcache->rtime+l2cache->rtime, 100*(dcache->rtime+l2cache->rtime)/totaltime);
  printf("  Writes = %-17d    [%d%%]\n", dcache->wtime+l2cache->wtime, 100*(dcache->wtime+l2cache->wtime)/totaltime);
  printf("  Inst.  = %-17d    [%d%%]\n", icache->itime+l2cache->itime, 100*(icache->itime+l2cache->itime)/totaltime);
  printf("  Total  = %-d\n\n", totaltime);
  printf("Average cycles per activity:\n");
  printf("  Read = %f; Write = %f; Inst. = %f\n", ((float)(dcache->rtime+l2cache->rtime))/(dcache->reads+l2cache->drefs),
        ((float)(dcache->wtime+l2cache->wtime))/(dcache->writes+l2cache->writes), ((float)(icache->itime+l2cache->itime))/(icache->reads+l2cache->irefs));
  printf("\n");
  printf("Memory Level: L1i\n");
  printf("  Hit Count = %d  Miss Count = %d\n", icache->hits, icache->misses);
  printf("  Total Requests = %d\n", icache->hits+icache->misses);
  printf("  Hit Rate = %f%%  Miss Rate = %f%%\n", (100.0*icache->hits)/(icache->hits+icache->misses), (100.0*icache->misses)/(icache->hits+icache->misses));
  printf("  Kickouts = %d; Dirty kickouts = %d; Transfers = %d\n\n", icache->kickouts, icache->dirtykickouts, icache->misses);
  printf("Memory Level: L1d\n");
  printf("  Hit Count = %d  Miss Count = %d\n", dcache->hits, dcache->misses);
  printf("  Total Requests = %d\n", dcache->hits+dcache->misses);
  printf("  Hit Rate = %f%%  Miss Rate = %f%%\n", (100.0*dcache->hits)/(dcache->hits+dcache->misses), (100.0*dcache->misses)/(dcache->hits+dcache->misses));
  printf("  Kickouts = %d; Dirty kickouts = %d; Transfers = %d\n\n", dcache->kickouts, dcache->dirtykickouts, dcache->misses);
  printf("Memory Level: L2\n");
  printf("  Hit Count = %d  Miss Count = %d\n", l2cache->hits, l2cache->misses);
  printf("  Total Requests = %d\n", l2cache->hits+l2cache->misses);
  printf("  Hit Rate = %f%%  Miss Rate = %f%%\n", (100.0*l2cache->hits)/(l2cache->hits+l2cache->misses), (100.0*l2cache->misses)/(l2cache->hits+l2cache->misses));
  printf("  Kickouts = %d; Dirty kickouts = %d; Transfers = %d\n\n", l2cache->kickouts, l2cache->dirtykickouts, l2cache->misses);
  printf("L1 cache cost (Icache $%d) + (Dcache $%d) = $%d\n", icache->cost, dcache->cost, icache->cost+dcache->cost);
  printf("L2 cache cost = $%d; Memory cost $%d\n", l2cache->cost, memcost);
  printf("Total cost = $%d\n\n", icache->cost+dcache->cost+l2cache->cost+memcost);
  for(i=0;i<width;i++) {
    printf("-");
  }
  printf("\n");

  //printcache(l2cache);

  if (verbose) {
    printf("Cache Contents\n");
    printf("Memory Level:  L1i\n");
  }

  //freecache();
  return 0;
}








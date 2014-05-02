#include <stdio.h>
#include <getopt.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include "cache.h"

unsigned long long updateaddr(struct Cache *cache, unsigned long long addrin, int byte) {
  unsigned long long addr;


  addr = addrin + cache->blocksize;

  return addr;
}

unsigned long long reconstruct(struct Cache *cache, unsigned long long tag, unsigned long long index) {
  unsigned long long result;
  result = (tag<<(cache->bytesize+cache->indexsize))|(index<<cache->bytesize);
  return result;
}

int refs(int byte, int size, int blocksize) {
  int len;
  if ((size+(byte%blocksize))%blocksize) {
    len = ((size+(byte%blocksize))/blocksize)+1;
  }
  else {
    len = ((size+(byte%blocksize))/blocksize);
  }
  return len;
}

int main(int argc, char* argv[]){
  char op;
  unsigned long long refnum=-1;
  unsigned long long addr[10];
  int size, length, memcost;
  int c, i,j;
  int argvar=2;
  int isc=0;
  unsigned long long res[10];
  unsigned long long tag[10], index[10], byte, l2tag[10], l2index[10], wbaddr, wbtag, wbindex;
  static char usage[] = "usage: cat <traces> | ./cachesim [-hv] <config_file>\n";
  int verbose=0;
  struct Cache *icache, *dcache, *l2cache;
  int memsendaddr=10, memready=50, memchunktime=20, memchunksize=16;
  int l2transtime=6, l2buswidth=16;
  unsigned long long totaltime=0;
  unsigned long long begin;
  unsigned long long misallignments=0;
  FILE *config;
  int val, l1isize=8192, l1dsize=8192, l2size=32768, l1iassoc=1, l1dassoc=1, l2assoc=1;
  char param[16];

  // code to retrive command flags
  while ((c=getopt(argc, argv, "hv"))!=-1) {
    switch (c) {
      case 'h':
        printf("%s",usage);
        argvar++;
        return 0;
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

  if (argc>1) {
    config=fopen(argv[1], "r");
    while (fscanf(config, "%s %d\n", param, &val) != EOF) {
      if (!strcmp(param, "l1isize")) {
        l1isize=val;
      }
      if (!strcmp(param, "l1dsize")) {
        l1dsize=val;
      }
      if (!strcmp(param, "l2size")) {
        l2size=val;
      }
      if (!strcmp(param, "l1iassoc")) {
        l1iassoc=val;
      }
      if (!strcmp(param, "l1dassoc")) {
        l1dassoc=val;
      }
      if (!strcmp(param, "l2assoc")) {
        l2assoc=val;
      }
      if (!strcmp(param, "memchunksize")) {
        memchunksize=val;
      }
    }
  }

  icache = initcache(l1isize, 32, l1iassoc, 1, 1);
  dcache = initcache(l1dsize, 32, l1dassoc, 1, 1);
  l2cache = initcache(l2size, 64, l2assoc, 8, 5);
  icache->cost=calcost(1, l1isize, l1iassoc);
  dcache->cost=calcost(1, l1dsize, l1dassoc);
  l2cache->cost=calcost(2, l2size, l2assoc);
  memcost=200*(memchunksize/16-1)+50+100*(50/memready-1)+25;

  icache->misses=0;
  while (scanf("%c %llX %d\n",&op,&addr[0],&size)==3) {
    if (verbose) {
      printf("%c,%llX,%d\n",op,addr[0],size);
    }
    refnum++;
    begin = icache->itime+dcache->rtime+dcache->wtime+l2cache->rtime+l2cache->wtime+l2cache->itime;

    if (op == 'I') {
      icache->rrefs++;
      byte = (~((~0)<<icache->bytesize))&addr[0];
      length = refs(byte, size, icache->blocksize);
      addr[0] = addr[0]&(~3);
      misallignments+=length-1;
      if (verbose) {
        printf("Ref %lld: Addr = %llx, Type = %c, BSize = %d\n", refnum, addr[0], op, size);
      }
      for (i=0;i<length;i++) {
        tag[i] = (((~0)<<(icache->indexsize+icache->bytesize))&addr[i])>>(icache->indexsize+icache->bytesize);
        index[i] = (((~((~0)<<icache->indexsize))<<icache->bytesize)&addr[i])>>(icache->bytesize);
        if (verbose) {
          printf("Level L1i access addr = %llx, offset = %llx, reftype = Read\n", addr[i], byte);
          printf("    index = %llX, tag =   %llx  ", index[i], tag[i]);
        }
        res[i] = readd(icache, tag[i], index[i], op);
        if (res[i]) {
          icache->itime+=icache->misstime;
        }
        else {
          icache->itime+=icache->hittime;
        }

        if (verbose) {
          if (res[i]) {
            printf("MISS\n");
            printf("Add L1i miss time (+ %d)\n", icache->misstime);
          }
          else {
            printf("HIT\n");
            printf("Add L1i hit time (+ %d)\n", icache->hittime);
          }
        }
        addr[i+1] = updateaddr(icache, addr[i], byte);
        byte=0;
        if (res[i]) {
          l2tag[i] = (((~0)<<(l2cache->indexsize+l2cache->bytesize))&addr[i])>>(l2cache->indexsize+l2cache->bytesize);
          l2index[i] = (((~((~0)<<l2cache->indexsize))<<l2cache->bytesize)&addr[i])>>(l2cache->bytesize);
          if (verbose) {
            printf("Level L2 access addr = %llx, offset = %llx, reftype = Read\n", addr[i], byte);
            printf("    index = %llX, tag =   %llx  ", l2index[i], l2tag[i]);
          }

          if (res[i]>1) {
            wbaddr = reconstruct(icache, res[i], index[i]);
            wbtag = (((~0)<<(l2cache->indexsize+l2cache->bytesize))&wbaddr)>>(l2cache->indexsize+l2cache->bytesize);
            wbindex = (((~((~0)<<l2cache->indexsize))<<l2cache->bytesize)&wbaddr)>>(l2cache->bytesize);
            res[i] = readd(l2cache, wbtag, wbindex, 'W');
            if (res[i]>1) {
              readd(l2cache, wbtag, wbindex, op);
            }

            res[i] = readd(icache, tag[i], index[i], op);
            if (res[i]) {
              readd(l2cache, l2tag[i], l2index[i], op);
              if (res[i]>1) {
                readd(l2cache, l2tag[i], l2index[i], op);
              }
            }

          }
          else {
            res[i] = readd(l2cache, l2tag[i], l2index[i], op);
            if (res[i]>1) {
              readd(l2cache, l2tag[i], l2index[i], op);
            }
          }

          if (res[i]) {
            l2cache->itime+=l2cache->misstime + l2cache->hittime;
            l2cache->itime+=memsendaddr+memready+memchunktime*l2cache->blocksize*8/memchunksize;
            icache->itime+=l2transtime*(icache->blocksize*8/l2buswidth);
            icache->itime+=icache->hittime;
          }
          else {
            l2cache->itime+=l2cache->hittime;
            l2cache->itime+=l2transtime*(icache->blocksize*8/l2buswidth)+icache->hittime;
          }


          if (verbose) {
            if (res[i]) {
              printf("MISS\n");
              printf("Add L2 miss time (+ %d)\n", l2cache->misstime);
              printf("Bringing line into L2.\n");
              printf("Add memory to L2 transfer time (+ %d)\n", memsendaddr+memready+memchunktime*l2cache->blocksize*8/memchunksize);
              printf("Add L2 hit replay time (+ %d)\n", l2cache->hittime);
              printf("Bringing line into L1i.\n");
              printf("Add L2 to L1 transfer time (+ %d)\n", l2transtime*(icache->blocksize*8/l2buswidth));
              printf("Add L1i hit replay time (+ %d)\n", icache->hittime);
            }
            else {
              printf("HIT\n");
              printf("Add L2 hit time (+ %d)\n", l2cache->hittime);
              printf("Bringing line into L1i.\n");
              printf("Add L2 to L1 transfer time (+ %d)\n", l2transtime*(icache->blocksize*8/l2buswidth));
              printf("Add L1i hit replay time (+ %d)\n", icache->hittime);
            }
          }
        }
      }
    }
    if (op == 'R') {
      dcache->rrefs++;
      byte = (~((~0)<<dcache->bytesize))&addr[0];
      length = refs(byte, size, dcache->blocksize);
      misallignments+=length-1;
      addr[0] = addr[0]&(~3);
      if (verbose) {
        printf("Ref %lld: Addr = %llx, Type = %c, BSize = %d\n", refnum, addr[0], op, size);
      }
      for (i=0;i<length;i++) {
        tag[i] = (((~0)<<(dcache->indexsize+dcache->bytesize))&addr[i])>>(dcache->indexsize+dcache->bytesize);
        index[i] = (((~((~0)<<dcache->indexsize))<<dcache->bytesize)&addr[i])>>(dcache->bytesize);
        if (verbose) {
          printf("Level L1d access addr = %llx, offset = %llx, reftype = Read\n", addr[i], byte);
          printf("    index = %llX, tag =   %llx  ", index[i], tag[i]);
        }
        res[i] = readd(dcache, tag[i], index[i], op);
        if (res[i]) {
          dcache->rtime+=dcache->misstime;
        }
        else {
          dcache->rtime+=dcache->hittime;
        }

        if (verbose) {
          if (res[i]) {
            printf("MISS\n");
          }
          else {
            printf("HIT\n");
          }
        }
        addr[i+1] = updateaddr(dcache, addr[i], byte);
        byte=0;
        if (res[i]) {
          l2tag[i] = (((~0)<<(l2cache->indexsize+l2cache->bytesize))&addr[i])>>(l2cache->indexsize+l2cache->bytesize);
          l2index[i] = (((~((~0)<<l2cache->indexsize))<<l2cache->bytesize)&addr[i])>>(l2cache->bytesize);
          if (verbose) {
            printf("Level L2 access addr = %llx, offset = %llx, reftype = Read\n", addr[i], byte);
            printf("    index = %llX, tag =   %llx  ", l2index[i], l2tag[i]);
          }

          if (res[i]>1) {
            wbaddr = reconstruct(dcache, res[i], index[i]);
            wbtag = (((~0)<<(l2cache->indexsize+l2cache->bytesize))&wbaddr)>>(l2cache->indexsize+l2cache->bytesize);
            wbindex = (((~((~0)<<l2cache->indexsize))<<l2cache->bytesize)&wbaddr)>>(l2cache->bytesize);
            res[i] = readd(l2cache, wbtag, wbindex, 'W');
            if (res[i]>1) {
              readd(l2cache, wbtag, wbindex, op);
            }

            res[i] = readd(dcache, tag[i], index[i], op);
            if (res[i]) {
              readd(l2cache, l2tag[i], l2index[i], op);
              if (res[i]>1) {
                readd(l2cache, l2tag[i], l2index[i], op);
              }
            }
          }
          else {
            res[i] = readd(l2cache, l2tag[i], l2index[i], op);
            if (res[i]>1) {
              readd(l2cache, l2tag[i], l2index[i], op);
            }

          }
          if (res[i]) {
            l2cache->rtime+=l2cache->misstime + l2cache->hittime;
            l2cache->rtime+=memsendaddr+memready+memchunktime*l2cache->blocksize*8/memchunksize;
            dcache->rtime+=l2transtime*(dcache->blocksize*8/l2buswidth);
            dcache->rtime+=dcache->hittime;
          }
          else {
            l2cache->rtime+=l2cache->hittime;
            dcache->rtime+=l2transtime*(dcache->blocksize*8/l2buswidth)+dcache->hittime;
          }


          if (verbose) {
            if (res[i]) {
              printf("MISS\n");
              printf("Add L2 miss time (+ %d)\n", l2cache->misstime);
              printf("Bringing line into L2.\n");
              printf("Add memory to L2 transfer time (+ %d)\n", memsendaddr+memready+memchunktime*l2cache->blocksize*8/memchunksize);
              printf("Add L2 hit replay time (+ %d)\n", l2cache->hittime);
              printf("Bringing line into L1i.\n");
              printf("Add L2 to L1 transfer time (+ %d)\n", l2transtime*(dcache->blocksize*8/l2buswidth));
              printf("Add L1d hit replay time (+ %d)\n", icache->hittime);
            }
            else {
              printf("HIT\n");
              printf("Add L2 hit time (+ %d)\n", l2cache->hittime);
              printf("Bringing line into L2.\n");
              printf("Add L2 to L1 transfer time (+ %d)\n", l2transtime*(dcache->blocksize*8/l2buswidth));
              printf("Add L1d hit replay time (+ %d)\n", dcache->hittime);
            }
          }
        }
      }
    }

    if (op == 'W') {
      dcache->wrefs++;
      byte = (~((~0)<<dcache->bytesize))&addr[0];
      length = refs(byte, size, dcache->blocksize);
      misallignments+=length-1;
      addr[0] = addr[0]&(~3);
      if (verbose) {
        printf("Ref %lld: Addr = %llx, Type = %c, BSize = %d\n", refnum, addr[0], op, size);
      }
      for (i=0;i<length;i++) {
        tag[i] = (((~0)<<(dcache->indexsize+icache->bytesize))&addr[i])>>(dcache->indexsize+dcache->bytesize);
        index[i] = (((~((~0)<<dcache->indexsize))<<dcache->bytesize)&addr[i])>>(dcache->bytesize);
        if (verbose) {
          printf("Level L1d access addr = %llx, offset = %llx, reftype = Read\n", addr[i], byte);
          printf("    index = %llX, tag =   %llx  ", index[i], tag[i]);
        }
        res[i] = readd(dcache, tag[i], index[i], op);
        if (res[i]) {
          dcache->wtime+=dcache->misstime;
        }
        else {
          dcache->wtime+=dcache->hittime;
        }

        if (verbose) {
          if (res[i]) {
            printf("MISS\n");
            printf("Add L1d miss time (+ %d)\n", dcache->misstime);
          }
          else {
            printf("HIT\n");
            printf("Add L1d hit time (+ %d)\n", dcache->hittime);
          }
        }
        addr[i+1] = updateaddr(dcache, addr[i], byte);
        byte=0;
        if (res[i]) {
          l2tag[i] = (((~0)<<(l2cache->indexsize+l2cache->bytesize))&addr[i])>>(l2cache->indexsize+l2cache->bytesize);
          l2index[i] = (((~((~0)<<l2cache->indexsize))<<l2cache->bytesize)&addr[i])>>(l2cache->bytesize);
          if (verbose) {
            printf("Level L2 access addr = %llx, offset = %llx, reftype = Read\n", addr[i], byte);
            printf("    index = %llX, tag =   %llx  ", l2index[i], l2tag[i]);
          }

          if (res[i]>1) {
            wbaddr = reconstruct(dcache, res[i], index[i]);
            wbtag = (((~0)<<(l2cache->indexsize+l2cache->bytesize))&wbaddr)>>(l2cache->indexsize+l2cache->bytesize);
            wbindex = (((~((~0)<<l2cache->indexsize))<<l2cache->bytesize)&wbaddr)>>(l2cache->bytesize);
            res[i] = readd(l2cache, wbtag, wbindex, 'W');

            if (res[i]>1) {
              readd(l2cache, wbtag, wbindex, op);
            }

            res[i] = readd(dcache, tag[i], index[i], op);
            if (res[i]) {
              readd(l2cache, l2tag[i], l2index[i], op);
              if (res[i]>1) {
                readd(l2cache, l2tag[i], l2index[i], op);
              }
            }
          }
          else {
            res[i] = readd(l2cache, l2tag[i], l2index[i], op);
            if (res[i]>1) {
              readd(l2cache, l2tag[i], l2index[i], op);
            }

          }

          if (res[i]) {
            l2cache->wtime+=l2cache->misstime + l2cache->hittime;
            l2cache->wtime+=memsendaddr+memready+memchunktime*l2cache->blocksize*8/memchunksize;
            dcache->wtime+=l2transtime*(icache->blocksize*8/l2buswidth);
            dcache->wtime+=icache->hittime;
          }
          else {
            l2cache->wtime+=l2cache->hittime;
            dcache->wtime+=l2transtime*(icache->blocksize*8/l2buswidth);
            dcache->wtime+=icache->hittime;
          }

          if (verbose) {
            if (res[i]) {
              printf("MISS\n");
              printf("Add L2 miss time (+ %d)\n", l2cache->misstime);
              printf("Bringing line into L2.\n");
              printf("Add memory to L2 transfer time (+ %d)\n", memsendaddr+memready+memchunktime*l2cache->blocksize*8/memchunksize);
              printf("Add L2 hit replay time (+ %d)\n", l2cache->hittime);
              printf("Bringing line into L1i.\n");
              printf("Add L2 to L1 transfer time (+ %d)\n", l2transtime*(dcache->blocksize*8/l2buswidth));
              printf("Add L1d hit replay time (+ %d)\n", dcache->hittime);
            }
            else {
              printf("HIT\n");
              printf("Add L2 hit time (+ %d)\n", l2cache->hittime);
              printf("Bringing line into L2.\n");
              printf("Bringing line into L1i.\n");
              printf("Add L2 to L1 transfer time (+ %d)\n", l2transtime*(dcache->blocksize*8/l2buswidth));
              printf("Add L1d hit replay time (+ %d)\n", dcache->hittime);
            }
          }
        }
      }
    }
    totaltime+=icache->itime+dcache->rtime+dcache->wtime+l2cache->rtime+l2cache->wtime+l2cache->itime-begin;
    if (verbose) {
      printf("Simulated Time: %lld\n", totaltime);
    }
  }
  //if (verbose) {
   // printcache(icache);
   // printf("\n");
   // printcache(dcache);
   // printf("\n");
 //   printcache(l2cache);
 // }
  printstuff(icache, dcache, l2cache, memready, memchunksize, memchunktime, refnum, misallignments, memcost);
  return 0;
}

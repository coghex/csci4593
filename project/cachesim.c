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
  int refnum=-1;
  unsigned long long addr[10];
  int size, length;
  int c, i,j;
  int argvar=2;
  int isc=0;
  int res[10];
  unsigned long long tag[10], index[10], byte;
  static char usage[] = "usage: cat <traces> | ./cachesim [-hv] <config_file>\n";
  int verbose=0;
  struct Cache *icache, *dcache, *l2cache;

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
  icache = initcache(8192, 32, 4);
  dcache = initcache(8192, 32, 4);
  l2cache = initcache(32768, 64, 4);

  icache->misses=0;
  while (scanf("%c %llX %d\n",&op,&addr[0],&size)==3) {
    if (verbose) {
      printf("%c,%llX,%d\n",op,addr[0],size);
    }
    refnum++;

    if (op == 'I') {
      icache->rrefs++;
      byte = (~((~0)<<icache->bytesize))&addr[0];
      length = refs(byte, size, icache->blocksize);
      addr[0] = addr[0]&(~3);
      if (verbose) {
        printf("Ref %d: Addr = %llx, Type = %c, BSize = %d\n", refnum, addr[0], op, size);
      }
      for (i=0;i<length;i++) {
        tag[i] = (((~0)<<(icache->indexsize+icache->bytesize))&addr[i])>>(icache->indexsize+icache->bytesize);
        index[i] = (((~((~0)<<icache->indexsize))<<icache->bytesize)&addr[i])>>(icache->bytesize);
        if (verbose) {
          printf("Level L1i access addr = %llx, offset = %llx, reftype = Read\n", addr[i], byte);
          printf("    index = %llX, tag =   %llx  ", index[i], tag[i]);
        }
        res[i] = readd(icache, tag[i], index[i], op);
        if (verbose) {
          if (res[i]) {
            printf("MISS\n");
          }
          else {
            printf("HIT\n");
          }
        }
        addr[i+1] = updateaddr(icache, addr[i], byte);
        byte=0;
        if (res[i]) {
          tag[i] = (((~0)<<(l2cache->indexsize+l2cache->bytesize))&addr[i])>>(l2cache->indexsize+l2cache->bytesize);
          index[i] = (((~((~0)<<l2cache->indexsize))<<l2cache->bytesize)&addr[i])>>(l2cache->bytesize);
          if (verbose) {
            printf("Level L2 access addr = %llx, offset = %llx, reftype = Read\n", addr[i], byte);
            printf("    index = %llX, tag =   %llx  ", index[i], tag[i]);
          }

          res[i] = readd(l2cache, tag[i], index[i], op);
          if (verbose) {
            if (res[i]) {
              printf("MISS\n");
            }
            else {
              printf("HIT\n");
            }
          }
        }
      }
    }
    if (op == 'R') {
      dcache->rrefs++;
      byte = (~((~0)<<dcache->bytesize))&addr[0];
      length = refs(byte, size, dcache->blocksize);
      addr[0] = addr[0]&(~3);
      if (verbose) {
        printf("Ref %d: Addr = %llx, Type = %c, BSize = %d\n", refnum, addr[0], op, size);
      }
      for (i=0;i<length;i++) {
        tag[i] = (((~0)<<(dcache->indexsize+dcache->bytesize))&addr[i])>>(dcache->indexsize+dcache->bytesize);
        index[i] = (((~((~0)<<dcache->indexsize))<<dcache->bytesize)&addr[i])>>(dcache->bytesize);
        if (verbose) {
          printf("Level L1d access addr = %llx, offset = %llx, reftype = Read\n", addr[i], byte);
          printf("    index = %llX, tag =   %llx  ", index[i], tag[i]);
        }
        res[i] = readd(dcache, tag[i], index[i], op);
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
          tag[i] = (((~0)<<(l2cache->indexsize+l2cache->bytesize))&addr[i])>>(l2cache->indexsize+l2cache->bytesize);
          index[i] = (((~((~0)<<l2cache->indexsize))<<l2cache->bytesize)&addr[i])>>(l2cache->bytesize);
          if (verbose) {
            printf("Level L2 access addr = %llx, offset = %llx, reftype = Read\n", addr[i], byte);
            printf("    index = %llX, tag =   %llx  ", index[i], tag[i]);
          }

          res[i] = readd(l2cache, tag[i], index[i], op);
          if (res[i]>1) {
            readd(l2cache, tag[i], index[i], 'W');
          }
          if (verbose) {
            if (res[i]) {
              printf("MISS\n");
            }
            else {
              printf("HIT\n");
            }
          }
        }
      }
    }

    if (op == 'W') {
      dcache->wrefs++;
      byte = (~((~0)<<dcache->bytesize))&addr[0];
      length = refs(byte, size, dcache->blocksize);
      addr[0] = addr[0]&(~3);
      if (verbose) {
        printf("Ref %d: Addr = %llx, Type = %c, BSize = %d\n", refnum, addr[0], op, size);
      }
      for (i=0;i<length;i++) {
        tag[i] = (((~0)<<(dcache->indexsize+icache->bytesize))&addr[i])>>(dcache->indexsize+dcache->bytesize);
        index[i] = (((~((~0)<<dcache->indexsize))<<dcache->bytesize)&addr[i])>>(dcache->bytesize);
        if (verbose) {
          printf("Level L1d access addr = %llx, offset = %llx, reftype = Read\n", addr[i], byte);
          printf("    index = %llX, tag =   %llx  ", index[i], tag[i]);
        }
        res[i] = readd(dcache, tag[i], index[i], op);
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
          tag[i] = (((~0)<<(l2cache->indexsize+l2cache->bytesize))&addr[i])>>(l2cache->indexsize+l2cache->bytesize);
          index[i] = (((~((~0)<<l2cache->indexsize))<<l2cache->bytesize)&addr[i])>>(l2cache->bytesize);
          if (verbose) {
            printf("Level L2 access addr = %llx, offset = %llx, reftype = Read\n", addr[i], byte);
            printf("    index = %llX, tag =   %llx  ", index[i], tag[i]);
          }

          res[i] = readd(l2cache, tag[i], index[i], op);
          if (res[i]>1) {
            readd(l2cache, tag[i], index[i], 'W');
          }

          if (verbose) {
            if (res[i]) {
              printf("MISS\n");
            }
            else {
              printf("HIT\n");
            }
          }
        }
      }
    }
  }
  if (verbose) {
    printcache(icache);
  }
  printstuff(icache, dcache, l2cache);
  return 0;
}

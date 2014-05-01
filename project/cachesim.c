#include <stdio.h>
#include <getopt.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include "cache.h"

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
  int refnum=0;
  unsigned long long addr[10];
  int size, length;
  int c, i;
  int argvar=2;
  int isc=0;
  int res;
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

  icache->misses=0;
  while (scanf("%c %llX %d\n",&op,&addr[0],&size)==3) {
    if (verbose) {
      printf("%c,%llX,%d\n",op,addr[0],size);
    }
    if (op == 'I') {
      printcache(icache);
      byte = (~((~0)<<icache->bytesize))&addr[0];
      length = refs(byte, size, icache->blocksize);
      if (verbose) {
        printf("Ref %d: Addr = %llx, Type = %c, BSize = %d\n", refnum, addr[0], op, size);
      }
      for (i=0;i<length;i++) {
        tag[i] = (((~0)<<(icache->indexsize+icache->bytesize))&addr[i])>>(icache->indexsize+icache->bytesize);
        index[i] = (((~((~0)<<icache->indexsize))<<icache->bytesize)&addr[i])>>(icache->bytesize);
        if (verbose) {
          printf("Level L1i access addr = %llx, reftype = Read\n", addr[i]);
          printf("    index = %llX, tag =   %llx  ", index[i], tag[i]);
        }
        res = readd(icache, tag[i], index[i]);
        if (verbose) {
          if (res) {
            printf("MISS\n");
          }
          else {
            printf("HIT\n");
          }
        }
        if (0){//(byte+length*4) > (4*icache->blocksize)) {
          addr[i+1]=(addr[0]+32*(i+1))&((~0)<<icache->bytesize);
          byte=0;
        }
        else {
          addr[i+1]=addr[i];
        }
      }
    }
    refnum++;
  }
  printf("hits: %lld misses: %lld\n", icache->hits, icache->misses);
  return 0;
}







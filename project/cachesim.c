#include <stdio.h>
#include <getopt.h>

int main(int argc, char* argv[]){
  char op;
  unsigned long addr;
  int size;
  int c;
  int argvar=2;
  int isc=0;
  int il1=8000;
  int il1map=0;
  int dl1=8000;
  int dl1map=0;
  int l2=32000;
  int l2map=0;
  FILE *f;
  static char usage[] = "usage: cat <traces> | ./cachesim <config_file> [-hc]\n";

  while ((c=getopt(argc, argv, "hc"))!=-1) {
    switch (c) {
      case 'h':
        printf("This is where a help message should print\n");
        argvar++;
        break;
      case 'c':
        argvar++;
        isc=1;
        break;
      default:
        printf("%s",usage);
        argvar++;
    }
  }

  if (argc > argvar) {
    printf("Too many arguments, -h for usag\ne");
  }

  f = fopen(argv[argvar-1], "r");
  if (isc) {
    printf("-c specified, using default parameters\n");
  }
  else if (argc < argvar) {
    printf("No config file specified, default parameters being used...\n");
  }
  else if (f == NULL) {
    printf("Cant open config file, using default parameters...\n");
  }
  else {
    while (fscanf(f, "%d %d %d %d %d %d", &il1, &il1map, &dl1, &dl1map, &l2, &l2map) != EOF);
    printf("Using provided config file...\n");
  }

  while (scanf("%c %lX %d\n",&op,&addr,&size)==3) {
    printf("%c,%lX,%d\n",op,addr,size);
  }

  fclose(f);
  return 0;
}

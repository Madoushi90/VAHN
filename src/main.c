#include <sahn.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){
  if(argc != 3){
    printf("Usage: vahn <node-file> <address>\n");
    exit(1);
  }

  sahn_init(argv[1],atoi(argv[2]));

  sahn_cleanup();
  return 0;
}

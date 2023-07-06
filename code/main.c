#include "kernel.h"

int main(void) {
  kernelInit();
  kernelLoop();  //execução do escalonador batch
  kernelLoop();  //execução do escalonador interativo
  return 0;
}
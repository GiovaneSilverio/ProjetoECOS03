#include <stdio.h>
#include <stdlib.h>

//==========================
// Declaração do arquivo
//==========================
FILE *input;

//==========================
// Flags de retorno
//==========================

#define SUCCESS 0
#define FAIL 1
#define REPEAT 2

//==========================
// Definições
//==========================
#define MAX_PROCESSES 10
#define MAX_QUEUE_SIZE 20
#define MAX_NUM_QUEUES 10

//==========================
// Ponteiro de função
//==========================
typedef char (*ptrFunc)(void);

//==========================
// Enum de estados
//==========================
typedef enum {
  BATCH = 0,
  INTERACTIVE
} STATES;

//==========================
// Definição das estruturas 
//==========================
typedef struct {
  int id;
  int priority;
  int time_left;
  int quantum;
  int running;
  int done;
} Process;

typedef struct {
    Process buffer[MAX_QUEUE_SIZE];
    int head;
    int tail;
} Queue;

typedef struct {
    Queue *queues;
    Process *process_pool;
    int num_queues;
    ptrFunc exec;
} Scheduler;

//==========================
// Criação das estruturas
//==========================
Scheduler circular_buffer;
Process processes[MAX_PROCESSES];
Queue queues[MAX_PROCESSES];
STATES state; 

//==========================
// Variáveis globais
//==========================
int start, end, linha, flag;

//==========================
// Protótipos de função
//==========================
char priorityScheduling(void);
char roundRobinScheduling(void);
char kernelInit(void);
char kernelAddProc();
char kernelRemoveProc();
void kernelLoop(void);
char kernelScheduler(void);
char exec(void);
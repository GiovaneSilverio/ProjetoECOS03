#include "kernel.h"

int main(void) {
  kernelInit();
  kernelLoop();  //execução do escalonador batch
  kernelLoop();  //execução do escalonador interativo
  return 0;
}

char kernelInit() { //inicializa o estado do tempo zero do kernel
  start = 0;                                 //indice que controla a inserção ou exclusão de itens do buffer circular
  end = 0;                                   //indice que irá percorrer o buffer circular
  linha = 0;                                 //controle numero de linhas
  state = BATCH;                             //estado do sistema para automatizar a execução
  //abertura do arquivo com os processos
  input = fopen("input.txt", "r");
  if (input == NULL) {  //verificação de erro
    printf("Erro ao abrir o arquivo.");
    return FAIL;
  }
  return SUCCESS;
}

char kernelAddProc() { //função que adiciona processos na última posição do buffer circular
  //lê a linha do arquivo
  fscanf(input, "%d %d %d %d", &processes[end].id, &processes[end].priority, &processes[end].time_left, &processes[end].quantum);
  if (processes[end].id >= 0)
    return SUCCESS;
  return FAIL;
}

char kernelRemoveProc() { //remove o processo do buffer circular movendo sua posição de start
  if (end != start) {
    processes[start].done = 1; //atualiza o estado de processo realizado com sucesso
    processes[start].priority = 0;
    processes[start].running = 0;
    processes[start].time_left = 0;
    start = (start + 1) % MAX_PROCESSES;
    return SUCCESS;
  }
  return FAIL;
}

void kernelLoop(void) { //loop de execução
  switch (state)  //identifica o estado
  {
    case BATCH:  //algoritmo em Batch
      printf("\n-------------- Priority Scheduling --------------\n");
      circular_buffer.exec = priorityScheduling;

      while (!feof(input)) {  //equanto não chegar ao fim do arquivo de input
        //preenchendo o buffer circular
        if (((end + 1) % MAX_PROCESSES) != start) {
          kernelAddProc();                    //adiciona o processo ao buffer
          end = (end + 1) % (MAX_PROCESSES);  //atualiza o indice do buffer
        } else {                              //ao encher o buffer começa a execução dos processos
          circular_buffer.exec();
        }
      }
      while (circular_buffer.exec() != FAIL) { //executa até acabar de esvaziar o buffer
        //atualiza a prioridade do processo para que ele seja marcado com 'executado'
        //buffer
        processes[start - 1].priority = 0;
      }
      fclose(input);     //fecha o arquivo de processos
      kernelScheduler();  //muda para o próximo estado, para executar o próximo algoritmo
      break;

    case INTERACTIVE:  // adicionará todos os processos do arquivo às filas para então executar
      printf("\n-------------- Round Robin Scheduling --------------\n");
      linha = 0;
      start = 0;
      end = 0;
      flag = 0;

      input = fopen("input.txt", "r");
      if (input == NULL) {
        printf("Erro ao abrir o arquivo.");
      }

      circular_buffer.exec = roundRobinScheduling;

      while (!feof(input)) {  //equanto não chegar ao fim do arquivo de input
        //preenchendo o buffer circular
        if (((end + 1) % MAX_PROCESSES) != start) {
          kernelAddProc();                    //adiciona o processo ao buffer
          end = (end + 1) % (MAX_PROCESSES);  //atualiza o indice do buffer
        } else {                              //ao encher o buffer começa a execução dos processos
          circular_buffer.exec();
        }
      }
      //execução dos processos restantes do buffer após todos os processos
      //do arquivo terem sido lidos
      while (circular_buffer.exec() != FAIL) {
        //atualiza a prioridade do processo para que ele seja marcado com 'executado'
        //buffer
        processes[start - 1].priority = 0;
      }
      fclose(input);     //fecha o arquivo de processos
      kernelScheduler();  //muda para o próximo estado, para executar o próximo algoritmo
      break;
    default:
      break;
  }
}

char kernelScheduler(void) { //automatiza o a execução para ambos os escalonadores
  if (state != INTERACTIVE) {
    state = INTERACTIVE;
    return SUCCESS;
  } else {
    state = BATCH;
    return SUCCESS;
  }
  return FAIL;
}

char priorityScheduling(void) { //escalonador batch de prioridade
  int end_aux = start;                               //indice auxiliar para percorrer o buffer circular
  Process highestPriorityProcess = processes[start]; //processo auxiliar para encontrar a maior prioridade
  while (((end_aux + 1) % MAX_PROCESSES) != start) { //organiza por prioridade enquanto o buffer não está cheio
    if (processes[end_aux].priority > highestPriorityProcess.priority && processes[end_aux].priority > 0) {
      highestPriorityProcess = processes[end_aux];
      processes[end_aux] = processes[start];
      processes[start] = highestPriorityProcess;
    }
    end_aux = (end_aux + 1) % (MAX_PROCESSES);  //atualiza indice que está percorrendo o buffer
  }
  processes[start].time_left = 0;  //atualiza o tempo restante de execução
  //imprime o processo a ser executado
  printf("%d. Process ID: %d \t\tPRIORITY: %d\t\tTIME-LEFT: %d\n", linha++, processes[start].id, processes[start].priority, processes[start].time_left);
  //verifica se houve a exclusão de um processo
  if (kernelRemoveProc() == SUCCESS) return SUCCESS;
  else return FAIL;
}

char roundRobinScheduling(void) { //escalonador interativo round robin
  int quantum = processes[start].quantum; //identifica o quantum do processo
  processes[start].time_left -= quantum; //realiza o time slice
  if (processes[start].time_left > 0) { //se ainda resta execução devolve ele pro fim do buffer
    printf("%d. Process ID: %d \t\tTIME-LEFT: %d \t\tQUANTUM: %d\n", linha++, processes[start].id, processes[start].time_left, processes[start].quantum);
    processes[start].running = 1; //em execução
    end = start;    //atualização do buffer
    start = (start + 1) % (MAX_PROCESSES);
  } else { //se não resta tempo de execução remove do buffer
    printf("%d. Process ID: %d \t\tTIME-LEFT: %d\t\tQUANTUM: %d\n", linha++, processes[start].id, processes[start].time_left, processes[start].quantum);
    if (kernelRemoveProc() == SUCCESS) {
      return SUCCESS;
    } else
      return FAIL;
  }
  return SUCCESS;
}
/* Contador de palavras
 *
 * Este programa recebera uma serie de caracteres representando palavras em sua
 * entrada. Ao receber um caractere fim de linha ('\n'), deve imprimir na tela o
 * numero de palavras separadas que recebeu e, apos, encerrar.
 */
//Lucas Rodolfo de Castro Moura - 156405
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

#define MAX_WORKERS 4
#define ATIVO 1
#define INATIVO 0

typedef struct {
  int N;
  int ID;
} thread_args;

pthread_mutex_t trava;
int n_workers = 0;
pthread_t workers[MAX_WORKERS];
int worker_status[MAX_WORKERS];
int contador=0;

int ehPrimo(long long int numero){            /*Funcao que analisa se um numero eh primo ou nao*/
      int flag = 0;
      for(long long int i = 2; i <= numero/2; ++i){
          if(numero%i == 0){
              flag = 1;
              break;
          }
      }
      if (numero == 1 || numero == 0){
        return 0;
      }
      else{
          if (flag == 0){
            return 1;
          }else{
            return 0;
          }
      }
}

void* worker(void *arg) {
  thread_args *info = (thread_args *)arg;
  pthread_mutex_lock(&trava);
  int M = ehPrimo(info->N);
  if (M==1) {               //Se M == 1, o numero avaliado eh primo, entao aumenta o contador
    contador=contador+1;
  }
  n_workers -= 1;
  worker_status[info->ID] = INATIVO;
  free(info);
  pthread_mutex_unlock(&trava);
  return NULL;
}

int main() {
  thread_args *send_args;
  char temp;
  long long int vetor[100];
  int i=0, tamanho=0, j=0, posicao, numero_recebido;

  do {                                    /*Esse loop lê string de entrada e armazena em um vetor de inteiros*/
    scanf("%lld%c", &vetor[i], &temp);
    i++;
  } while(temp != '\n');
  tamanho =i;                             /*Guarda o tamanho do vetor de inteiros criado*/

  while (tamanho>=0) {
    numero_recebido = vetor[tamanho];
    if (n_workers >= MAX_WORKERS) {
      //Muitas tarefas sendo executadas. Aguardando...
    } else {
      pthread_mutex_lock(&trava);
      send_args = (thread_args*)malloc(sizeof(thread_args));
      send_args->N = numero_recebido;
      /* Procura espaco para thread livre */
      j = 0;
      while (worker_status[j] == ATIVO) j++;
      send_args->ID = j;
      worker_status[j] = ATIVO;
      n_workers += 1;
      tamanho = tamanho - 1;
      pthread_create(& (workers[j]), NULL, worker, (void*) send_args);
      pthread_mutex_unlock(&trava);
    }
  }

  while (1) {  //Aguardando todos os workers terminarem antes do resultado final
    if (n_workers==0) {
      printf("%d\n",contador );
      break;
    }
  }

  return 0;
}

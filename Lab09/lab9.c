/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Módulo 3 - Laboratório: 9 */
/* Codigo: Variação na implementação do problema produtor/consumidor, usando semáforos.*/
/* Aluno: Raphael Mesquita */ 
/* DRE: 118020104 */ 

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define Prod 5
#define Cons 3
#define N 41

sem_t mutexProd, mutexCons; // Exclusão mútua
sem_t bufferCheio, slotVazio; // Condição
int Buffer[N]; // Buffer de elementos
int hasDelay = 0; // O programa deve haver pausas ou não

// Função para pausar brevemente a execução do programa.
// Depende se valor de hasDelay é true ou false.
void sleep() {
  long int ticks = hasDelay * 1000000000;
  for(long int i = 0; i < ticks; i++) {}
}

// Função que retira todos os elemento do buffer
void Retira (int* elementos, int id) {
  int out = 0; // Não precisa ser static, pois os elementos são todos retirados de uma vez só.
  // Espera encher o buffer
  sem_wait(&bufferCheio);
  // Exclusão mútua entre consumidores
  sem_wait(&mutexCons);
  for(int i = 0; i < N; i++) {
    elementos[i] = Buffer[out];
    out = (out + 1) % N;
  }
  sem_post(&mutexCons);

  // Imprimindo os valores retirados dentro da Retira() para outra thread
  // não imprimir durante a apresentação dos elementos retirados do buffer
  printf("Consumidor retirando -- id = %d\n", id);
  printf("Elementos consumidos: ");
  for(int i = 0; i < N; i++) printf("%d%s", elementos[i], i == N - 1 ? "\n\n" : " ");

  // Sinaliza o buffer vazio
  for(int i = 0; i < N; i++) {
    sem_post(&slotVazio);
  }
}

// Função que insere um elemento no Buffer
void Insere (int item, int id) {
  static int in=0;
  // Aguarda slot vazio
  sem_wait(&slotVazio);
  // Exclusão mútua entre produtores
  sem_wait(&mutexProd);
  printf("Inserindo %d -- id = %d\n", item, id);
  Buffer[in] = item;
  in++;
  sem_post(&mutexProd);
  // Sinaliza o buffer cheio
  if(in == N) {
    in = 0;
    sem_post(&bufferCheio);
  }
}

// Reperesenta as threads produtoras. Insere um elemento por vez.
void *produtor(void * arg) {
  int elemento = 0;
  int id = *(int*)arg;
  while(1) {
    Insere(elemento+id, id);
    elemento++;
    sleep();
  }
  pthread_exit(NULL);
}

// Representa as threads consumidoras. Consome todo o buffer de uma vez só
void *consumidor(void * arg) {
  int* elementos;
  int id = *(int*)arg;
  elementos = (int*) malloc(sizeof(int) * N);
  while(1) {
    Retira(elementos, id);
    sleep();
  }
  free(elementos);
  pthread_exit(NULL);
}

// Função principal do programa
int main() {
  int NTHREADS = Prod + Cons; // Número de threads é a soma de threads consumidoras e threads produtoras
  pthread_t tid[NTHREADS];
  int *id[NTHREADS], i;

  for (i=0; i<NTHREADS; i++) {
    if ((id[i] = malloc(sizeof(int))) == NULL) {
      pthread_exit(NULL); return 1;
    }
    *id[i] = i+1;
  }

  // Inicia os semáforos
  sem_init(&mutexCons, 0, 1);   // Semáforo binário, para exclusão mútua entre consumidores
  sem_init(&mutexProd, 0, 1);   // Semáforo binário, para exclusão mútua entre produtores
  sem_init(&bufferCheio, 0, 0); // Semáforo binário, condicional para consumo do buffer
  sem_init(&slotVazio, 0, N);   // Semáforo para liberar as inserções no buffer

  // Cria as produtoras threads
  for(i = 0; i < Prod; i++) {
    if (pthread_create(&tid[i], NULL, produtor, (void *)id[i])) { printf("ERRO! --> pthread_create()\n"); exit(-1); }
  }

  // Cria as produtoras threads
  for(; i < Prod + Cons; i++) {
    if (pthread_create(&tid[i], NULL, consumidor, (void *)id[i])) { printf("ERRO! --> pthread_create()\n"); exit(-1); }
  }

  // Espera todas as threads terminarem
  for (i=0; i<NTHREADS; i++) {
    if (pthread_join(tid[i], NULL)) { printf("ERRO! --> pthread_join() \n"); exit(-1); } 
    free(id[i]);
  } 
  pthread_exit(NULL);

  return 0;
}
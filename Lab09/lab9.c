#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define Prod 5
#define Cons 1
#define N 4

sem_t mutexProd, mutexCons; // Exclusão mútua
sem_t bufferCheio, slotVazio; // Condição
int Buffer[N];

void sleep() {
  long int ticks = 1000000000;
  for(long int i = 0; i < ticks; i++) {}
}

void Retira (int* elementos) {
  int out = 0;
  // Espera encher o buffer
  sem_wait(&bufferCheio);
  //exclusao mutua entre consumidores
  sem_wait(&mutexCons);
  // printf("Entrou no Retira() -- out = %d\n", out);
  for(int i = 0; i < N; i++) {
    elementos[i] = Buffer[out];
    out = (out + 1) % N;
  }
  sem_post(&mutexCons);
  //sinaliza o buffer vazio
  for(int i = 0; i < N; i++) {
    sem_post(&slotVazio);
  }
}

void Insere (int item) {
  static int in=0;
  //aguarda slot vazio
  sem_wait(&slotVazio);
  //exclusao mutua entre produtores
  sem_wait(&mutexProd);
  printf("Entrou no Insere() -- in = %d\n", in);
  Buffer[in] = item;
  in++;
  sem_post(&mutexProd);
  //sinaliza um slot cheio
  if(in == N) {
    in = 0;
    sem_post(&bufferCheio);
  }
}

void *produtor(void * arg) {
  int elemento = 0;
  while(1) {
    Insere(elemento+*(int*)arg);
    elemento++;
    sleep();
  }
  pthread_exit(NULL);
}

void *consumidor(void * arg) {
  int* elementos;
  elementos = (int*) malloc(sizeof(int) * N);
  while(1) {
    Retira(elementos);
    printf("Consumidor retirando id = %d\n", *(int*)arg);
    for(int i = 0; i < N; i++) printf("%d%c\n", elementos[i], i == N - 1 ? '\n' : ' ');
    sleep();
  }
  free(elementos);
  pthread_exit(NULL);
}

int main() {
  int NTHREADS = Prod + Cons;
  pthread_t tid[NTHREADS];
  int *id[NTHREADS], i;

  for (i=0; i<NTHREADS; i++) {
    if ((id[i] = malloc(sizeof(int))) == NULL) {
      pthread_exit(NULL); return 1;
    }
    *id[i] = i+1;
  }

  //inicia os semaforos
  sem_init(&mutexCons, 0, 1); // Semáforo binário, para exclusão mútua
  sem_init(&mutexProd, 0, 1); // Semáforo binário, para exclusão mútua
  sem_init(&bufferCheio, 0, 0);
  sem_init(&slotVazio, 0, N);

  //cria as tres threads
  for(i = 0; i < Prod; i++) {
    if (pthread_create(&tid[i], NULL, produtor, (void *)id[i])) { printf("--ERRO: pthread_create()\n"); exit(-1); }
  }

  for(; i < Prod + Cons; i++) {
    if (pthread_create(&tid[i], NULL, consumidor, (void *)id[i])) { printf("--ERRO: pthread_create()\n"); exit(-1); }
  }
  //--espera todas as threads terminarem
  for (i=0; i<NTHREADS; i++) {
    if (pthread_join(tid[i], NULL)) {
      printf("--ERRO: pthread_join() \n"); exit(-1); 
    } 
    free(id[i]);
  } 
  pthread_exit(NULL);

  return 0;
}
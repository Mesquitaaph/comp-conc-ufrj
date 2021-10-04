#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
// #include <semaphore.h>

#define NTHREADS 4
#define N 4

sem_t mutexProd, mutexCons; // Exclusão mútua
sem_t bufferCheio, slotVazio; // Condição
int Buffer[N];

void Retira (int* elementos) {
  int out=0;
  // Espera encher o buffer
  sem_wait(&bufferCheio);
  //exclusao mutua entre consumidores
  sem_wait(&mutexCons);
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
  Buffer[in] = item;
  in = (in + 1) % N;
  sem_post(&mutexProd);
  //sinaliza um slot cheio
  if(in == N - 1) {
    in = 0;
    sem_post(&bufferCheio);
  }
}

void *produtor(void * arg) {
  int elemento = 0;
  while(1) {
    Insere(elemento);
    elemento++;
  }
  pthread_exit(NULL);
}

void *consumidor(void * arg) {
  int* elementos;
  elementos = (int*) malloc(sizeof(int) * N);
  while(1) {
    Retira(elementos);
    for(int i = 0; i < N; i++) printf("%d%c\n", elementos[i], i == N - 1 ? '\n' : ' ');
  }
  pthread_exit(NULL);
}

int main() {
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
  sem_init(&slotVazio, 0, NTHREADS);

  //cria as tres threads
  if (pthread_create(&tid[1], NULL, welcome, (void *)id[1])) { printf("--ERRO: pthread_create()\n"); exit(-1); }
  if (pthread_create(&tid[0], NULL, staying, (void *)id[0])) { printf("--ERRO: pthread_create()\n"); exit(-1); }
  if (pthread_create(&tid[3], NULL, staying, (void *)id[3])) { printf("--ERRO: pthread_create()\n"); exit(-1); }
  if (pthread_create(&tid[2], NULL, goodbye, (void *)id[2])) { printf("--ERRO: pthread_create()\n"); exit(-1); }

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
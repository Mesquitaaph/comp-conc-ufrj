#include<stdio.h>
#include<stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NTHREADS 4
// T1 - “Fique a vontade.”
// T2 - “Seja bem-vindo!”
// T3 - “Volte sempre!”
// T4 - “Sente-se por favor.”

// Variaveis globais
int msgs = 2; // Número de mensagens a serem impressas depois da T1 e antes da T3
sem_t em, cond_stay, cond_goodbye;     //semaforos para sincronizar a ordem de execucao das threads

// Thread 2 - Mensagem: "Seja bem-vindo!"
void* welcome(void* args) {
  printf("Seja bem-vindo!\n");

  // Libera as próximas duas execuções de staying()
  sem_post(&cond_stay);
  sem_post(&cond_stay);

  pthread_exit(NULL);
}

void* staying(void* args) {
  int tid = *(int*) args;

  sem_wait(&cond_stay); // Espera a welcome() executar

  if(tid == 1) printf("Fique a vontade.\n");
  else printf("Sente-se por favor.\n");

  sem_wait(&em); // Entrada da seção crítica
  msgs--;
  if(msgs == 0) sem_post(&cond_goodbye); // Se não há mais mensagens, libera a goodbye()
  sem_post(&em); // Saída da seção crítica

  pthread_exit(NULL);
}

void* goodbye(void* args) {
  sem_wait(&cond_goodbye); // Espera todas as staying() executarem
  printf("Volte sempre!\n");
  
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
  sem_init(&em, 0, 0); // Semáforo binário, para exclusão mútua
  sem_init(&cond_stay, 0, 0);
  sem_init(&cond_goodbye, 0, 0);

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
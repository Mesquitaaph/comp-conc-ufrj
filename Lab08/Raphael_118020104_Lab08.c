/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Módulo 3 - Laboratório: 8 */
/* Codigo: Ordem de execução das threads controlada por semáforos */
/* Aluno: Raphael Mesquita */ 
/* DRE: 118020104 */ 

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NTHREADS 4

// Variaveis globais
int msgs = 2; // Número de mensagens a serem impressas depois da T2 e antes da T3

sem_t em; // Semáforo binário, para exclusão mútua
sem_t cond_stay, cond_goodbye; // Semáforos para sincronizar a ordem de execução das threads

// Thread 2 - Mensagem: "Seja bem-vindo!"
void* welcome(void* args) {
  printf("Seja bem-vindo!\n");

  // Libera as próximas duas execuções de staying()
  sem_post(&cond_stay);
  sem_post(&cond_stay);

  pthread_exit(NULL);
}

// Thread 1 - Mensagem: "Fique a vontade."
// Thread 4 - Mensagem: "Sente-se por favor."
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

// Thread 3 - Mensagem: "Volte sempre!"
void* goodbye(void* args) {
  sem_wait(&cond_goodbye); // Espera todas as staying() executarem

  printf("Volte sempre!\n");
  
  pthread_exit(NULL);
}

int main() {
  pthread_t tid[NTHREADS];
  int *id[NTHREADS], i;

  // Preenchendo os identificadores das threads
  for (i = 0; i < NTHREADS; i++) {
    if ((id[i] = malloc(sizeof(int))) == NULL) {
      pthread_exit(NULL); return 1;
    }
    *id[i] = i+1;
  }

  // Inicia os semáforos
  sem_init(&em, 0, 1);
  sem_init(&cond_stay, 0, 0);
  sem_init(&cond_goodbye, 0, 0);

  // Criando as threads
  if (pthread_create(&tid[1], NULL, welcome, (void *)id[1])) { printf("ERRO!! --> pthread_create()\n"); exit(-1); }
  if (pthread_create(&tid[0], NULL, staying, (void *)id[0])) { printf("ERRO!! --> pthread_create()\n"); exit(-1); }
  if (pthread_create(&tid[3], NULL, staying, (void *)id[3])) { printf("ERRO!! --> pthread_create()\n"); exit(-1); }
  if (pthread_create(&tid[2], NULL, goodbye, (void *)id[2])) { printf("ERRO!! --> pthread_create()\n"); exit(-1); }

  // Esperando todas as threads terminarem
  for (i = 0; i < NTHREADS; i++) {
    if (pthread_join(tid[i], NULL)) {
      printf("ERRO!! --> pthread_join()\n");
      return -1; 
    } 
    free(id[i]);
  } 
  pthread_exit(NULL);
}
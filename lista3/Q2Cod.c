#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

int n=0; sem_t s, d; //s inicializado com 1 e d inicializado com 0

void sleep() {
  for(long int i = 0; i < 100000000; i++) {}
}

void *cons(void *args) { 
  int item; 
  while(1) { 
    sem_wait(&d); 
    sem_wait(&s); 
    // retira_item(&item); 
    n--; 
    printf("Cons n = %d\n", n);
    sem_post(&s); 
    // consome_item(item); 
    sleep();
    sleep();
  }
}

void *prod(void *args) {
  int item = 0;
  while(item < 100) {
    // produz_item(&item);
    sem_wait(&s);
    // insere_item(item);
    n++;
    printf("Prod n = %d\n", n);
    sem_post(&d);
    sem_post(&s);
    item++;
    sleep();
  }
}

int main() {
  int Cons = 1, Prod = 1;
  int NTHREADS = Cons + Prod;
  pthread_t tid[NTHREADS];
  int *id[NTHREADS], i;

  for (i=0; i<NTHREADS; i++) {
    if ((id[i] = malloc(sizeof(int))) == NULL) {
      pthread_exit(NULL); return 1;
    }
    *id[i] = i+1;
  }

  //inicializacoes que devem ser feitas na main() antes da criacao das threads
  sem_init(&s, 0, 1);
  sem_init(&d, 0, 0);

  // Cria as produtoras threads
  for(i = 0; i < Prod; i++) {
    if (pthread_create(&tid[i], NULL, prod, (void *)id[i])) { printf("ERRO! --> pthread_create()\n"); exit(-1); }
  }

  // Cria as produtoras threads
  for(; i < Prod+Cons; i++) {
    if (pthread_create(&tid[i], NULL, cons, (void *)id[i])) { printf("ERRO! --> pthread_create()\n"); exit(-1); }
  }

  // Espera todas as threads terminarem
  for (i=0; i<NTHREADS; i++) {
    if (pthread_join(tid[i], NULL)) { printf("ERRO! --> pthread_join() \n"); exit(-1); } 
    free(id[i]);
  } 
  pthread_exit(NULL);

  return 0;
}
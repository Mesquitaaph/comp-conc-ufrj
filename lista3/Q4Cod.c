#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

//variaveis internas
sem_t s, x, h; int aux = 0, n = 0, notf = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t em = PTHREAD_MUTEX_INITIALIZER;

void sleep() {
  long int delay = 100000000;
  for(long int i = 0; i < delay; i++) {}
}


void wait() {
  //pre-condicao: a thread corrente detem o lock de ’m’
  sem_wait(&x);
  aux++;
  sem_post(&x);
  // pthread_mutex_unlock(&m);
  sem_wait(&h);
  sem_post(&s);
  // pthread_mutex_lock(&m);
}

void notify() {
  sem_wait(&x);
  if (aux > 0) {
    aux--;
    sem_post(&h);
    sem_wait(&s);
  }
  sem_post(&x);
}

void notifyAll() {
  sem_wait(&x);
  for (int i = 0; i < aux; i++)
    sem_post(&h);
  while (aux > 0) {
    aux--;
    sem_wait(&s);
  }
  sem_post(&x);
}

void *A() {
  sleep();
  while(1) {
    n++;
    notifyAll();

    wait();
  }
  pthread_exit(NULL);
}

void *B() {
  while(1) {
    wait();
    printf("n = %d\n", n);
    pthread_mutex_lock(&em);
    if(!notf) {
      notf = 1;
    } else {
      notf = 0;
      notify();
    }
    pthread_mutex_unlock(&em);
  }
  pthread_exit(NULL);
}

void *C() {
  while(1) {
    wait();
    printf("n = %d\n", n);
    pthread_mutex_lock(&em);
    if(!notf) {
      notf = 1;
    } else {
      notf = 0;
      notify();
    }
    pthread_mutex_unlock(&em);
  }
  pthread_exit(NULL);
}

int main() {
  int As = 1, Bs = 1, Cs = 1;
  int NTHREADS = As + Bs + Cs;
  pthread_t tid[NTHREADS];
  int *id[NTHREADS], i;

  for (i=0; i<NTHREADS; i++) {
    if ((id[i] = malloc(sizeof(int))) == NULL) {
      pthread_exit(NULL); return 1;
    }
    *id[i] = i+1;
  }

  //inicializacoes feitas na funcao principal
  sem_init(&s,0,0);
  sem_init(&x,0,1);
  sem_init(&h,0,0);

  // Cria as produtoras threads
  for(i = 0; i < As; i++) {
    if (pthread_create(&tid[i], NULL, A, (void *)id[i])) { printf("ERRO! --> pthread_create()\n"); exit(-1); }
  }

  // Cria as produtoras threads
  for(; i < As+Bs; i++) {
    if (pthread_create(&tid[i], NULL, B, (void *)id[i])) { printf("ERRO! --> pthread_create()\n"); exit(-1); }
  }

  // Cria as produtoras threads
  for(; i < As+Bs+Cs; i++) {
    if (pthread_create(&tid[i], NULL, C, (void *)id[i])) { printf("ERRO! --> pthread_create()\n"); exit(-1); }
  }

  // Espera todas as threads terminarem
  for (i=0; i<NTHREADS; i++) {
    if (pthread_join(tid[i], NULL)) { printf("ERRO! --> pthread_join() \n"); exit(-1); } 
    free(id[i]);
  } 
  pthread_exit(NULL);
}
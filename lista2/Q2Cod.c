#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Variaveis globais */
int bloqueadas = 0;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;
int nthreads;
int* vetor;

void printNumeros(int* v) {
  for(int i = 0; i < nthreads; i++) printf("%d ", v[i]);
  printf("\n");
}

int FazAlgo(long long int contador) {
  // printf("%lld\n", contador);
  int t = 2;
  return t+2;
}

long long int contador=0;

void * T1 (void * t) {
  int delay = 100000000;
  for(int i = 0; i < delay; i++) {}

  delay = 1000000;
  while(1) {
    pthread_mutex_lock(&x_mutex);
    FazAlgo(contador);
    contador++;
    if(contador % 100 == 0) {
      pthread_cond_signal(&x_cond);
      pthread_cond_wait(&x_cond, &x_mutex);
    }
    pthread_mutex_unlock(&x_mutex);
    // for(int i = 0; i < delay; i++) {}
  }
  pthread_exit(NULL);
}

void * T2 () {
  while(1) {
    pthread_mutex_lock(&x_mutex);
    pthread_cond_wait(&x_cond, &x_mutex);
    printf("%lld\n", contador);
    pthread_cond_signal(&x_cond);
    pthread_mutex_unlock(&x_mutex);
  }
  pthread_exit(NULL);
}

int main() {
  nthreads = 2;
  pthread_t threads[nthreads];

  int id[nthreads];

  /* Inicializa o mutex (lock de exclusao mutua) e a variavel de condicao */
  pthread_mutex_init(&x_mutex, NULL);
  pthread_cond_init (&x_cond, NULL);

  /* Inicia as variaveis globais e carrega o vetor de entrada */
  vetor = (int*) malloc(nthreads * sizeof(nthreads));

  /* Cria as threads */
  pthread_create(&threads[0], NULL, T1, (void *) &id[0]);
  pthread_create(&threads[1], NULL, T2, NULL);

  /* Espera todas as threads completarem */
  for (int i = 0; i < nthreads; i++) {
    pthread_join(threads[i], NULL);
  }

  return 0;
}
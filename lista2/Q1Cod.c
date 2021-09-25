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
int* somaSeq;

void printNumeros(int* v) {
  for(int i = 0; i < nthreads; i++) printf("%d ", v[i]);
  printf("\n");
}

void somaSequencial() {
  for(int i=1; i<nthreads; i++) somaSeq[i] = somaSeq[i] + somaSeq[i-1];
}

/* Funcao barreira */
void barreira(int nthreads) {
  pthread_mutex_lock(&x_mutex);
  if (bloqueadas == (nthreads-1)) {
    //ultima thread a chegar na barreira
    pthread_cond_broadcast(&x_cond);
    bloqueadas=0;
  } else {
    bloqueadas++;
    pthread_cond_wait(&x_cond, &x_mutex);
  }
  pthread_mutex_unlock(&x_mutex);
}

/* Funcao das threads */
void *tarefa (void *arg) {
  int id = *(int*)arg;
  int salto;
  int aux;
  for(salto=1; salto<nthreads; salto*=2) {
    if(id >= salto) {
      aux = vetor[id-salto];
      barreira(nthreads-salto);
      vetor[id] = aux + vetor[id];
      barreira(nthreads-salto);
    } else break;
  }
  pthread_exit(NULL);
}
// 0,   1,      2,        3,          4,            5,              6,                7,                  8,                    9
// 0, 0+1,    1+2,      2+3,        3+4,          4+5,            5+6,              6+7,                7+8,                  8+9
// 0, 0+1,  0+1+2,  0+1+2+3,    1+2+3+4,      2+3+4+5,        3+4+5+6,          4+5+6+7,            5+6+7+8,              6+7+8+9
// 0, 0+1,  0+1+2,  0+1+2+3,  0+1+2+3+4,  0+1+2+3+4+5,  0+1+2+3+4+5+6,  0+1+2+3+4+5+6+7,    1+2+3+4+5+6+7+8,      2+3+4+5+6+7+8+9
// 0, 0+1,  0+1+2,  0+1+2+3,  0+1+2+3+4,  0+1+2+3+4+5,  0+1+2+3+4+5+6,  0+1+2+3+4+5+6+7,  0+1+2+3+4+5+6+7+8,  0+1+2+3+4+5+6+7+8+9

int main() {
  nthreads = 10;
  pthread_t threads[nthreads];

  int id[nthreads];

  /* Inicializa o mutex (lock de exclusao mutua) e a variavel de condicao */
  pthread_mutex_init(&x_mutex, NULL);
  pthread_cond_init (&x_cond, NULL);

  /* Recebe os parametros de entrada (tamanho do vetor == n ́umero de threads) */

  /* Inicia as variaveis globais e carrega o vetor de entrada */
  vetor = (int*) malloc(nthreads * sizeof(nthreads));
  somaSeq = (int*) malloc(nthreads * sizeof(nthreads));

  srand(time(NULL));
  for(int i = 0; i < nthreads; i++) {
    vetor[i] = (rand() % 10) * (rand() % 2 ? 1 : -1);
    somaSeq[i] = vetor[i];
  }
  printNumeros(vetor);

  /* Cria as threads */
  for(int i=0;i<nthreads;i++) {
    id[i]=i;
    pthread_create(&threads[i], NULL, tarefa, (void *) &id[i]);
  }

  /* Espera todas as threads completarem */
  for (int i = 0; i < nthreads; i++) {
    pthread_join(threads[i], NULL);
  }

  somaSequencial();
  printNumeros(somaSeq);
  printNumeros(vetor);

  /* Armazena o vetor de saida, libera variaveis e encerra */
  free(vetor);
  free(somaSeq);
  pthread_mutex_destroy(&x_mutex);
  pthread_cond_destroy(&x_cond);

  return 0;
}
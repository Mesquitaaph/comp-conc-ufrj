#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

//globais
int a=0, b=0, c=0; //numero de threads A, B e C usando o recurso, respectivamente
sem_t emA, emB, emC; //semaforos para exclusao mutua
sem_t rec; //semaforo para sincronizacao logica

//funcao executada pelas As
void *A () {
  while(1) {
    sem_wait(&emA);
    a++;
    if(a==1) {
      sem_wait(&rec);
    }
    printf("Entrou A --> a = %d\n", a-1);
    sem_post(&emA);
    //SC: usa o recurso
    sem_wait(&emA);
    printf("Saindo A --> a = %d\n", a);
    a--;
    if(a==0) sem_post(&rec);
    sem_post(&emA);
  }
}
//funcao executada pelas Bs
void *B () {
  while(1) {
    sem_wait(&emB);
    b++;
    if(b==1) {
      sem_wait(&rec);
    }
    printf("Entrou B --> b = %d\n", b-1);
    sem_post(&emB);
    //SC: usa o recurso
    sem_wait(&emB);
    printf("Saindo B --> b = %d\n", b);
    b--;
    if(b==0) sem_post(&rec);
    sem_post(&emB);
  }
}

//funcao executada pelas Cs
void *C () {
  while(1) {
    sem_wait(&emC);
    c++;
    if(c==1) {
      sem_wait(&rec);
    }
    printf("Entrou C --> c = %d\n", c-1);
    sem_post(&emC);
    //SC: usa o recurso
    sem_wait(&emC);
    printf("Saindo C --> c = %d\n", c);
    c--;
    if(c==0) sem_post(&rec);
    sem_post(&emC);
  }
}

int main() {
  int As = 4, Bs = 3, Cs = 1;
  int NTHREADS = As + Bs + Cs;
  pthread_t tid[NTHREADS];
  int *id[NTHREADS], i;

  for (i=0; i<NTHREADS; i++) {
    if ((id[i] = malloc(sizeof(int))) == NULL) {
      pthread_exit(NULL); return 1;
    }
    *id[i] = i+1;
  }

  //inicializacoes que devem ser feitas na main() antes da criacao das threads
  sem_init(&emA, 0, 1);
  sem_init(&emB, 0, 1);
  sem_init(&emC, 0, 1);
  sem_init(&rec, 0, 2);

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

  return 0;
}
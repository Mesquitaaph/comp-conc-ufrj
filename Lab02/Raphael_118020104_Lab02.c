/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Módulo 1 - Laboratório: 2 */
/* Codigo: */
/* Aluno: Raphael Mesquita */ 
/* DRE: 118020104 */ 

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

typedef struct {
  int id;
} tArgs;

int dim;
int n_threads;

int* mtx_ref;

int* mtx_A;
int* mtx_B;
int* mtx_C;

void* multMatrix(void* args) {
  tArgs* index = (tArgs*)args;

  for(int i = index->id; i < dim; i += n_threads) {
    for(int j = 0; j < dim; j++) {
      for(int k = 0; k < dim; k++) {
        mtx_C[i*dim + j] += mtx_A[i*dim + k] * mtx_B[j + k*dim];
      }
    }
  }
  pthread_exit(NULL);
}

void multMatrixSeqRef() {
  for(int i = 0; i < dim; i++) {
    for(int j = 0; j < dim; j++) {
      for(int k = 0; k < dim; k++) {
        mtx_ref[i*dim + j] += mtx_A[i*dim + k] * mtx_B[j + k*dim];
      }
    }
  }
}

void compareMtxThreadAndSeq() {
  for(int i = 0; i < dim; i++) {
    for(int j = 0; j < dim; j++) {
      if(mtx_ref[i*dim + j] != mtx_C[i*dim + j]) {
        printf("Multiplicao errada de matrizes\n");
        return;
      }
    }
  }
  printf("Multiplicao certa de matrizes\n");
}

int main(int argc, char* args[]) {
  int isSeq;
  if(argc < 2) {
    printf("Digite: %s <dimensao da matriz> <numero de threads>\n", args[0]);
    return 1;
  }
  if(argc == 2) {
    dim = atoi(args[1]);
    isSeq = 1;
    printf("Iniciando multiplicacao sequencial de matrizes com dim = %d\n", dim);
  }
  if(argc == 3) {
    dim = atoi(args[1]);
    n_threads = atoi(args[2]);
    isSeq = 0;
    printf("Iniciando multiplicacao de matrizes com dim = %d e %d threads\n", dim, n_threads);
  }

  double start1, end1, elapsed1;
  double start2, end2, elapsed2;
  double start3, end3, elapsed3;

  GET_TIME(start1);
  pthread_t* tid;
  tArgs* t_args;

  tid = (pthread_t*)malloc(sizeof(pthread_t) * n_threads);
  t_args = (tArgs*)malloc(sizeof(tArgs) * n_threads);

  mtx_A = (int*)malloc(sizeof(int) * dim * dim);
  mtx_B = (int*)malloc(sizeof(int) * dim * dim);
  mtx_C = (int*)malloc(sizeof(int) * dim * dim);
  mtx_ref = (int*)malloc(sizeof(int) * dim * dim);

  if(
      tid == NULL || t_args == NULL ||
      mtx_A == NULL || mtx_B == NULL ||
      mtx_C == NULL || mtx_ref == NULL
    ) { printf("ERRO-malloc\n"); return 2; }

  for(int i = 0; i < dim; i++) {
    for(int j = 0; j < dim; j++) {
      if(i == j) {
        mtx_A[i*dim + j] = 1;
        mtx_B[i*dim + j] = 1;
      } else {
        mtx_A[i*dim + j] = 1;
        mtx_B[i*dim + j] = 1;
      }
      mtx_C[i*dim + j] = 0;
    }
  }
  GET_TIME(end1);
  elapsed1 = end1 - start1;
  printf("Inicializacao das estruturas de dados durou %lf segundos\n", elapsed1);

  if(isSeq) {
    GET_TIME(start2);
    multMatrixSeqRef();
    GET_TIME(end2);

    elapsed2 = end2 - start2;
    printf("Multiplicacao sequencial durou %lf segundos\n", elapsed2);
  } else {
    GET_TIME(start2);
    for(int i = 0; i < n_threads; i++) {
      t_args[i].id = i;
      if(pthread_create(tid+i, NULL, multMatrix, (void*) (t_args+i))) { printf("Erro-pthread_create\n"); return 3;}
    }

    for(int i = 0; i < n_threads; i++) {
      if(pthread_join(*(tid+i), NULL)) { printf("Erro-pthread_join\n"); return 3; }
    }
    GET_TIME(end2);
    elapsed2 = end2 - start2;

    printf("Multiplicacao multithread durou %lf segundos\n", elapsed2);
  }

  GET_TIME(start3);

  free(mtx_ref);
  free(mtx_A);
  free(mtx_B);
  free(mtx_C);
  free(tid);
  free(t_args);

  GET_TIME(end3);

  elapsed3 = end3 - start3;
  printf("Finalização do programa durou %lf segundos\n", elapsed3);

  double total = elapsed1+elapsed2+elapsed3;
  printf("Programa durou um total de %lf segundos\n\n", total);

  return 0;
}
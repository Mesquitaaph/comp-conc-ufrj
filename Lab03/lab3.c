/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Módulo 1 - Laboratório: 3 */
/* Codigo:  */
/* Aluno: Raphael Mesquita */ 
/* DRE: 118020104 */ 

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

typedef struct {
  float menor;
  float maior;
} t_result;

int N_THREADS;
long long int N;
float *vetor;

void initVetor() {
  double now;
  for(long long int i = 0; i < N; i++) {
    GET_TIME(now);
    long int randomNumber = ((long int)(now*1000000000))%10000;
    vetor[i] = randomNumber / 1000.0;
  }
}

t_result achaMenorMaiorSeq() {
  t_result resultadoSeq;
  resultadoSeq.maior = -1.0;
  resultadoSeq.menor = -1.0;

  for(long long int i = 0; i < N; i++) {
    printf("%f ", vetor[i]);
    if(resultadoSeq.menor == -1.0) {
      resultadoSeq.menor = vetor[i];
    } else {
      if(resultadoSeq.menor > vetor[i]) {
        resultadoSeq.menor = vetor[i];
      }
    }

    if(resultadoSeq.maior == -1.0) {
      resultadoSeq.maior = vetor[i];
    } else {
      if(resultadoSeq.maior < vetor[i]) {
        resultadoSeq.maior = vetor[i];
      }
    }
  }

  return resultadoSeq;
}

void* achaMenorMaiorConc(void* args) {
  long long int id = (long long int) args;
  t_result* resultadoThread;
  resultadoThread = (t_result*)malloc(sizeof(t_result));
  resultadoThread->maior = -1.0;
  resultadoThread->menor = -1.0;

  long long int tamBloc = N/N_THREADS;
  long long int ini = id * tamBloc;
  long long int fim;
  if(id == N_THREADS - 1) fim = N; 
  else fim = ini + tamBloc;

  for(long long int i = ini; i < fim; i++) {
    printf("%f ", vetor[i]);
    if(resultadoThread->menor == -1.0) {
      resultadoThread->menor = vetor[i];
    } else {
      if(resultadoThread->menor > vetor[i]) {
        resultadoThread->menor = vetor[i];
      }
    }

    if(resultadoThread->maior == -1.0) {
      resultadoThread->maior = vetor[i];
    } else {
      if(resultadoThread->maior < vetor[i]) {
        resultadoThread->maior = vetor[i];
      }
    }
  }
  pthread_exit((void*) resultadoThread);
}

int main(int argc, char* args[]) {
  if(argc < 3) {
    fprintf(stderr, "Digite: %s <numero de elementos> <numero de threads>\n", args[0]);
    return 1;
  }

  N = atoll(args[1]);
  N_THREADS = atoi(args[2]);

  vetor = (float *)malloc(sizeof(float) * N);
  if(vetor == NULL) {
    fprintf(stderr, "ERRO-malloc\n"); return 2;
  }
  initVetor();

  pthread_t* tid;
  tid = (pthread_t*)malloc(sizeof(pthread_t) * N_THREADS);

  t_result* resultadoThread;

  t_result resultadoConc;
  resultadoConc.maior = -1.0;
  resultadoConc.menor = -1.0;

  for(long int i = 0; i < N_THREADS; i++) {
    if(pthread_create(tid+i, NULL, achaMenorMaiorConc, (void*)i)) { printf("Erro-pthread_create\n"); return 3;}
  }

  for(int i = 0; i < N_THREADS; i++) {
    if(pthread_join(*(tid+i), (void**)&resultadoThread)) { printf("Erro-pthread_create\n"); return 3;}
    if(resultadoConc.menor == -1.0 || resultadoThread->menor < resultadoConc.menor) {
      resultadoConc.menor = resultadoThread->menor;
    }
    if(resultadoConc.maior == -1.0 || resultadoThread->maior > resultadoConc.maior) {
      resultadoConc.maior = resultadoThread->maior;
    }

    free(resultadoThread);
  }

  t_result resultadoSeq = achaMenorMaiorSeq();
  
  printf("\n");

  printf("Maior: %f\nMenor: %f\n", resultadoSeq.maior, resultadoSeq.menor);
  printf("Maior: %f\nMenor: %f\n", resultadoConc.maior, resultadoConc.menor);

  return 0;
}
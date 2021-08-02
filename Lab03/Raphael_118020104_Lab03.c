/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Módulo 1 - Laboratório: 3 */
/* Codigo: Encontrado o menor e maior valor de um vetor com uma ou mais threads */
/* Aluno: Raphael Mesquita */ 
/* DRE: 118020104 */ 

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

// Numero de execucoes para calcular a media de tempo
// para a forma sequencial e concorrente do programa
#define NUM_EXECS 10

// Struct que contém o necessário para passar para cada thread.
typedef struct {
  float menor;
  float maior;
} t_result;

int N_THREADS; // Variável global para guardar o número de threads
long long int N; // Variável global para guardar a dimensão do vetor
float *vetor; // Variável global que guarda o vetor utilizado no programa

// Função que inicializa o vetor com valores
// pseudo aleatórios e positivos
void initVetor() {
  for(long long int i = 0; i < N; i++) {
    vetor[i] = 1000.0/(i+1);
  }
}

// Função que acha o menor e o maior valor no vetor global
t_result achaMenorMaiorSeq() {
  t_result resultadoSeq;
  resultadoSeq.maior = -1.0;
  resultadoSeq.menor = -1.0;

  for(long long int i = 0; i < N; i++) {
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

// Função que acha o menor e o maior valor no vetor global.
// Cada thread vai verificar uma parte do vetor e retornar
// seu resultado.
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
    if(resultadoThread->menor == -1.0 || resultadoThread->menor > vetor[i]) {
      resultadoThread->menor = vetor[i];
    } 

    if(resultadoThread->maior == -1.0 || resultadoThread->maior < vetor[i]) {
      resultadoThread->maior = vetor[i];
    }
  }
  pthread_exit((void*) resultadoThread);
}

// Função principal do programa
int main(int argc, char* args[]) {
  if(argc < 3) { // Verifica se tem a quantidade mínima de argumentos
    fprintf(stderr, "Digite: %s <numero de elementos> <numero de threads>\n", args[0]);
    return 1;
  }

  N = atoll(args[1]);
  N_THREADS = atoi(args[2]);

  vetor = (float *)malloc(sizeof(float) * N);
  if(vetor == NULL) {
    fprintf(stderr, "ERRO-malloc\n"); return 2;
  }
  initVetor(); // Inicializa o vetor com tamanho N

  // Inicialização das estruturas de dados
  pthread_t* tid;
  tid = (pthread_t*)malloc(sizeof(pthread_t) * N_THREADS);

  t_result* resultadoThread;

  t_result resultadoConc;
  resultadoConc.maior = -1.0;
  resultadoConc.menor = -1.0;

  // Declarando as variáveis que vão aguardar o tempo de execução de cada etapa do programa 
  double iniSeq, fimSeq, iniConc, fimConc;
  double mediaSeq = 0.0, mediaConc = 0.0;

  // Primeira execução concorrente para verificar a corretude da solução concorrente
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

  // Primeira execução sequencial para verificar a corretude da solução concorrente
  t_result resultadoSeq = achaMenorMaiorSeq();

  if(resultadoSeq.maior == resultadoConc.maior && resultadoSeq.menor == resultadoConc.menor)
    printf("Resultados iguais\n");
  else
    printf("Resultados diferentes\n");

  // Sequência de execuções concorrentes para calcular a média de tempo decorrido
  for(int i = 0; i < NUM_EXECS; i++) {
    resultadoConc.maior = -1.0;
    resultadoConc.menor = -1.0;
    GET_TIME(iniConc);
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
    GET_TIME(fimConc);

    mediaConc += fimConc - iniConc;
  }

  // Sequência de execuções sequenciais para calcular a média de tempo decorrido
  for(int i = 0; i < NUM_EXECS; i++) {
    GET_TIME(iniSeq);
    achaMenorMaiorSeq();
    GET_TIME(fimSeq);
    mediaSeq += fimSeq - iniSeq;
  }

  // Finalização do programa
  printf("\nMedia Sequencial com dimensao %lld: %lf segundos\n", N, mediaSeq / NUM_EXECS);
  printf("Media Concorrente com dimensao %lld e %d threads: %lf segundos\n", N, N_THREADS, mediaConc / NUM_EXECS);
  printf("Aceleracao foi de %lf\n\n\n", (mediaConc / NUM_EXECS) / (mediaSeq / NUM_EXECS));

  free(tid);
  free(vetor);

  return 0;
}
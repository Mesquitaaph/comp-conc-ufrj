/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Módulo 1 - Laboratório: 1 */
/* Codigo: "Caculando o quadrado de números de forma multi-thread" usando threads em C */
/* Aluno: Raphael Mesquita */ 
/* DRE: 118020104 */ 

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N_THREAD 2
#define N_ELEMENTS 10000

int vector[N_ELEMENTS];
int vectorRef[N_ELEMENTS];

// Função que atribui valores inteiros a elementos de um vetor com N_ELEMENTS elementos.
void initVector(int* vector) {
  for(int i = 0; i < N_ELEMENTS; i++) {
    vector[i] = i / 4;
  }
}

// Função que separa o cálculo do quadrado de elementos de um vetor em duas threads.
// Uma thread irá calcular o quadrado de elementos pares e outra thread irá calcular 
// o quadrado de elementos ímpares.
void* calculateSquare(void* arg) {
  int isEven = *(int*)arg;

  for(int i = 0; i < N_ELEMENTS; i++) {
    int element = vector[i];
    if((isEven && i%2 == 0) || (!isEven && i%2 == 1)) {
      vector[i] = element * element;
    }
  }
  pthread_exit(NULL);
}

// Função principal
int main() {
  pthread_t tid[N_THREAD];
  int parity[N_THREAD];

  initVector(vectorRef);
  initVector(vector);

  for(int i = 0; i < N_THREAD; i++) {
    parity[i] = i % 2 == 0; // Determina a paridade da thread
    
    if(pthread_create(&tid[i], NULL, calculateSquare, (void*)&parity[i])) {
      printf("ERRO pthread_create\n"); exit(-1);
    }
  }

  for(int i = 0; i < N_THREAD; i++) {
    if(pthread_join(tid[i], NULL)) {
      printf("ERRO pthread_join\n"); exit(-1);
    }
  }

  // Verifica se os elementos do vetor utilizado pelos cálculos multi-thread estão iguais
  // ao quadrado elementos do vetor referência.
  int isCorrect = 1;
  for(int i = 0; i < N_ELEMENTS; i++) {
    if(vectorRef[i] * vectorRef[i] != vector[i]) {
      isCorrect = 0;
      break;
    }
  }

  printf("%s\n", isCorrect ? "Calculos certos" : "Calculos errados");
  return 0;
}
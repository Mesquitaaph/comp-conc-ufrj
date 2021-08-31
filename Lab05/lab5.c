/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Módulo 2 - Laboratório: 5 */
/* Codigo: Implementação de soma de inteiros de um vetor com sincronização coletiva com barreira */
/* Aluno: Raphael Mesquita */ 
/* DRE: 118020104 */ 

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Variáveis globais
int N_THREADS;
int* numeros;

int bloqueadas;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;

// Função auxiliar que imprime na saída padrão os inteiros no vetor numeros
void printNumeros() {
  for(int i = 0; i < N_THREADS; i++) printf("%d ", numeros[i]);
  printf("\n");
}

void barreiraSoma(long long int id, int somaLocal) {
  // Seção crítica que soma os inteiros na variavel somaLocal
  pthread_mutex_lock(&x_mutex);

  // Lógica da barreira
  // A última thread a chegar na barreira. Libera todas as threads, incluindo a si mesma, para seguir suas execuções
  if(bloqueadas == N_THREADS-1) {
    printf("Thread id = %lld -> soma = %d (ultima a chegar)\n\n", id, somaLocal);

    pthread_cond_broadcast(&x_cond);
    bloqueadas=0;
  } else {
    bloqueadas++;
    printf("Thread id = %lld -> soma = %d (esperando)\n", id, somaLocal);
    pthread_cond_wait(&x_cond, &x_mutex);
  }

  pthread_mutex_unlock(&x_mutex);
}

void barreiraVetor(long long int id) {
  // Seção crítica que redefine os inteiros do vetor numeros
  pthread_mutex_lock(&x_mutex);

  // Lógica da barreira
  // A última thread a chegar na barreira. Libera todas as threads, incluindo a si mesma, para seguir suas execuções
  if(bloqueadas == N_THREADS-1) {
    printf("Thread id = %lld -> numero gerado = %d (ultima a chegar)\n", id, numeros[id]);
    printf("Novo vetor: "); printNumeros(); printf("\n");

    pthread_cond_broadcast(&x_cond);
    bloqueadas=0;
  } else {
    bloqueadas++;
    printf("Thread id = %lld -> numero gerado = %d (esperando)\n", id, numeros[id]);
    pthread_cond_wait(&x_cond, &x_mutex);
  }
  pthread_mutex_unlock(&x_mutex);
}

// Função única para todas as threads. 
// Soma os inteiros no vetor numeros e redefine, com inteiros aleatorios, seus elementos.
// Esse processo se repete N_THREADS vezes.
void* somaInteiros(void* arg) {
  long long int id = (long long int) arg;
  int* somaLocal = (int*) malloc(sizeof(int));

  for(int i = 0; i < N_THREADS; i++) {
    for(int j = 0; j< N_THREADS; j++) {
      *somaLocal += numeros[j];
    }
    barreiraSoma(id, *somaLocal);

    numeros[id] = id == 0 ? (rand() * (id+1)) % 10 : (rand() * id) % 10;    
    barreiraVetor(id);
  }
  
  printf("Thread id = %lld -> soma = %d\n", id, *somaLocal);
  pthread_exit((void*)somaLocal);
}

// Função principal do programa
int main(int argc, char* args[]) {
  if(argc < 2) { // Verifica se tem a quantidade mínima de argumentos
    fprintf(stderr, "Digite: %s <numero de threads>\n", args[0]);
    return 1;
  }
  N_THREADS = atoi(args[1]);

  pthread_t threads[N_THREADS];
  int** somas = (int**)malloc(sizeof(int*) * N_THREADS); // Vetor que guarda o resultado da soma de cada thread

  // Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao
  pthread_mutex_init(&x_mutex, NULL);
  pthread_cond_init (&x_cond, NULL);

  // Inicialização do vetor de números inteiros
  numeros = (int*)malloc(sizeof(int) * N_THREADS);
  srand(time(NULL));
  for(int i = 0; i < N_THREADS; i++) {
    numeros[i] = rand() % 10;
  }
  printNumeros();

  // Criação das threads
  for(long long int i = 0; i < N_THREADS; i++) {
    if(pthread_create(&threads[i], NULL, somaInteiros, (void*)i)) {
      fprintf(stderr, "Erro pthread_create\n");
      return 2;
    }
  }

  // Espera todas as threads terminarem suas execuções
  for(int i = 0; i < N_THREADS; i++) {
    if(pthread_join(threads[i], (void**)&somas[i])) {
      fprintf(stderr, "Erro pthread_join\n");
      return 2;
    }
  }

  // Verifica se o somatório de todas as threads são iguais
  int somasCorretas = 1;
  for(int i = 1; i < N_THREADS; i++) {
    if(*somas[i-1] != *somas[i]) somasCorretas = 0;
    free(somas[i-1]);
  }

  if(somasCorretas) printf("Somas corretas\n");
  else printf("Erro nas somas\n");

  // Desaloca variáveos e termina o programa
  free(somas[N_THREADS-1]);
  free(somas);

  pthread_mutex_destroy(&x_mutex);
  pthread_cond_destroy(&x_cond);

  free(numeros);
  return 0;
}
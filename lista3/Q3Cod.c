#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

int leitores = 0, escritores = 0, n = 0; // Leitores lendo
sem_t l_mutex, e_mutex, tentaLer, escrita; // Variáveis para exclusão mútua
void sleep() {
  long int delay = 100000000;
  for(long int i = 0; i < 0; i++) {}
}

void *leitor() {
  while(1) {

    sem_wait(&tentaLer);        // Indica que um leitor esta tentando ler

    // Inicio do funcionamento padrao
    sem_wait(&l_mutex);
    leitores++;
    if (leitores == 1)
      sem_wait(&escrita);
    sem_post(&l_mutex);
    // Fim do funcionamento padrao

    sem_post(&tentaLer);        // Indica que a leitura foi liberada

    printf("Lendo n = %d\n", n);

    // Inicio do funcionamento padrao
    sem_wait(&l_mutex);
    leitores--;
    if (leitores == 0)
      sem_post(&escrita);
    sem_post(&l_mutex);
    // Fim do funcionamento padrao
  }
  pthread_exit(NULL);
}

void *escritor(){
  while(1){
    
    // Entra na seção crítica dos escritores
    sem_wait(&e_mutex);
    escritores++;
    // Impede que leitores sejam impedidos de tentar ler
    if (escritores == 1)
      sem_wait(&tentaLer);
    sem_post(&e_mutex);

    // Inicio do funcionamento padrao
    sem_wait(&escrita);

    printf("Escrevendo n = %d para n = %d\n", n, n+1);
    n++;

    sem_post(&escrita);
    // Fim do funcionamento padrao

    // Entra na seção crítica dos escritores
    sem_wait(&e_mutex);
    escritores--;
    // Libera que os leitores tentem ler
    if (escritores == 0)
      sem_post(&tentaLer);
    sem_post(&e_mutex);
  }
  pthread_exit(NULL);
}

int main()  {
  int L = 5, E = 1;
  int NTHREADS = L + E;
  pthread_t tid[NTHREADS];
  int *id[NTHREADS], i;

  for (i=0; i<NTHREADS; i++) {
    if ((id[i] = malloc(sizeof(int))) == NULL) {
      pthread_exit(NULL); return 1;
    }
    *id[i] = i+1;
  }

  sem_init(&l_mutex, 0, 1);
  sem_init(&e_mutex, 0, 1);
  sem_init(&tentaLer, 0, 1);
  sem_init(&escrita, 0, 1);

  // Cria as produtoras threads
  for(i = 0; i < L; i++) {
    if (pthread_create(&tid[i], NULL, leitor, (void *)id[i])) { printf("ERRO! --> pthread_create()\n"); exit(-1); }
  }

  // Cria as produtoras threads
  for(; i < E+L; i++) {
    if (pthread_create(&tid[i], NULL, escritor, (void *)id[i])) { printf("ERRO! --> pthread_create()\n"); exit(-1); }
  }

  // Espera todas as threads terminarem
  for (i=0; i<NTHREADS; i++) {
    if (pthread_join(tid[i], NULL)) { printf("ERRO! --> pthread_join() \n"); exit(-1); } 
    free(id[i]);
  } 
  pthread_exit(NULL);

  return 0;
}
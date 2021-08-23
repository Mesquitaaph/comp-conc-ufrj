/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Módulo 1 - Laboratório: 4 */
/* Codigo: Ordem de execução das threads controlada pela sincronização condicional com variáveis de condição */
/* Aluno: Raphael Mesquita */ 
/* DRE: 118020104 */ 

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N_THREADS  4

/* Variaveis globais */
int x = 0; // Conta o número de mensagens já apresentadas
pthread_mutex_t x_mutex;
pthread_cond_t x_cond_1;
pthread_cond_t x_cond_2;

// Thread 1 - Mensagem: "Fique a vontade."
/* 
 * Apesar de serem praticamente idênticas, decidi manter as funções das threads 1 e 4 separadas,
 * assim há uma discrição mais direta do funcionamento das threads.
 * Obs.: Essa não é uma escolha que afeta o funcionamento do programa, apenas dificulta a manutenção.
 */
void *thread_1(void *arg) {
  pthread_mutex_lock(&x_mutex);

  // Condicional apenas se a mensagem "Seja bem-vindo!" ainda não foi apresentada
  if(x == 0)
    pthread_cond_wait(&x_cond_1, &x_mutex);
  x++;
  printf("Fique a vontade.\n");

  // Se ja foram apresentadas as 3 mensagens iniciais, libera a thread 3 para apresentar
  // a mensagem "Volte sempre!"
  if(x == 3) {
    pthread_cond_signal(&x_cond_2);
  }

  pthread_mutex_unlock(&x_mutex);
  pthread_exit(NULL);
}

// Thread 2 - Mensagem: "Seja bem-vindo!"
void *thread_2(void *arg) {
  int tempo1, tempo2;

  printf("Seja bem-vindo!\n");

  // Gasta tempo para evitar que esta thread execute o lock antes das threads 1 e 4
  tempo1=10000; tempo2=-10000; while (tempo2 < tempo1) tempo2++;
  pthread_mutex_lock(&x_mutex);

  x++;
  // Libera a thread 1 e 4 para apresentar suas mensagens
  pthread_cond_broadcast(&x_cond_1);

  pthread_mutex_unlock(&x_mutex);
  pthread_exit(NULL);
}

// Thread 3 - Mensagem: "Volte sempre!"
void *thread_3(void *arg) {
  pthread_mutex_lock(&x_mutex);

  // Condicional apenas se as mensagens "Fique a vontade." e "Sente-se por favor." 
  // ainda não foram apresentadas
  if(x < 3)
    pthread_cond_wait(&x_cond_2, &x_mutex);
  printf("Volte sempre!\n");

  pthread_mutex_unlock(&x_mutex);
  pthread_exit(NULL);
}

// Thread 4 - Mensagem: "Sente-se por favor."
void *thread_4(void *arg) {
  pthread_mutex_lock(&x_mutex);

  // Condicional apenas se a mensagem "Seja bem-vindo!" ainda não foi apresentada
  if(x == 0)
    pthread_cond_wait(&x_cond_1, &x_mutex);
  x++;
  printf("Sente-se por favor.\n");

  // Se ja foram apresentadas as 3 mensagens iniciais, libera a thread 3 para apresentar
  // a mensagem "Volte sempre!"
  if(x == 3) {
    pthread_cond_signal(&x_cond_2);
  }

  pthread_mutex_unlock(&x_mutex); 

  pthread_exit(NULL);  
}

int main() {
  int i;
  pthread_t threads[N_THREADS];

  /* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao */
  pthread_mutex_init(&x_mutex, NULL);
  pthread_cond_init (&x_cond_1, NULL);
  pthread_cond_init (&x_cond_2, NULL);

  /* Cria as threads */
  pthread_create(&threads[1], NULL, thread_2, NULL);
  pthread_create(&threads[0], NULL, thread_1, NULL);
  pthread_create(&threads[3], NULL, thread_4, NULL);
  pthread_create(&threads[2], NULL, thread_3, NULL);

  /* Espera todas as threads completarem */
  for (i = 0; i < N_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  /* Desaloca variaveis e termina */
  pthread_mutex_destroy(&x_mutex);
  pthread_cond_destroy(&x_cond_1);
  pthread_cond_destroy(&x_cond_2);
}
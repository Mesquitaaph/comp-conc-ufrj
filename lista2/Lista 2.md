### Questão 2:
```c
/* Variaveis globais */
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;

long long int contador=0;
void * T1 (void * t) {
	/* 	O laço é para o programa atrasar o início da T1 e
		garantir que a T2 sempre inicie primeiro */
	int delay = 100000000;
	for(int i = 0; i < delay; i++) {}

	while(1) {
		pthread_mutex_lock(&x_mutex);
		FazAlgo(contador);
		contador++;

		/* 	Quando o contador é um múltiplo de 100,
			envia um sinal para a T2 e aguarda ela imprimir
			o valor do contador.
			Essa espera é necessária, pois se a T1 continuar
			sua execução, ela pode incrementar o contador
			antes da T2 imprimir o valor da variável.
			Isto faz a T2 imprimir valores que podem não
			serem valores múltiplos de 100.
		*/
		if(contador % 100 == 0) {
			pthread_cond_signal(&x_cond);
			pthread_cond_wait(&x_cond, &x_mutex);
		}
		pthread_mutex_unlock(&x_mutex);
	}
	pthread_exit(NULL);
}

/* 	A thread já inicia esperando um sinal para continuar sua execução.
	Por isso o "laço de atraso" na T1.
	Assim que inicia, aguarda o sinal para retomar sua execução.
	Uma vez que recebe o sinal(quando o contador é um múltiplo de 100), 
	imprime o valor do contador, envia um sinal para a T1 retomar sua execução
	e segue para a próxima execução do loop.
*/
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
```
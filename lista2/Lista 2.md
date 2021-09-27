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

### Questão 3:
a) 
<p>

A primeira etapa do programa se dá pela instanciação de um objeto da classe _FilaTarefas_. Quando a instância é criada, as threads são iniciadas. Após isso, com a fila de tarefas vazia, elas aguardam tarefas serem escalonadas, com o método `queue.wait()`.</p>

<p>

Uma vez escalonada, a tarefa entra na fila, com `queue.addLast(r)`, e envia uma notificação,  com `queue.notify()`, à primeira thread aguardando.</p>

<p>

Ao receber a notificação, a thread sai do estado "esperando", verifica se há realmente tarefas na fila e verifica se o programa está encerrando. Com a fila não vazia ou aplicação não encerrando, a sua execução segue normal para a tarefa que realizou a notificação, removendo-a da fila de tarefas. Após completá-la, a thread aguarda novamente outra tarefa.</p>

<p>

Ao encerrar o programa, o método `shutdown()` é chamado. Este altera o atributo privado de _FilaTarefas_ para `true`. Esta condição faz uma thread que recém finalizou uma tarefa, finalize sua execução caso não haja mais tarefas na fila. Assim, quando todas as threads terminam, o programa termina.</p>
 
<br>

b) 
<p>

Apesar da explicação do item anterior mostrar, de fato, o que o programa faz, faltou explicar o que acontece num caso em específico: o de não haver mais tarefas a se executar **antes** do método `shutdown()` ser chamado.</p>

<p>

Nesse caso, a fila de tarefas estará vazia e, por isso, as threads vão aguardar com o `queue.wait()`, necessitando de notificações para serem liberadas. Mas o `shutdown()` não notifica. Isso faz com que o programa não encerre, pois estará sempre aguardando as threads finalizarem, o que não acontecerá.</p>

<p>

Assim, uma forma fácil de resolver este problema é colocar o método `queue.notifyAll()` ao encerrar o programa. Essa mudança faz com que a aplicação libere todas as threads travadas no seu encerramento. Teríamos então:</p>

```c
public void shutdown() {
  synchronized(queue) { this.shutdown=true; queue.notifyAll(); }
  for (int i=0; i<nThreads; i++)
    try { threads[i].join(); }
    catch (InterruptedException e) {return;}
}
```
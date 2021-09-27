**<center>Módulo 2 - Lista de Exercícios 2 (2021/1 REMOTO)</center>**

<center><b>Computação Concorrente (MAB-117)</b></center>
<center><b>Prof. Silvana Rossetto</b></center>

<br>

<center>Instituto de Computação/UFRJ</center>
<center>27 de setembro de 2021</center>
<br>
<center>Raphael Felipe de Cantuaria Mesquita | DRE 118020104</center>
<br>

### Questão 1:
Podemos dividir o programa em três etapas: a função principal, a função das threads, representando o algoritmo concorrente, e a barreira. Num geral, tudo escrito abaixo explica o item “a”. Já os trechos destacados em negrito respondem os outros itens.

1.  Na função principal, basicamente, são iniciadas as variáveis globais, preenchido o vetor de números inteiros e iniciadas as threads, cada uma com um identificador igual à sua ordem de iniciação (primeira thread com *id = 0*, segunda com *id = 1* etc). Depois, o programa espera as threads terminarem suas execuções e, por fim, encerra o programa. Existe apenas **um erro** neste ponto do programa, pois a variável *nthreads* não é iniciada em momento nenhum. **O certo seria sua inicialização ocorrer antes de criar a variável `int id[nthreads]`.** Em seguida, temos o funcionamento do programa considerando que a correção foi aplicada.
    
2.  Quando as threads são iniciadas, é passada para elas, além dos seus identificadores (*id*), a função ``tarefa()`` para executarem. Para cada thread, temos uma iteração de 1 a *nthreads* (número de threads usadas no programa) com um salto de $2^i$, sendo $i$ o número de iterações já realizadas. Ou seja, a primeira iteração tem $i=0$, logo $salto=2^0=1$. A segunda iteração tem $i=1$, logo $salto=2^1=2$ e assim por diante.
    

	Além disso, cada thread é responsável por um índice no vetor. Se uma thread tem *id=3* ela vai ter a tarefa de somar, ao número no índice 3, todos os números antes.

	Para cada iteração, temos dois passos: primeiro se guarda o número que está a $salto$ índices antes do índice da thread no mesmo vetor (se $salto=2$, então é guardado o número que está a 2 índices antes) e depois o soma ao número que está no índice da thread. Como este último passo é um que altera valores no vetor que podem ainda estar sendo lidos por outras threads, este não pode ser executado antes que todas as threads tenham feito o primeiro passo. Logo, é necessário usar a barreira para garantir que todas as threads guardem (leitura) os valores necessários. O mesmo ocorre na transição entre uma iteração e a seguinte. Como o primeiro passo é ler um valor no vetor, é necessário garantir que todas as threads tenham executado o segundo passo (escrita) antes. Assim sendo necessário usar também uma barreira após a escrita. Ou seja, **duas barreiras são necessárias para cada iteração**. **(Este parágrafo explica o item “c”)**

	Detalhando restrições do algoritmo e como funciona sua lógica, temos que um salto pode ser maior que a quantidade de elementos que existem antes da posição de uma thread. Então é necessário verificar antes de cada iteração se $id \geq salto$. Assim, para cada iteração as barreiras vão esperar menos threads. 
	Especificamente falando, para cada thread com $id > 0$, temos 
	#### $n = 1 + \left \lfloor log_2(id) \right \rfloor$ 
	iterações.
	Abaixo temos uma tabela preenchida à risca pelo algoritmo concorrente. Cada linha representa o vetor ao final de cada iteração, onde $v[i]$, $0\leq i<10$ representa um número no vetor:
	
	<center>
	
	![](https://lh5.googleusercontent.com/XSd9Lo1qZjGpFBIO_KllMdJmGKVcRiT1nKkVCveZHEymrqenWhLTboPqYjHaH2Iw3SwKGfB5eALdZNPFMq0LKHSAk7SvMJChHEYyAEGZUkvqtWUt-12kVlSrDChiR-lmnD5UNZdX=s0)

	![](https://lh6.googleusercontent.com/Lfi8Vvw5j1AZM19D1_tuo75wHo1n-Ws4U4TAXdpEY04CnTgCvl-ZBiq8sYCoqb2AdDHEtjCuZ9kqQscwKCsGSCc0Ek16byF4j0owoMd2IezasTCkD7FJrETBYNX4Cqm_mSvjIcln=s0)
	</center>

	Verificando esta tabela, percebemos que, ao final, a soma é realizada **corretamente**, tendo o resultado final idêntico ao da implementação concorrente (aplicando a correção citada no início da explicação). Também **adaptei o código para executar o algoritmo nas duas formas, com o mesmo vetor inicial e o resultado foi o mesmo. (Este parágrafo, junto à correção no início, responde o item “b”)**

3.  O algoritmo concorrente está implementado de uma maneira que as threads com *id* menor terminam sua execução mais rápido que as threads com *id* maior. Assim, para cada vez que a barreira é usada, precisa saber quantas threads devem ser esperadas. Descobrimos essa quantidade com a subtração $nthreads - salto$, que é a quantidade de threads com $id \geq salto$. Uma vez que todas as threads alcançam a barreira, as threads são liberadas a continuarem suas execuções normalmente.
    

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

Ao receber a notificação, a thread sai do estado "esperando", verifica se há realmente tarefas na fila e verifica se o programa está encerrando. Com a fila não vazia ou aplicação não encerrando, a sua execução segue normal para a tarefa que realizou a notificação, removendo-a da fila de tarefas. Após completá-la, a thread recebe ou aguarda novamente outra tarefa.</p>

<p>

Ao encerrar o programa, o método `shutdown()` é chamado. Este altera o atributo privado *shutdown* de _FilaTarefas_ para `true`. Esta condição faz uma thread que recém finalizou uma tarefa, finalize sua execução caso não haja mais tarefas na fila. Assim, quando todas as threads terminam, o programa termina.</p>
 
<br>

b) 
<p>

Apesar da explicação do item anterior mostrar, de fato, o que o programa faz, faltou explicar o que acontece num caso em específico: o de não haver mais tarefas a se executar **antes** do método `shutdown()` ser chamado.</p>

<p>

Nesse caso, a fila de tarefas estará vazia e, por isso, as threads vão aguardar com o `queue.wait()`, necessitando de notificações para serem liberadas. Mas o `shutdown()` não notifica. Isso faz com que o programa não encerre, pois estará sempre aguardando as threads finalizarem, o que não acontecerá.</p>

<p>

Assim, uma forma fácil de resolver este problema é colocar o método `queue.notifyAll()` ao encerrar o programa. Essa mudança faz com que a aplicação libere todas as threads travadas no seu encerramento. Teríamos então:</p>

```java
public void shutdown() {
  synchronized(queue) { this.shutdown=true; queue.notifyAll(); }
  for (int i=0; i<nThreads; i++)
    try { threads[i].join(); }
    catch (InterruptedException e) {return;}
}
```


### Questão 4:
a) Dois são os exemplos que eu pude pensar para ocorrer a inanição:
- Caso a leitura seja muito rápida sempre, a thread leitora pode encerrar sua tarefa e iniciar a seguinte antes mesmo da thread escritora "pedir" para escrever. Isso torna extremamente difícil que a escritura seja feita.
- O outro caso é quando há pelo menos duas threads leitoras(L1 e L2), na qual uma delas(L1) demora muito tempo para ler e a outra(L2) executa sua leitura enquanto a L1 não está lendo. Isso faz com que sempre tenha uma thread lendo. Assim, se uma thread escritora "pedir" para escrever, vai ser bloqueada e permanecerá neste estado até que o tempo de leitura de L1 e L2 de alguma forma se altere, de modo que em algum momento o número de threads lendo seja igual à zero.

b) 
<p>A solução que cheguei me pareceu bem simples. Primeiro eu criei uma variável que representa quantos escritores querem escrever, ou quantos escritores estão na fila.
Depois eu alterei o método de entrada para leitores: se há escritores na fila, então a thread leitora deve ser bloqueada.</p>
<p>Por fim, alterei o método de entrada para escritores: ao ser bloqueado, entra na fila e ao desbloqueio, sai da fila.</p>

<p>

Não foi necessário acrescentar mais nada, pois o próprio método de saída para escritores já possui o `notifyAll()`.</p>

<p>

A implementação foi baseada no padrão apresentado por: 
> Prof. Silvana Rossetto, em *Módulo 2 - Semana 4*

</p>

```java
class LE {
  private int leit, escr, qescr;
 
  // Construtor
  LE() {
    this.leit = 0; //leitores lendo (0 ou mais)
    this.escr = 0; //escritor escrevendo (0 ou 1)
    this.qescr = 0; //escritores querendo escrever (0 ou mais)
  } 

  // Entrada para leitores
  public synchronized void EntraLeitor (int id) {
    try { 
	  while (this.escr > 0 || this.qescr > 0) {
		wait(); //bloqueia pela condicao logica da aplicacao 
	  }
	  this.leit++; //registra que ha mais um leitor lendo
	} catch (InterruptedException e) { }
  }

  // Saida para leitores
  public synchronized void SaiLeitor (int id) {
	this.leit--; //registra que um leitor saiu
	if (this.leit == 0) 
	  this.notify(); //libera escritor (caso exista escritor bloqueado)
  }








  // Entrada para escritores
  public synchronized void EntraEscritor (int id) {
	try { 
	  while ((this.leit > 0) || (this.escr > 0)) {
		this.qescr++; //registra o bloqueio de um escritor
		wait(); //bloqueia pela condicao logica da aplicacao 
		this.qescr--; //registra o desbloqueio de um escritor
	  }
	  this.escr++; //registra que ha um escritor escrevendo
	} catch (InterruptedException e) { }
  }

  // Saida para escritores
  public synchronized void SaiEscritor (int id) {
	this.escr--; //registra que o escritor saiu
	notifyAll(); //libera leitores e escritores
  }
}
```
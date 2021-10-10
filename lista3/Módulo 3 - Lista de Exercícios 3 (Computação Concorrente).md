**<center>Módulo 3 - Lista de Exercícios 3 (2021/1 REMOTO)</center>**

<center><b>Computação Concorrente (MAB-117)</b></center>
<center><b>Prof. Silvana Rossetto</b></center>

<br>

<center>Instituto de Computação/UFRJ</center>
<center>9 de outubro de 2021</center>
<br>
<center>Raphael Felipe de Cantuaria Mesquita | DRE 118020104</center>
<br>

### Questão 1:
a)
<p>As funções que as threads de tipo A, B e C são praticamente idênticas, sendo as únicas diferenças as variáveis de contagem de quantas threads do mesmo tipo estão utilizando o mesmo recurso ao mesmo tempo e o semáforo de exclusão mútua. Podemos entao considerar a *thread A* como a primeira a executar no programa.</p>

Feitas as considerações temos a seguinte sequência de passos: 
A *thread A* entra na primeira seção crítica:
```c
sem_wait(&emA);
a++;
if(a==1) {
	sem_wait(&rec);
}
sem_post(&emA);
```
<p>

A exclusão mútua é, então, feita com o `sem_wait(&emA)` e o `sem_post(&emA)` em conjunto, envolvendo a seção crítica, impedindo outras *threads A* de incrementar o valor da variável `int a`. Assim, a primeira *thread A* que entra neste trecho, decrementa 1 unidade do semáforo para sincronização lógica com `sem_wait(&rec)`. Isso impede os outros tipos de thread de continuar suas execuções, já que ficarão aguardando no semáforo. E como o semáforo está dentro da seção crítica, com exlusão mútua, todas as threads dos tipos travados, ficarão no mesmo estado.</p>
<p>

Após a seção crítica, outras *threads A* podem realizar suas execuções, respeitando a exlusão mútua, sem precisar passar pelo semáforo, usando o mesmo recurso sem problemas.
</p>
<p>

Em seguida, temos outra seção crítica com exclusão mútua:
```c
sem_wait(&emA);
a--;
if(a==0) sem_post(&rec);
sem_post(&emA);
```
</p> 
<p>

Esta é responsável por decrementar em 1 o valor da `int a`, informando ao programa que já terminou de usar o recurso compartilhado. Caso a última *thread A* executando passe por este trecho, ela incrementa 1 unidade do semáforo para sincronização lógica com `sem_post(&rec)`. Isso libera outro tipo de *thread* que esteja aguardando com `sem_wait(&rec)`. Por fim, encerra a sua execução.
</p>
<p>

Em seguida a próxima thread na fila do semáforo vai ser liberada e os passos ocorrem do mesmo jeito explicado para a *thread A*.</p>
<p>

Portanto, temos a implementação respeitando as condições do problema, ou quase isso. Se a parte que **usa o recurso compartilhado** não for uma operação atômica, teríamos um problema de **condição de corrida**. Para isso seria necessário implementar a exclusão mútua para esta seção crítica. Além disso, *deadlocks* não são um problema. Para um tipo de *thread executar*, é preciso que outro tipo libere o semáforo. Mas como essa liberação é "guardada", então não há risco de a *thread* a ser liberada chegue no semáforo depois da liberação e ficar aguardando por tempo indeterminado.</p>

b)
<p>

Esta solução **pode** levar as threads a um estado de *starvation*. Caso tivermos, por exemplo, muitas *threads A* e poucas dos outros tipos e o trecho de uso do recurso for suficientemente demorado, podemos ter *threads A* repetindo suas execuções antes de zerar a quantidade de threads do mesmo tipo utilizando o mesmo recurso. Dessa forma, o semáforo nunca vai ser liberado para outros tipos de *thread*.
</p>

c)
<p>

Caso o semáforo $rec$ seja iniciado com 0 sinais, teríamos a mesma situação para todos os três tipos de *thread*.</p>
<p>

A primeira *thread* do tipo entraria na primeira seção crítica, impedindo outras do mesmo tipo de acessar este mesmo trecho. Como é a primeira, ela precisa passar pelo semáforo. No entanto, o semáforo nao tem sinais, logo sua execução ficaria travada ali. Por causa deste travamento, também não há chances de incrementar sinais ao semáforo $rec. Dessa forma, nenhum outro tipo de *thread* conseguirá executar completamente, sendo barradas no mesmo semáforo sem sinais.
</p>

d)
<p>

Caso o semáforo $rec$ seja iniciado com mais de um sinal, a primeira passagem da primeira *thread* pelo semáforo, não travaria a *thread*, de outro tipo, seguinte. Fazendo com que as condições do problema não sejam respeitadas, pois pelo menos dois tipos diferentes de *thread* acessariam o mesmo recurso ao mesmo tempo.
</p>

### Questão 2:
a)
<p>

Existe um erro no código: considerando que o produtor produziu e inseriu exatamente um item no buffer, um sinal é incrementado no semáforo $d$ com `sem_post(&d)`. Isso libera a entrada do consumidor no loop. Após isso, o produtor incrementa um sinal no semáforo $s$ com `sem_post(&s)`, liberando a produção de um novo item ou o consumo do item pelo consumidor.
</p>
<p>
	
Consideremes que o consumidor é o que vai usar o sinal do semáforo. Neste caso ele retira o item do buffer normalmente, da mesma forma que o produtor produz e insere,  incrementando um sinal no semáforo $s$ com `sem_post(&s)`. Após isso ele pode consumir o item e esperar mais ser produzido.
</p>
<p>

No entanto, caso antes da linha 11, que é o trecho do código onde é verificado se há 0 itens produzidos (caso seja verdade, aguarda um sinal do semáforo $d$ `sem_wait(&d)`), o produtor produza e insira um item no buffer, teríamos um elemento no buffer. Assim, na linha 10 um sinal seria incrementado no semáforo $d$ com `sem_post(&d)`.
</p>
<p>

Agora voltemos para a thread consumidora, que está antes da linha 11. Neste momento, temos exatamente um item no buffer e um sinal no semáforo $d$. Logo, quando executar a linha 11, não vai usar o sinal do semáforo, pois o número de itens no buffer não é 0. Isso pode se tornar um problema, pois na próxima vez que o número de itens no buffer chegar a 0, o consumidor não vai esperar outro ser produzido. O semáforo com 0 sinais representa esta espera. No entanto, nesse momento o semáforo tem um sinal.
</p>

<p>

Assim, quando o consumidor for retirar um item do buffer, ele vai retirar um que não existe, pois o produtor ainda não fez nenhum novo.
</p>

<p>

Esse problema ocorre principalmente pelo fato da verificação da quantidade de itens no buffer ser feita fora da exclusão mútua. Então para resolver o problema do programa, precisamos mudar esta parte.
</p>

b)
<p>

Uma maneira de resolver esse problema é modificar as duas funções como apresentado logo abaixo:
</p>

```c
int n=0; sem_t s, d; //s inicializado com 1 e d inicializado com 0
void *cons(void *args) {
	int item;
 	while(1) {
		sem_wait(&d); // O semáforo aguarda sinais já dentro do loop
 		sem_wait(&s);
 		retira_item(&item);
 		n--;
 		sem_post(&s);
 		consome_item(item);
		// Não depende do valor de n para esperar. Isso já feito com o semáforo
	}
}

void *prod(void *args) {
	int item;
	while(1) {
		produz_item(&item);
		sem_wait(&s);
		insere_item(item);
		n++;
		sem_post(&d); // Incrementa um sinal no semáforo para cada item no buffer
		sem_post(&s);
	}
}

```
<p>

Dessa forma não é preciso verificar a quantidade de elementos no buffer para travar ou liberar o consumidor. A quantidade de vezes que o consumidor pode retirar um elemento do buffer é controlada pela quantidade de sinais no semáforo $d$. Já o produtor incrementa um sinal no mesmo semáforo a cada item produzido. Além disso, a exclusão mútua feita com o semáforo $s$ garante que os itens vão ser inseridos e retirados corretamente, assim como a alteração no número de itens no buffer.
</p>

### Questão 3:

### Questão 4:
a)
<p>

O semáforo $x$ tem como finalidade realizar a exclusão mútua das seções críticas envolvendo a variável $aux$. Esta variável é responsável por guardar a quantidade de *threads* esperando sinal para prosseguirem suas execuções. Ela é necessária para evitar o problema de condição de corrida, uma vez que várias threads poderiam acessa a variável $aux$ ao mesmo tempo. Este semáforo foi inicializado corretamente com o valor 1, pois ela precisa ter um sinal disponível para a primeira *thread* que for realizar a exclusão mútua.
</p>
<p>

O semáforo $h$ tem como finalidade realizar, de fato, a espera da *thread*, pois, na função `wait()`, é onde trava a execução, aguardando um sinal. Sinal este que é enviado nas funções `notify()` ou `notifyAll()`. Sua inicialização foi feita corretamente com o valor 0, pois inicialmente não há nenhuma notificação enviada para nenhuma *thread*.
</p>
<p>

O semáforo $s$ tem como finalidade fazer as funções `notify()` ou `notifyAll()` esperarem cada *thread* notificada terminarem de receber as notificações para, juntamente com o semáforo $x$, fazer com que o número de *threads* esperando notificação, na variável $aux$, seja decrementado corretamente. Sua inicialização foi feita corretamente com o valor 0, pois inicialmente não há nenhuma *thread* notificada.
</p>

b)
<p>


</p>
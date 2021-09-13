class Numero {
	private int numero; // Recurso compartilhado entre as threads
	private boolean pyLog; // Variável que determina se o programa vai imprimir um log .py ou um log que o enunciado da atividade pede
	
	// Construtor
	Numero(boolean pyLog) {
		this.numero = 0;
		this.pyLog = pyLog;
	}
	
	// Retorna o valor atual do número inteiro
	public int get() {
		// if(!pyLog) System.out.println("Numero agora --> " + this.numero);
		return this.numero;
	}
	
	// Atribui um valor ao número inteiro
	public void set(int n) {
		if(!pyLog) System.out.println("Numero agora --> " + n);
		this.numero = n;
	}
	
	// Imprime se o valor atual do número é primo ou não primo
	public void isPrime() {
		if(this.numero <= 1) {
			if(!pyLog) System.out.println(this.numero + " nao eh primo");
			return;
		}
		for(int i = 2; i <= this.numero; i++) {
			if(this.numero % i == 0 && this.numero != i) {
				if(!pyLog) System.out.println(this.numero + " nao eh primo");
				return;
			}
		}
		if(!pyLog) System.out.println(this.numero + " eh primo");
		return;
	}
	
	// Imprime se o valor atual do número é par ou ímpar
	public void isEven() {
		if(!pyLog) System.out.printf("%d eh %s\n", this.numero, this.numero % 2 == 0 ? "par" : "impar");
	}
}

// Monitor que implementa a logica do padrao leitores/escritores
// Todos os seus métodos tem a opção de indicar se a thread que está lendo
// ou escrevendo é uma thread Leitora/Escritora, imprimindo na saída padrão
// caso o programa não seja configurado para imprimir um log .py
class LE {
  private int leit, escr;
  private boolean pyLog; // Variável que determina se o programa vai imprimir um log .py ou um log que o enunciado da atividade pede
  
  // Construtor
  LE(boolean pyLog) { 
     this.leit = 0; //leitores lendo (0 ou mais)
     this.escr = 0; //escritor escrevendo (0 ou 1)
     this.pyLog = pyLog;
  } 
  
  // Entrada para leitores
  public synchronized void EntraLeitor (int id, boolean leitorEscritor) {
    try { 
      while (this.escr > 0) {
         System.out.println ("le.leitor" + (leitorEscritor && !this.pyLog ? "Escritor" : "") + "Bloqueado("+id+")");
         wait();  //bloqueia pela condicao logica da aplicacao 
      }
      this.leit++;  //registra que ha mais um leitor lendo
      System.out.println ("le.leitor" + (leitorEscritor && !this.pyLog ? "Escritor" : "") + "Lendo("+id+")");
    } catch (InterruptedException e) { }
  }
  
  // Saida para leitores
  public synchronized void SaiLeitor (int id, boolean leitorEscritor) {
     this.leit--; //registra que um leitor saiu
     if (this.leit == 0) 	
           this.notify(); //libera escritor (caso exista escritor bloqueado)
     System.out.println ("le.leitor" + (leitorEscritor && !this.pyLog ? "Escritor" : "") + "Saindo("+id+")");
  }
  
  // Entrada para escritores
  public synchronized void EntraEscritor (int id, boolean leitorEscritor) {
    try { 
      while ((this.leit > 0) || (this.escr > 0)) {
         System.out.println ("le.escritor" + (leitorEscritor && !this.pyLog ? "Leitor" : "") + "Bloqueado("+id+")");
         wait();  //bloqueia pela condicao logica da aplicacao 
      }
      this.escr++; //registra que ha um escritor escrevendo
      System.out.println ("le.escritor" + (leitorEscritor && !this.pyLog ? "Leitor" : "") + "Escrevendo("+id+")");
    } catch (InterruptedException e) { }
  }
  
  // Saida para escritores
  public synchronized void SaiEscritor (int id, boolean leitorEscritor) {
     this.escr--; //registra que o escritor saiu
     notifyAll(); //libera leitores e escritores (caso existam leitores ou escritores bloqueados)
     System.out.println ("le.escritor" + (leitorEscritor && !this.pyLog ? "Leitor" : "") + "Saindo("+id+")");
  }
}

// Representa thread leitora
// Imprime se o valor atual do número (recurso compartilhado) é primo ou não primo
class Leitor extends Thread {
	int id; //identificador da thread
  LE monitor;//objeto monitor para coordenar a lógica de execução das threads
  Numero numero;
  int delay;

  // Construtor
  Leitor (int id, LE m, Numero inteiro, int delay) {
    this.id = id;
    this.monitor = m;
    this.numero = inteiro;
    this.delay = delay;
  }

  // Método executado pela thread
  public void run () {
    try {
      for (;;) {
        this.monitor.EntraLeitor(this.id, false);
        numero.isPrime();
        this.monitor.SaiLeitor(this.id, false);
        sleep(this.delay);
      }
    } catch (InterruptedException e) { return; }
  }
}

// Representa thread escritora
// Atribui ao valor do número(recurso compartilhado) o número identificador da própria thread
class Escritor extends Thread {
	int id; //identificador da thread
  LE monitor;//objeto monitor para coordenar a lógica de execução das threads
  Numero numero;
  int delay;

  // Construtor
  Escritor (int id, LE m, Numero inteiro, int delay) {
    this.id = id;
    this.monitor = m;
    this.numero = inteiro;
    this.delay = delay;
  }

  // Método executado pela thread
  public void run () {
    try {
      for (;;) {
        this.monitor.EntraEscritor(this.id, false);
        numero.set(this.id);
        this.monitor.SaiEscritor(this.id, false);
        sleep(this.delay);
      }
    } catch (InterruptedException e) { return; }
  }
}

// Representa thread leitora/escritora
// Imprime se o valor atual do número (recurso compartilhado) é par ou ímpar
// Atribui ao valor do número(recurso compartilhado) o dobro do valor atual 
class LeitorEscritor extends Thread {
	int id; //identificador da thread
  LE monitor;//objeto monitor para coordenar a lógica de execução das threads
  Numero numero;
  int delay;

  // Construtor
  LeitorEscritor (int id, LE m, Numero inteiro, int delay) {
    this.id = id;
    this.monitor = m;
    this.numero = inteiro;
    this.delay = delay;
  }

  // Método executado pela thread
  public void run () {
    try {
      for (;;) {
      	this.monitor.EntraLeitor(this.id, true);
      	numero.isEven();
      	this.monitor.SaiLeitor(this.id, true);
      	sleep(this.delay);
        this.monitor.EntraEscritor(this.id, true);
        numero.set(numero.get()*2);
        this.monitor.SaiEscritor(this.id, true);
        sleep(this.delay);
      }
    } catch (InterruptedException e) { return; }
  }
}

public class Lab7 {
	public static int NL = 1;
	public static int NE = 2;
	public static int NLE = 4;
	
	public static void main(String[] args) {
		boolean pyLog = false; // Variável que determina se o programa vai imprimir um log .py ou um log que o enunciado da atividade pede
		
		if(args.length > 0 && args[0].equals("pyLog")) pyLog = true;
		
		

		Numero inteiro = new Numero(pyLog);
		LE monitor = new LE(pyLog);
		Leitor[] leitores = new Leitor[NL];
		Escritor[] escritores = new Escritor[NE];
		LeitorEscritor[] leitoresEscritores = new LeitorEscritor[NLE];

		if(pyLog) {
			//inicia o log de saida
	    System.out.println ("import verificaLE");
	    System.out.println ("le = verificaLE.LE()");
		}
		
		int i;

		// Iniciando as threads leitoras
		for (i=0; i<NL; i++) {
	    leitores[i] = new Leitor(i+1, monitor, inteiro, (i+1)*500);
	    leitores[i].start();
		}
		
		// Iniciando as threads escritoras
		for (i=0; i<NE; i++) {
  	 escritores[i] = new Escritor(i+1, monitor, inteiro, (i+1)*500);
  	 escritores[i].start();
		}
		
		// Iniciando as threads leitoras/escritoras
		for (i=0; i<NLE; i++) {
  	 leitoresEscritores[i] = new LeitorEscritor(i+1, monitor, inteiro, (i+1)*500);
  	 leitoresEscritores[i].start();
		}
	}
}

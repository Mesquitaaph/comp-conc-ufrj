
class Numero {
	private int numero; // Recurso compartilhado entre as threads
	
	// Construtor
	Numero() {
		this.numero = 0;
	}
	
	// Retorna o valor atual do número inteiro
	public int get() {
		// if(!pyLog) System.out.println("Numero agora --> " + this.numero);
		return this.numero;
	}
	
	// Atribui um valor ao número inteiro
	public void set(int n) {
		// System.out.println("Numero agora --> " + n);
		this.numero = n;
	}
	
	// Imprime se o valor atual do número é primo ou não primo
	public void isPrime() {
		if(this.numero <= 1) {
			// System.out.println(this.numero + " nao eh primo");
			return;
		}
		for(int i = 2; i <= this.numero; i++) {
			if(this.numero % i == 0 && this.numero != i) {
				// System.out.println(this.numero + " nao eh primo");
				return;
			}
		}
		// System.out.println(this.numero + " eh primo");
		return;
	}
	
	// Imprime se o valor atual do número é par ou ímpar
	public void isEven() {
		// System.out.printf("%d eh %s\n", this.numero, this.numero % 2 == 0 ? "par" : "impar");
	}
}

class LE {
  private int leit, escr, qescr;
  
  // Construtor
  LE() { 
    this.leit = 0;  //leitores lendo (0 ou mais)
    this.escr = 0;  //escritor escrevendo (0 ou 1)
    this.qescr = 0; //escritores querendo escrever (0 ou mais)
  } 
  
  // Entrada para leitores
  public synchronized void EntraLeitor (int id) {
    try { 
      while (this.escr > 0 || this.qescr > 0) {
        wait();  //bloqueia pela condicao logica da aplicacao 
      }
      this.leit++;  //registra que ha mais um leitor lendo
    } catch (InterruptedException e) { }
  }
  
  // Saida para leitores
  public synchronized void SaiLeitor (int id) {
    this.leit--; //registra que um leitor saiu
    if (this.leit == 0) 	
      this.notify(); //libera escritor (caso exista escritor bloqueado)
  }
  
  // Entrada para escritores
  public synchronized void EntraEscritor (int id) {
    try { 
      while ((this.leit > 0) || (this.escr > 0)) {
        this.qescr++; //registra o bloqueio de um escritor
        wait();  //bloqueia pela condicao logica da aplicacao 
        this.qescr--; //registra o desbloqueio de um escritor
      }
      this.escr++; //registra que ha um escritor escrevendo
    } catch (InterruptedException e) { }
  }
  
  // Saida para escritores
  public synchronized void SaiEscritor (int id) {
    this.escr--; //registra que o escritor saiu
    notifyAll(); //libera leitores e escritores (caso existam leitores ou escritores bloqueados)
  }
}

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
        this.monitor.EntraLeitor(this.id);
        numero.isEven();
        sleep(this.delay);
        this.monitor.SaiLeitor(this.id);
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
        this.monitor.EntraEscritor(this.id);
        numero.set(this.id);
        this.monitor.SaiEscritor(this.id);
        sleep(this.delay);
      }
    } catch (InterruptedException e) { return; }
  }
}

public class Q4Cod {
  public static int NL = 10;
	public static int NE = 1;

  public static void main(String args[]) {
    Numero inteiro = new Numero();
    LE monitor = new LE();
    Leitor[] leitores = new Leitor[NL];
    Escritor[] escritores = new Escritor[NE];

    int i;

		// Iniciando as threads leitoras
		for (i=0; i<NL-1; i++) {
	    leitores[i] = new Leitor(i+1, monitor, inteiro, (i+1)*700);
	    leitores[i].start();
		}
    // leitores[NL-1] = new Leitor(NL, monitor, inteiro, 10000);
    // leitores[NL-1].start();
		
    // for(long j = 0L; j < 100000000L; j++) {}
    // System.out.println("Esdad");
		// Iniciando as threads escritoras
		for (i=0; i<NE; i++) {
      escritores[i] = new Escritor(i+1, monitor, inteiro, (i+1)*500);
      escritores[i].start();
		}
  }
}
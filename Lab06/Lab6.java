/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Módulo 2 - Laboratório: 6 */
/* Codigo: Implementação da contagem de números pares, na forma concorrente em Java, num vetor de inteiros */
/* Aluno: Raphael Mesquita */ 
/* DRE: 118020104 */ 

// Classe para contar, na forma sequencial, o número de pares no vetor
class ProcuraParesSequencial {
	int contPares;
	int[] inteiros;
	
	public ProcuraParesSequencial(int[] inteiros) {
		this.contPares = 0;
		this.inteiros = inteiros;
	}
	
	public void contaPares() {
		for(int i = 0; i < inteiros.length; i++) {
			if(inteiros[i] % 2 == 0) this.contPares++;
		}
	}
	
	public int get() {
		return this.contPares;
	}
}

// Classe da estrutuda de dados compartilhada entre as threads
class ProcuraPares {
	// Recursos compartilhados
	int index;
	int contPares;

	// Contrutor
	public ProcuraPares() {
		this.index = 0;
		this.contPares = 0;
	}
	
	// Método que retorna o índice do vetor de inteiros a ser verificado e o incrementa para a sua próxima leitura
	public synchronized int next() {
		return this.index++;
	}
	
	// Método que incrementa o número de pares encontrados
	public synchronized void incPares() {
		this.contPares++;
	}
	
	// Método que retorna o número de pares encontrados
	public int get() {
		return this.contPares;
	}
}

// Classe que estende a interface Thread e implementa a tarefa de cada thread do programa
class Tarefa extends Thread {
	// Identificador da thread
	private int id;
	
	// Objetos compartilhados entre as threads
	private int[] inteiros;
	private ProcuraPares pares;

	// Construtor
	public Tarefa(int tid, int[] ints, ProcuraPares ps) {
		this.id = tid;
		this.inteiros = ints;
		this.pares = ps;
	}
	
	// Método main da thread
	public void run() {
		System.out.println("Iniciando a thread " + id);
		while(true) {
			int i = pares.next();
			if(i >= inteiros.length) break;
			
			if(inteiros[i] % 2 == 0) pares.incPares();
		}
		System.out.println("Encerrando a thread " + id);
	}
}

// Classe principal do programa
public class Lab6 {
	static final int N_THREADS = 4;
	static final int N = 1000;
	
	public static void main(String[] args) {
		// reserva espaço para vetor de threads
		Thread[] threads = new Thread[2];
		
		// Cria o vetor de números inteiros aleatórios
		int[] inteiros = new int[N];
		for(int i = 0; i < N; i++) {
			inteiros[i] = (int) (Math.random()*100);
			System.out.print(inteiros[i] + " ");
		}
		System.out.println();

		// Cria intâncias de recursos compartilhados entre as threads
		ProcuraPares pares = new ProcuraPares();
		ProcuraParesSequencial paresSeq = new ProcuraParesSequencial(inteiros);
		
		// Cria as threads do programa
		for (int i=0; i<threads.length; i++) {
			threads[i] = new Tarefa(i, inteiros, pares);
		}
		
		// Inicia as threads
		for (int i=0; i<threads.length; i++) {
			threads[i].start();
		}

		// Espera as threads do programa encerrarem suas execuções
    for (int i=0; i<threads.length; i++) {
    	try { threads[i].join(); } catch (InterruptedException e) { return; }
    }
    
    // Conta a quantidade de pares no vetor de inteiros na forma sequencial
    paresSeq.contaPares();

    // Verifica a corretude da forma concorrente de contagem de números pares no vetor de inteiros
    System.out.println("ParesConc no vetor = " + pares.get()); 
    System.out.println("ParesSeq no vetor = " + paresSeq.get()); 
    
    System.out.printf("Contagem de pares está %s\n", pares.get() == paresSeq.get() ? "correta" : "errada");
		
	}
}

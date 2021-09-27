import java.util.LinkedList;

class Incr extends Thread {
  private int contador;
   
  //--construtor
  public Incr() { 
    this.contador = 0;
  }

  //--metodo executado pela thread
  public void run() {
    try{
      int delay = 1000;
      // for(int i = 0; i < delay; i++) {
      //   for(int j = 0; j < delay; j++) {
      //     if(i == delay-1 && j == delay - 1) System.out.println(contador++); 
      //   }
      // }
      sleep(0);
      // for(long i = 0L; i < contador*100000L; i++) {}
      System.out.println("TESTE");
      // System.out.println(contador++); 
    } catch (InterruptedException e) { return; }
  }
}

class FilaTarefas {
  private int nThreads;
  private MyPoolThreads[] threads;

  //similar a um vetor de objetos Runable
  private LinkedList<Runnable> queue;
  private boolean shutdown;

  public FilaTarefas(int nThreads) {
      this.shutdown = false;
      this.nThreads = nThreads;
      queue = new LinkedList<Runnable>();
      threads = new MyPoolThreads[nThreads];
      for (int i=0; i<nThreads; i++) {
        threads[i] = new MyPoolThreads(i);
        threads[i].start(); 
      }
  }
  
  public void execute(Runnable r) {
    synchronized(queue) {
      if (this.shutdown) return;
      queue.addLast(r); //inclui um novo elemento na lista ’queue’
      System.out.println("Notificando");
      queue.notify();
    }
  }

  public void shutdown() {
    synchronized(queue) { this.shutdown=true; queue.notifyAll();}
    for (int i=0; i<nThreads; i++)
      try { threads[i].join(); }
      catch (InterruptedException e) {return;}
  }

  private class MyPoolThreads extends Thread {
    public int id;

    public MyPoolThreads(int id) {
      this.id = id;
    }

    public void run() {
      // for(long i = 0L; i < 10000000000L; i++) {}
      //       System.out.println("Thread iniciada");
      Runnable r;
      while (true) {
        // for(long i = 0L; i < (1 - this.id)*100000000L; i ++) {}
        synchronized(queue) {
          //verifica se a lista esta vazia...
          while (queue.isEmpty() && (!shutdown)) {
            try { System.out.println("esperando thread " + this.id);queue.wait();System.out.println("encerrando thread " + this.id); }
            catch (InterruptedException ignored){}
          }
          if(queue.isEmpty() && shutdown) {
            System.out.println("N tem mais nada");
            return;
          }

          //retira o primeiro elemento da lista e o retorna
          r = (Runnable) queue.removeFirst();
        }
        try { r.run(); } catch (RuntimeException e) {}
      }
    }
  } 
}

public class Q3 {
  public static int NTHREADS = 2;

  public static void main(String[] args) {
    FilaTarefas ft = new FilaTarefas(NTHREADS);
    Incr incre = new Incr();

    // for(int i = 0; i < 10; i++) ft.execute(incre);

    // for(long i = 0L; i < 1000000000L; i++) {}
    System.out.println("Encerrando programa");
    ft.shutdown();
  }
}
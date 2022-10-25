import java.util.concurrent.*;

public class TrafficController {

    Semaphore sem;

    public TrafficController() 
    {
        sem = new Semaphore(1);
    }

    public void enterLeft() {
        try {
            sem.acquire();
        }
        catch (InterruptedException exc) {
            System.out.println("aqui" + exc);
        }
    }
    public void enterRight() {
        try {
            sem.acquire();
        }
        catch (InterruptedException exc) {
            System.out.println(exc);
        }
    }
    public void leaveLeft() {
        sem.release();
    }
    public void leaveRight() {
        sem.release();
    }

}

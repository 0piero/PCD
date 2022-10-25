import java.util.concurrent.*;

public static class TrafficController {

    Semaphore sem;

    public TrafficController() 
    {
        sem = new Semaphore(1);
    }

    public void enterLeft() {
        sem.acquire();
    }
    public void enterRight() {
        sem.acquire();
    }
    public void leaveLeft() {
        sem.release();
    }
    public void leaveRight() {
        sem.release();
    }

}

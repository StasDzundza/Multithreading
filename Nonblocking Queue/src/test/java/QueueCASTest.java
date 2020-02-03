import org.junit.Assert;
import org.junit.Test;

public class QueueCASTest {
    @Test
    public void offerTest() throws InterruptedException {
        QueueCAS<Integer> queue= new QueueCAS<>();
        int useTimes = 10;
        QueueUser user = new QueueUser(queue,useTimes);
        new Thread(user,"1").start();
        new Thread(user,"2").start();
        Thread.sleep(200);
        Assert.assertEquals(useTimes*2,queue.getSize());
    }

    @Test
    public void pollTest() throws InterruptedException {
        QueueCAS<Integer> queue= new QueueCAS<>();
        int useTimes = 10;
        QueueUser user = new QueueUser(queue,useTimes);
        new Thread(user,"1").start();
        new Thread(user,"2").start();
        Thread.sleep(200);
        user.setUseFunction((q)->{
            for(int i = 0; i < useTimes; i++){
                q.poll();
            }
        });
    }
}

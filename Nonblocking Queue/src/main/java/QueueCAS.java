
import java.util.concurrent.atomic.AtomicReferenceFieldUpdater;

public class QueueCAS<E> {

    private static class Node<E> {
        private volatile E item;
        private volatile Node<E> next;
        private volatile Node<E> prev;

        Node(E x) { item = x; next = null; prev = null; }
        Node(E x, Node<E> n) { item = x; next = n; prev = null; }
        E getItem() {
            return item;
        }
        Node<E> getNext() {
            return next;
        }
        void setNext(Node<E> val) {
            next = val;
        }
        Node<E> getPrev() {
            return prev;
        }
        void setPrev(Node<E> val) {
            prev = val;
        }
    }

    private static final
    AtomicReferenceFieldUpdater<QueueCAS, Node>
            tailUpdater =
            AtomicReferenceFieldUpdater.newUpdater
                    (QueueCAS.class, Node.class, "tail");

    private static final
    AtomicReferenceFieldUpdater<QueueCAS, Node>
            headUpdater =
            AtomicReferenceFieldUpdater.newUpdater
                    (QueueCAS.class, Node.class, "head");

    private boolean casTail(Node<E> expectedValue, Node<E> newValue) {
        return tailUpdater.compareAndSet(this, expectedValue, newValue);
    }

    private boolean casHead(Node<E> expectedValue, Node<E> newValue) {
        return headUpdater.compareAndSet(this, expectedValue, newValue);
    }

    private transient volatile Node<E> head = new Node<E>(null, null);
    private transient volatile Node<E> tail = head;

    /**Tail is in the beginning*/
    public boolean offer(E value) {
        if (value == null) throw new NullPointerException();
        Node<E> newNode = new Node<E>(value, null);
        while (true){
            Node<E> currentTail = tail;
            newNode.setNext(currentTail);
            if (casTail(currentTail, newNode)) {
                currentTail.setPrev(newNode);
                return true;
            }
        }
    }

    /**deleting from the end*/
    public E poll() {
        while (true){
            Node<E> curHead = head;
            Node<E> curTail = tail;
            Node<E> first = curHead.getPrev();
            if (curHead == head) {
                if (curHead != curTail) {
                    if (first == null){
                        fixList(curTail,curHead);
                        continue;
                    }
                    E item = first.getItem();
                    if (casHead(curHead,first)) {
                        curHead.setNext(null);
                        curHead.setPrev(null);
                        return item;
                    }
                }
                else
                    return null;
            }
        }
    }

    /**Fixing the backwords pointers when needed*/
    private void fixList(Node<E> tail, Node<E> head){
        Node<E> curNodeNext;
        Node<E> curNode = tail;
        while (head == this.head && curNode != head){
            curNodeNext = curNode.getNext();
            curNodeNext.setPrev(curNode);
            curNode = curNode.getNext();
        }
    }

    @Override
    public String toString(){
        StringBuilder stringBuilder = new StringBuilder();
        Node<E>cur = head.getPrev();
        while(cur!=null){
            stringBuilder.append(cur.getItem() + " ");
            cur = cur.getPrev();
        }
        return stringBuilder.toString();
    }

    public int getSize(){
        int size = 0;
        Node<E>cur = head.getPrev();
        while(cur!=null){
            size++;
            cur = cur.getPrev();
        }
        return size;
    }
}
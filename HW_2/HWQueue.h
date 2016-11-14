#ifndef HWQUEUE_H
#define HWQUEUE_H

template <class DATA>
class HWQueue {
private:
    int capacity;
    DATA* container;
    int front_ind;
    int back_ind;
    bool last_op;
    int current_size;
public:
    HWQueue();
    HWQueue(int capacity);

    void enqueue(DATA toEnqueue);
    DATA dequeue();
    DATA seeFront();
    int getSize();
private:
    bool isFull();
    bool isEmpty();

    void initializeContainer();

    void setCapacity(int capacity);
};

#endif // HWQUEUE_H

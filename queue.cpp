#include<iostream>
#define INVALID_ELEMENT -1
#define DEQUEUED 0
#define ENQUEUED 1

using namespace std;

class MyQueue {
private:
    int max_capacity = 5;
    int* data = new int[max_capacity];
    int front = 0;
    int back = 0;
    bool last_op = DEQUEUED;

public:
    bool isEmpty()
    {
        return (front == back && last_op == DEQUEUED);
    }

    bool isFull()
    {
        return (front == back && last_op == ENQUEUED);
    }

    void enqueue(int el)
    {
        if (!isFull())
        {
            data[back] = el;
            back = (back + 1) % max_capacity;
            last_op = ENQUEUED;
        } else {
            cout << "queue is full ..." << endl;
        }
    }

    int dequeue()
    {
        if (!isEmpty())
        {
            int front_el = data[front];
            front = (front + 1) % max_capacity;
            last_op = DEQUEUED;
            return front_el;
        } else {
            cout << "queue is empty ..." << endl;
            return INVALID_ELEMENT;
        }
    }
};

main(void)
{
    MyQueue * test = new MyQueue();

    for (int i = 0; i < 5; i++)
    {
        test->enqueue(i);
    }

    for (int i = 0; !test->isEmpty(); i++)
    {
        cout << test->dequeue() << endl;
    }
}

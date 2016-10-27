#include<iostream>
#define INVALID_ELEMENT -1

using namespace std;


class MyQueue {
private:
    class QueueNode {
    public:
        int data = INVALID_ELEMENT;
        QueueNode * next = NULL;
    };

    QueueNode * front = new QueueNode();
    QueueNode * back = front;

public:
    bool isEmpty()
    {
        return front == back;
    }

    void enqueue(int el)
    {
        QueueNode * newBack = new QueueNode();
        back->data = el;
        back->next = newBack;
        back = newBack;
    }

    int dequeue()
    {
        if (!isEmpty())
        {
            QueueNode * old_front = front;

            int front_el = front->data;
            front = front->next;

            delete old_front;
            return front_el;
        } else {
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

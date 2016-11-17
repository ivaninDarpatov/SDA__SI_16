#ifndef HWQUEUE_H
#define HWQUEUE_H

#define INVALID_ELEMENT -1
#define DEQUEUED 0
#define ENQUEUED 1

template <class DATA>
class HWQueue
{
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
    ~HWQueue();

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

template <class DATA>
HWQueue<DATA>::HWQueue() {}

template <class DATA>
HWQueue<DATA>::HWQueue(int capacity)
{
    this->setCapacity(capacity);
    this->initializeContainer();
    this->front_ind = 0;
    this->back_ind = 0;
    this->last_op = DEQUEUED;
    this->current_size = 0;
}

template <class DATA>
HWQueue<DATA>::~HWQueue()
{

}

//interface methods
template <class DATA>
void HWQueue<DATA>::enqueue(DATA toEnqueue)
{
    if (!this->isFull())
    {
        this->container[this->back_ind] = toEnqueue;
        this->back_ind = (this->back_ind + 1) % this->capacity;
        this->last_op = ENQUEUED;
        this->current_size++;
    }
    else
    {
        throw CustomException("exception: queue is full");
    }
}

template <class DATA>
DATA HWQueue<DATA>::dequeue()
{
    if (!this->isEmpty())
    {
        DATA front_el = this->container[this->front_ind];
        this->front_ind = (this->front_ind + 1) % this->capacity;
        this->last_op = DEQUEUED;
        this->current_size--;
        return front_el;
    }
    else
    {
        throw CustomException("exception: queue is empty");
    }
}

template <class DATA>
DATA HWQueue<DATA>::seeFront()
{
    if (!this->isEmpty())
    {
        return this->container[this->front_ind];
    }
    else
    {
        throw CustomException("exception: queue is empty");
    }
}

template <class DATA>
int HWQueue<DATA>::getSize()
{
    return this->current_size;
}
//

//private methods
template <class DATA>
bool HWQueue<DATA>::isFull()
{
    return (this->front_ind == this->back_ind && this->last_op == ENQUEUED);
}

template <class DATA>
bool HWQueue<DATA>::isEmpty()
{
    return (this->front_ind == this->back_ind && this->last_op == DEQUEUED);
}

template <class DATA>
void HWQueue<DATA>::initializeContainer()
{
    if (this->capacity > 0)
    {
        this->container = new DATA[this->capacity];
    } else
    {
        throw CustomException("exception: to initialize container, initialize capacity first");
    }
}

template <class DATA>
void HWQueue<DATA>::setCapacity(int capacity)
{
    if (capacity > 0)
    {
        this->capacity = capacity;
    } else
    {
        throw CustomException("exception: capacity must be higher than 0 elements");
    }
}
//

#endif // HWQUEUE_H

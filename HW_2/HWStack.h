/**
 *
 * Solution to homework task
 * Data Structures Course
 * Faculty of Mathematics and Informatics of Sofia University
 * Winter semester 2016/2017
 *
 * @author Ivan Darpatov
 * @idnumber 61810
 * @task 2
 * @compiler GCC
 *
 */

#ifndef HWSTACK_H
#define HWSTACK_H
#include <iostream>

#define STACK_BEGIN_INDEX 0

using namespace std;

template <class DATA>
class HWStack {
private:
    int capacity;
    DATA* container;
    int TOS;
    int current_size;
public:
    HWStack();
    HWStack(int capacity);
    ~HWStack();

    void push(DATA toPush);
    DATA pop();
    DATA peek();
    int getSize();
private:
    bool isFull();
    bool isEmpty();

    void initializeContainer();

    void setCapacity(int capacity);
};

template <class DATA>
HWStack<DATA>::HWStack() {}

template <class DATA>
HWStack<DATA>::HWStack(int capacity)
{
    this->setCapacity(capacity);
    this->initializeContainer();
    this->TOS = STACK_BEGIN_INDEX;
    this->current_size = STACK_BEGIN_INDEX;
}

template <class DATA>
HWStack<DATA>::~HWStack()
{

}

//private methods
template <class DATA>
bool HWStack<DATA>::isEmpty()
{
    return this->current_size <= 0;
}

template <class DATA>
bool HWStack<DATA>::isFull()
{
    return this->current_size >= this->capacity;
}

template <class DATA>
void HWStack<DATA>::initializeContainer()
{
    if (this->capacity > 0)
    {
        this->container = new DATA[this->capacity];
    } else
    {
        throw CustomException("exception: to initialize container, initialize capacity first");
    }
}

//setters
template <class DATA>
void HWStack<DATA>::setCapacity(int capacity)
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
//

//'interface' methods
template <class DATA>
void HWStack<DATA>::push(DATA toPush)
{
    if (this->isFull())
    {
        throw CustomException("exception: stack is full");
    }

    this->container[this->TOS++] = toPush;
    this->current_size++;
}

template <class DATA>
DATA HWStack<DATA>::pop()
{
    if (this->isEmpty())
    {
        throw CustomException("exception: stack is empty");
    }

    DATA toReturn = this->container[--this->TOS];
    this->current_size--;
    return toReturn;
}

template <class DATA>
DATA HWStack<DATA>::peek()
{
    if (this->isEmpty())
    {
        throw CustomException("exception: stack is empty");
    }

    return this->container[this->TOS - 1];
}

template <class DATA>
int HWStack<DATA>::getSize()
{
    return this->current_size;
}
//
#endif

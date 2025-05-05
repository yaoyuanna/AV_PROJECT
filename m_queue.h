#ifndef M_QUEUE_H
#define M_QUEUE_H
#include<queue>
#include<QMutex>
using namespace std;
template <class T>
class m_queue
{
public:
    m_queue();
    queue<T>que;
    QMutex lock;
    void push(T data);
    void pop();
    T front();
};
#include "m_queue.h"


template<class T>
m_queue<T>::m_queue()
{
}

template<class T>
void m_queue<T>::push(T data)
{
    QMutexLocker locker(&lock);
    return que.push(data);
}

template<class T>
T m_queue<T>::front()
{
    QMutexLocker locker(&lock);
    return que.front();
}

template<class T>
void m_queue<T>::pop()
{
    QMutexLocker locker(&lock);
    return que.pop();
}

#endif // M_QUEUE_H

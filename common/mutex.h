#pragma once
#include <pthread.h>

class CMutex
{
    public:
        CMutex(bool isRecursive=false)
        {
            if(!isRecursive) 
            {
                pthread_mutexattr_init(&m_attr);
                pthread_mutexattr_settype(&m_attr,PTHREAD_MUTEX_RECURSIVE);
            }
            pthread_mutex_init(&m_mutex,&m_attr);
        }
        virtual ~CMutex()
        {
            pthread_mutex_unlock(&m_mutex);
            pthread_mutexattr_destroy(&m_attr);
            pthread_mutex_destroy(&m_mutex);
        }
        void Lock()
        {
            pthread_mutex_lock(&m_mutex); 
        }
        /*true -getlock success
         *false-false - getlock failed
         */
        bool TryLock()
        {
           return pthread_mutex_trylock(&m_mutex)==0? true:false;
        }
        void UnLock()
        {
            pthread_mutex_unlock(&m_mutex);
        }
    public:
        pthread_mutex_t m_mutex;
        pthread_mutexattr_t  m_attr;
};

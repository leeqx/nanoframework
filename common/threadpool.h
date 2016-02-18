#pragma once
#include <pthread.h>
#include "common/global.h"

class CThreadPool
{
    public:
        CThreadPool(unsigned int poolSize);
        virtual ~CThreadPool();

    private:
        unsigned int m_poolSize;
        unsigned int m_poolFree;
        pthread_mutex_t m_mutex;

};


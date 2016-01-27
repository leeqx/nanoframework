/**
 * 后端消息处理,工厂方法，根据消息创建需要的处理对象然后处理消息
 */
#pragma once
#include <pthread.h>
#include "msghandler/msg.h"

class CBaseHandler
{
    public:
        virtual int Handler();
};
// TODO
class CNanoFactory
{
    public:
        CNanoFactory()
        {
            pthread_mutex_init(&m_mutex,NULL);
        }
        virtual ~CNanoFactory(){
            pthread_mutex_destroy(&m_mutex);
        }
        static CNanoFactory* Instance()
        {
            pthread_mutex_lock(&m_mutex);
            if(NULL == m_pNanoFactory)
            {
                m_pNanoFactory = new CNanoFactory;
            }
            pthread_mutex_unlock(&m_mutex);
            return m_pNanoFactory;
        }
        CBaseHandler* CreateHandleByType(t_msg* msg)
        {
            return NULL;
        }
    public:
        static  CNanoFactory *m_pNanoFactory;
        static  pthread_mutex_t m_mutex;
};


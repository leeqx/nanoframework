#include "handler_factory.h"

CNanoFactory *CNanoFactory::m_pNanoFactory = NULL;
pthread_mutex_t CNanoFactory::m_mutex      = PTHREAD_MUTEX_INITIALIZER;

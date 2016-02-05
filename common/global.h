#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>


#define    ERROR -1
#define    DEBUG 0
#define    INFO 1
#define    WARN 2
#define    MAX_EVENTS_SIZE  65535
#define    time(x) 0

#define __LOG(fd,level,fmt,...) \
    if((level) == ERROR)\
    { \
        fprintf(fd,"%ld,ERROR:%s-%s-%u:" fmt "\n",time(NULL),__FILE__,__func__,__LINE__,##__VA_ARGS__);\
    } \
    else if ((level) == DEBUG) \
    { \
        fprintf(fd,"%ld,DEBUG:%s-%s-%u:" fmt "\n",time(NULL),__FILE__,__func__,__LINE__,##__VA_ARGS__);\
    }\
    else if((level) == INFO) \
    { \
        fprintf(fd,"%ld,INFO:%s-%s-%u:" fmt "\n",time(NULL),__FILE__,__func__,__LINE__,##__VA_ARGS__);\
    }\
    else if((level) == WARN) \
    { \
        fprintf(fd,"%ld,INFO:%s-%s-%u:" fmt "\n",time(NULL),__FILE__,__func__,__LINE__,##__VA_ARGS__);\
    }

#define LOG(level,fmt,...) __LOG(stdout,level,fmt,##__VA_ARGS__)

#define ASSERT_EQ(a,b) if((a)==(b)) \
                    { \
                        LOG(DEBUG, "assert failed"); \
                    }

#define ASSERT_NE(a,b) if((a)!=(b)) \
                    { \
                        LOG(DEBUG, "assert failed"); \
                    }

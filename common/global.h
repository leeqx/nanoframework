#pragma once

#define    ERROR -1
#define    DEBUG 0
#define    INFO 1
#define    WARN 2

#define __LOG(fd,level,fmt,...) \
    time_t cur; \
    time(&cur); \
    if((level) == ERROR)\
    { \
        fprintf(fd,"%ld,ERROR:%s-%s-%d:" fmt,cur,__FILE__,__func__,__LINE__,##__VA_ARGS__);\
    } \
    else if ((level) == DEBUG) \
    { \
        fprintf(fd,"%ld,DEBUG:%s-%s-%d:" fmt,cur,__FILE__,__func__,__LINE__,##__VA_ARGS__);\
    }\
    else if((level) == INFO) \
    { \
        fprintf(fd,"%ld,INFO:%s-%s-%d:" fmt,cur,__FILE__,__func__,__LINE__,##__VA_ARGS__);\
    }\
    else if((level) == WARN) \
    { \
        fprintf(fd,"%ld,INFO:%s-%s-%d:" fmt,cur,__FILE__,__func__,__LINE__,##__VA_ARGS__);\
    }

#define LOG(level,fmt,...) __LOG(stdout,level,fmt,...)


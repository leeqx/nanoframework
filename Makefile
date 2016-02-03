TARGET=libnanofrw.so
BIN=main

CXX=g++
CFLAGS+=-g -shared -fPIC
INCLUDE=-I./
LIB_LIBRARY_PATH+=
LIBS=-lpthread -lrt

SRC=$(wildcard common/*.cpp msghandler/*.cpp net/*.cpp protocol/*.cpp)
OBJS=$(SRC:.cpp=.o)

MAIN_SCR=$(wildcard main/*.cpp)
MAIN_OBJS=$(MAIN_SRC:.cpp=.o)

all:lib/${TARGET} bin/${BIN}

lib/${TARGET}:${OBJS}
	${CXX} ${CFLAGS} $(addprefix ./objs/,$?) -o $@   ${LIB_LIBRARY_PATH} ${LIBS}

bin/${BIN}:${MAIN_OBJS}
	${CXX} ${CFLAGS} $(addprefix ./objs/,$?) -o $@  ${LIB_LIBRARY_PATH} ${LIBS} -L./lib/ -lnanofrw

${OBJS}:%.o:%.cpp
	[ ! -e $(dir $(addprefix ./objs/,$?)) ] && mkdir  -p $(dir $(addprefix ./objs/,$?)); ${CXX} -c ${CFLAGS} $? -o $(addprefix ./objs/,$@) ${INCLUDE}

${MAIN_OBJS}:%.o:%.cpp
	[ ! -e $(dir $(addprefix ./objs/,$?)) ] && mkdir -p $(dir $(addprefix ./objs/,$?)); ${CXX} -c -g $? -o $(addprefix ./objs/,$@) ${INCLUDE}

.PHONY:dir
dir:
	[ ! -e objs ] && mkdir objs; [ ! -e bin ] && mkdir bin; [ ! -e lib ] && mkdir lib;echo " create dirs ok"

.PHONY:clean
clean:
	rm -rf $(addprefix ./objs/,${OBJS}) $(addprefix ./objs/,${MAIN_OBJS}) bin/* lib/*

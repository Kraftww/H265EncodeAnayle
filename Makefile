MAIN_DIR=$(shell pwd)
SRC_DIR=${MAIN_DIR}/src
INC_DIR=${MAIN_DIR}/include
BIN_DIR=${MAIN_DIR}/bin
LIB_DIR=${MAIN_DIR}/lib
OBJ_DIR=${MAIN_DIR}/obj

SRC_FILE:=H265AnlyPrint.c
OBJ_FILE:=H265AnlyPrint

#gcc
GCC=gcc
C_FLAGS=-c
O_FLAGS=-o
OBJS=H265AnlyPrint.o

all:$(OBJS)
	${GCC} -I${INC_DIR} ${O_FLAGS} ${BIN_DIR}/${OBJ_FILE} ${OBJ_DIR}/${OBJS}

${OBJS}:${SRC_DIR}/${SRC_FILE}
	${GCC} ${C_FLAGS} ${SRC_DIR}/${SRC_FILE} 
	@mv ${OBJS} ${OBJ_DIR}

.PHONY:clean

clean:
	${RM} ${BIN_DIR}/* ${LIB_DIR}/* ${OBJ_DIR}/*
	@date; echo "Bye kraft"

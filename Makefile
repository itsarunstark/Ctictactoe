export CC=gcc
export BIN=./bin
export LIB=./lib
export SRC=./src
# export OBJ=./obj
export FLAGS=-Wall -g -lSDL2 -lSDL2_ttf -lSDL2_gfx

${BIN}/TicTacToe : ${BIN}/binary
	echo 'LD_LIBRARY_PATH=`dirname -- $0`/../lib `dirname -- $0`/binary' > $@
	chmod +x $@

${BIN}/binary : ${SRC}/tictactoe.c ${BIN} ${LIB} ${LIB}/libwindow.so  ${LIB}/libengine.so
	${CC} ${FLAGS} -L ${LIB} -lwindow -lengine -o  $@ $<

${LIB}/libwindow.so : ${SRC}/window.c
	${CC} ${FLAGS} -o $@ $< -shared -fPIC

${LIB}/libengine.so : ${SRC}/tools.c
	${CC} ${FLAGS} -o $@ $< -shared -fPIC

${BIN}	:
	mkdir ${BIN}
${LIB}	:
	mkdir ${LIB}


clean	:
	rm -rf ${BIN}/*
	rm -rf ${LIB}/*
	rmdir ${BIN}
	rmdir ${LIB}




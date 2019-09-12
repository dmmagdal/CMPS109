# $Id: Makefile,v 1.10 2016-06-30 17:35:43-07 - - $

GPP  = g++ -std=gnu++14 -g -O0 -Wall -Wextra -Wold-style-cast
LIBS = -lglut -lGLU -lGL -lX11 -lrt -lm

SOURCES  = hello-world.cpp bonjour-le-monde.cpp ciao-mondo.cpp \
           hallo-welt.cpp konnichiwa-sekai.cpp
BINARIES = ${SOURCES:.cpp=}

all : ${BINARIES}

% : %.cpp
	${GPP} $< -o $@ ${LIBS}

ci : ${SOURCES} Makefile
	cpplint.py.perl ${SOURCES}
	checksource ${SOURCES} Makefile
	cid + ${SOURCES} Makefile

clean :
	- rm ${BINARIES} Listing.ps Listing.pdf

test : all
	for exec in ${BINARIES}; do $$exec & done

lis : ${SOURCES}
	mkpspdf Listing.ps ${SOURCES} Makefile

again :
	${MAKE} clean ci all lis test


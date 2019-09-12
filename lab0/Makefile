# $Id: Makefile,v 1.4 2018-01-09

MKFILE      = Makefile
DEPFILE     = ${MKFILE}.dep
NOINCL      = ci clean spotless
NEEDINCL    = ${filter ${NOINCL}, ${MAKECMDGOALS}}
GMAKE       = ${MAKE} --no-print-directory
COMPILECPP  = g++ -g -O0 -Wall -Wextra -rdynamic -std=gnu++17
MAKEDEPCPP  = g++ -MM -std=gnu++14

CPPHEADER   = hello.h
CPPSOURCE   = hello.cpp main.cpp
ALLCPPSRC   = ${CPPHEADER} ${CPPSOURCE}
OBJECTS     = ${CPPSOURCE:.cpp=.o}
EXECBIN     = hello
OTHERS      = ${MKFILE}
ALLSOURCES  = ${ALLCPPSRC} ${OTHERS}
LISTING     = Listing.ps

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${COMPILECPP} -o $@ ${OBJECTS}

%.o : %.cpp
	${COMPILECPP} -c $<

ci : ${ALLSOURCES}
	- checksource ${ALLSOURCES}
	cid + ${ALLSOURCES}

lis : ${ALLSOURCES} test file
	mkpspdf ${LISTING} ${ALLSOURCES} ${DEPFILE} test.out file.out

clean :
	- rm ${OBJECTS} ${DEPFILE} core test.out

spotless : clean
	- rm ${EXECBIN} ${LISTING} ${LISTING:.ps=.pdf}

test : ${EXECBIN}
	./${EXECBIN} foo bar baz qux >test.out 2>&1

file : ${EXECBIN}
	file * >file.out 2>&1


dep : ${ALLCPPSRC}
	@ echo "# ${DEPFILE} created `LC_TIME=C date`" >${DEPFILE}
	${MAKEDEPCPP} ${CPPSOURCE} >>${DEPFILE}

${DEPFILE} : ${MAKEFILE}
	@ touch ${DEPFILE}
	${GMAKE} dep

again :
	${GMAKE} spotless dep ci all lis

ifeq (${NEEDINCL}, )
include ${DEPFILE}
endif
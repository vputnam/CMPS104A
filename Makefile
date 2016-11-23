 
#Rashmi Sehgal rasehgal
#Vanessa Putnam vputnam 
#5/9/16
#Assignment 3 


GCC        = g++ -g -O0 -Wall -Wextra -std=gnu++14
MKDEP      = g++ -MM -std=gnu++14
VALGRIND   = valgrind --leak-check=full --show-reachable=yes

MKFILE     = Makefile
DEPFILE    = Makefile.dep
SOURCES    = auxlib.cpp main.cpp stringset.cpp emitter.cpp\
astree.cpp lyutils.cpp typecheck.cpp symtable.cpp symstack.cpp 
HEADERS    = auxlib.h stringset.h astree.h emitter.h\
lyutils.h typecheck.h symtable.h symstack.h
LSOURCES   = scanner.l
YSOURCES   = parser.y
CLGEN      = yylex.cc
HYGEN      = yyparse.h
CYGEN      = yyparse.cc
CGENS      = ${CLGEN} ${CYGEN}
ALLGENS    = ${HYGEN} ${CGENS}
EXECBIN    = oc
OBJECTS    = ${SOURCES:.cpp=.o} ${CGENS:.cc=.o}
LREPORT    = yylex.output
YREPORT    = yyparse.output
SRCFILES   = ${HEADERS} ${SOURCES} ${LSOURCES} ${YSOURCES} ${MKFILE}
SMALLFILES = ${DEPFILE} foo.oc foo1.oh foo2.oh
CHECKINS   = ${SRCFILES} ${SMALLFILES}
LISTING    = Listing.ps
TESTINS    = ${wildcard test*.in}
LISTSRC    = ${SRCFILES} ${HYGEN}

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${GCC} -o${EXECBIN} ${OBJECTS}

${OBJECTS} : ${SOURCES} ${CGENS}
	${GCC} -c ${SOURCES}
	${GCC} -c ${CGENS}

${CLGEN} : ${LSOURCES}
		flex --outfile=${CLGEN} ${LSOURCES} 2>${LREPORT}
		- grep -v '^  ' ${LREPORT}

${CYGEN} ${HYGEN} : ${YSOURCES}
		bison --defines=${HYGEN} --output=${CYGEN} ${YSOURCES}

%.o : %.cpp
	${GCC} -c $<

ci :
	cid + ${CHECKINS}
	checksource ${CHECKINS}

clean :
	- rm ${OBJECTS}

spotless : clean
	- rm ${EXECBIN} ${LISTING} ${LISTING:.ps=.pdf} ${DEPFILE} \
	     test.out test.err misc.lis

${DEPFILE} :
	${MKDEP} ${SOURCES} >${DEPFILE}

dep :
	- rm ${DEPFILE}
	${MAKE} --no-print-directory ${DEPFILE}

test : ${EXECBIN}
	${VALGRIND} ${EXECBIN} foo.oc 1>test.out 2>test.err

misc.lis : ${DEPFILE} foo.oc foo1.oh foo2.oh
	catnv ${DEPFILE} foo.oc foo1.oh foo2.oh >misc.lis

lis : misc.lis test
	mkpspdf ${LISTING} ${SRCFILES} misc.lis test.out test.err

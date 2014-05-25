# $Id$
CC      = gcc
CFLAGS  = -g -O0 -Wall -Wextra -std=gnu99
LEXFILE = scanner.l
LCGEN   = yylex.c
LRPT    = yylex.output
YACFILE = parser.y
YCGEN   = yyparse.c
YHGEN   = yyparse.h
YRPT    = yyparse.output
HDRS    = auxlib.h

all : oc
oc : oc.o auxlib.o stringtable.o
	${CC} ${CFLAGS} oc.o auxlib.o stringtable.o -o oc

oc.o : oc.c
	${CC} ${CFLAGS} -c oc.c

auxlib.o : auxlib.c auxlib.h
	${CC} ${CFLAGS} -c auxlib.c

stringtable.o : stringtable.c stringtable.h
	${CC} ${CFLAGS} -c stringtable.c

ci :
	- git add *.c *.h Makefile
	- git commit -m "Automated commmit on `date`"
	- git push

clean :
	- rm *.o

spotless : clean
	rm -fr *core oc

#	#cid + Makefile
#
#${LCGEN} : ${LEXFILE}
#	flex --outfile=${LCGEN} ${LEXFILE} 2>${LRPT}
#	- grep -v '^ ' ${LRPT}
#
#${YCGEN} ${YHGEN} : ${YACFILE}
#	bison --defines=${YHGEN} --output=${YCGEN} ${YACFILE}
#
#
#parser.c parser.h : parser.y
#	#cid + parser.y
#	bison -dtv parser.y -o parser.c
#
#printtokens : printtokens.c parser.c parser.h
#	#cid + printtokens.c
#	${GCC} printtokens.c parser.c -o printtokens
#
#% : %.c
#	#cid + $<
#	${GCC} $< -o $@
#
#clean :
#	- rm parser.o parser.h parser.c \
#	     printtokens printtokens.o printtokens.output \
#	     parser.output printtokens.output
#
#ci :
#	#cid + make-kw-table.perl \
#	    parser.y printtokens.c Makefile \
#
#%.output : %
#	./$< >$@
#

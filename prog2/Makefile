// $Id: Makefile.v 1.4 2018-01-08

MKFILE		= Makefile
DEPFILE		= $(MKFILE).dep
NOINCL		= ci clean spotless
NEEDINCL	= $(filter $(NOINCL), $(MAKECMDGOALS))
GMAKE		= $(MAKE) --no-print-directory
COMPILECPP	= g++ -std=gnu++17 -g -O0 -Wall -Wextra -Wold-style-cast
MAKEDEPCPP	= g++ -std=gnu++17 -MM

MODULES		= ubigint bigint libfns scanner debug util
CPPHEADER	= $(MODULES:=.h) iterstack.h relops.h
CPPSOURCE	= $(MODULES:=.cpp) main.cpp
EXEBIN		= ydc
OBJECTS		= $(CPPSOURCE:.cpp=.o) 
MODULESSRC	= $(foreach MOD, $(MODULES), $(MOD).h $(MOD).cpp)
OTHERSRC	= $(filter-out $(MODULESSRC), $(CPPHEADER) $(CPPSOURCE))
ALLSOURCES	= $(MODULESSRC) $(OTHERSRC) $(MKFILE)
LISTING		= Listing.ps

all : $(EXEBIN)

$(EXEBIN) : $(OBJECTS)
	$(COMPILECPP) -o $@ $(OBJECTS)

%.o : %.cpp
	- cpplint.py.perl
	$(COMPILECPP) -c $<

ci : $(ALLSOURCES)
	- checksource $(ALLSOURCES)
	cid + $(ALLSOURCES)

lis : $(ALLSOURCES)
	mkpspdf $(LISTING) $(ALLSOURCES) $(DEPFILE)

clean :
	- rm $(OBJECTS) $(DEPFILE) core $(EXEBIN).errs

spotless : clean
	- rm $(EXEBIN) $(LISTING) $(LISTING:.ps=.pdf)

dep : $(CPPSOURCE) $(CPPHEADER)
	@ echo "# $(DEPFILE) created 'LC_TIME=C'" >$(DEPFILE)
	$(MAKEDEPCPP) $(CPPSOURCE) >>$(DEPFILE)

$(DEPFILE) :
	@ touch $(DEPFILE)
	$(GMAKE) dep

again :
	$(GMAKE) spotless dep ci all lis

ifeq ($(NEEDINCL), )
include $(DEPFILE)
endif
COMP = g++-9.3.0

COMMON = ../Common
INTERFACES = ../Interfaces
ALGORITHM = ../Algorithm

SHARED_OBJS = _308394642_a.so _308394642_b.so
CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG -fPIC -fvisibility=hidden
CPP_LINK_FLAG = -lstdc++fs -shared

all: $(SHARED_OBJS)

_308394642_a.so: _308394642_a.o AlgorithmsBaseClass.o $(COMMON)/Parser.o $(COMMON)/ShipPlan.o $(COMMON)/Container.o $(COMMON)/ISO6346.o $(COMMON)/ShipRoute.o $(COMMON)/Port.o
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^
_308394642_b.so: _308394642_b.o AlgorithmsBaseClass.o $(COMMON)/Parser.o $(COMMON)/ShipPlan.o $(COMMON)/Container.o $(COMMON)/ISO6346.o $(COMMON)/ShipRoute.o $(COMMON)/Port.o
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^

_308394642_a.o: _308394642_a.h _308394642_a.cpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
_308394642_b.o: _308394642_a.h _308394642_a.cpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
AlgorithmsBaseClass.o: AlgorithmsBaseClass.cpp AlgorithmsBaseClass.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
$(COMMON)/Parser.o: $(COMMON)/Parser.h $(COMMON)/Parser.cpp
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
$(COMMON)/ShipPlan.o: $(COMMON)/ShipPlan.h $(COMMON)/ShipPlan.cpp
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
$(COMMON)/Container.o: $(COMMON)/Container.h $(COMMON)/Container.cpp
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
$(COMMON)/ISO6346.o: $(COMMON)/ISO6346.cpp $(COMMON)/ISO6346.h
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
$(COMMON)/ShipRoute.o: $(COMMON)/ShipRoute.h $(COMMON)/ShipRoute.cpp
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
$(COMMON)/Port.o: $(COMMON)/Port.h $(COMMON)/Port.cpp
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp

clean:
	rm -f $(OBJS) $(SHARED_OBJS)
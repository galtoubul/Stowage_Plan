COMP = g++-9.3.0

COMMON = ../Common
INTERFACES = ../Interfaces
ALGORITHM = ../algorithm
OBJS = main.o Registrar.o Simulation.o $(COMMON)/Parser.o $(COMMON)/ShipPlan.o $(COMMON)/Container.o TasksProducer.o $(COMMON)/Errors.o \
$(COMMON)/ISO6346.o $(COMMON)/ShipRoute.o $(COMMON)/Port.o $(COMMON)/Travel.o $(COMMON)/WeightBalanceCalculator.o AlgorithmRegistration.o 

EXEC = simulator
CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG -fPIC
CPP_LINK_FLAG = -lstdc++fs -ldl -export-dynamic -pthread

$(EXEC): $(OBJS)
	$(COMP) $(OBJS) $(CPP_LINK_FLAG) -o $@

main.o: main.cpp 
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
TasksProducer.o: TasksProducer.h TasksProducer.cpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
Registrar.o: Registrar.cpp Registrar.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
Simulation.o: Simulation.h Simulation.cpp
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
$(COMMON)/Travel.o: $(COMMON)/Travel.h $(COMMON)/Travel.cpp
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
$(COMMON)/WeightBalanceCalculator.o: $(INTERFACES)/WeightBalanceCalculator.h $(COMMON)/WeightBalanceCalculator.cpp
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
$(COMMON)/Errors.o: $(COMMON)/Errors.cpp $(COMMON)/Errors.h
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
AlgorithmRegistration.o: $(INTERFACES)/AlgorithmRegistration.h AlgorithmRegistration.cpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

clean:
	rm -f $(OBJS) $(EXEC)




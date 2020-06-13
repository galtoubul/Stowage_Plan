cmake_minimum_required(VERSION 3.15)
project(ex3_308394642_205611544)

set(CMAKE_CXX_STANDARD 17)
set(GCC_COVERAGE_COMPILE_FLAGS "-std=c++2a -Wall -Wextra -pedantic-errors")
set(GCC_COVERAGE_LINK_FLAGS    "-lstdc++fs -pthread")

add_definitions(${GCC_COVERAGE_COMPILE_FLAGS} ${GCC_COVERAGE_LINK_FLAGS})

add_executable(ex3_308394642_205611544
        Algorithm/AlgorithmsBaseClass.h         Algorithm/AlgorithmsBaseClass.cpp
        Algorithm/_308394642_a.h                Algorithm/_308394642_a.cpp
        Algorithm/_308394642_b.h                Algorithm/_308394642_b.cpp
        Common/Container.h                      Common/Container.cpp
        Common/ISO6346.h                        Common/ISO6346.cpp
        Common/Parser.h                         Common/Parser.cpp
        Common/Port.h                           Common/Port.cpp
        Common/ShipPlan.h                       Common/ShipPlan.cpp
        Common/ShipRoute.h                      Common/ShipRoute.cpp
        Common/Travel.h                         Common/Travel.cpp
        Common/WeightBalanceCalculator.cpp
        Common/Errors.h                         Common/Errors.cpp
        Interfaces/AbstractAlgorithm.h
        Interfaces/AlgorithmRegistration.h
        Interfaces/WeightBalanceCalculator.h
        Simulator/AlgorithmRegistration.cpp
        Simulator/main.cpp
        Simulator/Registrar.h                   Simulator/Registrar.cpp
        Simulator/Simulation.h                  Simulator/Simulation.cpp
        Simulator/Named.h
        Simulator/TasksProducer.cpp Simulator/TasksProducer.h
        Simulator/ThreadPool.h)


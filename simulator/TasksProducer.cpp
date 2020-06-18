#include "TasksProducer.h"
using std::unique_ptr;
using std::string;
using std::cout;
using std::endl;
using std::get;
using std::lock_guard;
using std::make_tuple;
#define SEPARATOR string(1, fs::path::preferred_separator)
#define CANNOT_RUN_TRAVEL ((1 << 3) | (1 <<4) | (1 << 7) | (1 << 8))
#define SUM_AND_NUM_ERRORS 2

TasksProducer::TasksProducer(NumTravels _numTravels, NumAlgorithms _numAlgorithms,
                             vector<tuple<pair<string,function<unique_ptr<AbstractAlgorithm>()>>, Travel, pair<int,int>>> _travelAlgVec)
        : numTravels(_numTravels), numAlgorithms(_numAlgorithms), travelAlgVec(std::move(_travelAlgVec)) {
    numTasks = numTravels * numAlgorithms;
    shipPlanAndRouteMap = std::unordered_map<int,tuple<int,ShipPlan,ShipRoute>>();
}

std::optional<int> TasksProducer::next_task_index_simple() {
    if(task_counter < numTasks) {
        int next_counter = ++task_counter;

        if(next_counter <= numTasks)
            return {next_counter - 1};

        else
            --task_counter;
    }
    return {};
}

std::optional<std::function<void(void)>> TasksProducer::getTask(vector<vector<int>>& outputMat) {
    auto task_index = next_task_index_simple();
    if(task_index) {

        return [task_index, this, &outputMat] {
            std::lock_guard g{m};

            // -------------------------- initializations -------------------------- //

            auto& algorithm = get<0>(travelAlgVec[*task_index]);
            unique_ptr<AbstractAlgorithm> alg = algorithm.second();
            Travel& travel = get<1>(travelAlgVec[*task_index]);

            Simulator simulator{};
            simulator.setErrorsFileName(travel.getOutputPath() + SEPARATOR + "errors" + SEPARATOR +
                                        algorithm.first + "_" + travel.getName() + ".errors");

            // -------------------------- get input for simulator -------------------------- //

            int algInd = get<2>(travelAlgVec[*task_index]).first;
            int travelInd = get<2>(travelAlgVec[*task_index]).second;
            int travelErrors = 0;

            // didn't read data for this travel -> read shipPlan & shipRoute
            if(shipPlanAndRouteMap.find(travelInd) == shipPlanAndRouteMap.end()){
                travelErrors |= Parser::readShipPlan(get<1>(shipPlanAndRouteMap[travelInd]), travel.getShipPlanPath().string());
                travelErrors |= Parser::readShipRoute(get<2>(shipPlanAndRouteMap[travelInd]), travel.getShipRoutePath().string());
                get<0>(shipPlanAndRouteMap[travelInd]) = travelErrors;
            }

            travelErrors = get<0>(shipPlanAndRouteMap[travelInd]);
            if (simulator.cantRunTravel(travelErrors, travel.getOutputPath(), outputMat, algInd, travelInd)) return;

            simulator.setShipPlan(get<1>(shipPlanAndRouteMap[travelInd]));
            simulator.setShipRoute(get<2>(shipPlanAndRouteMap[travelInd]));

            WeightBalanceCalculator _calculator;
            alg->setWeightBalanceCalculator(_calculator);
            simulator.setWeightBalanceCalculator(_calculator);

            // -------------------------- get input for algorithm -------------------------- //

            int errorsOfAlgorithm = 0;  // errorsOfAlgorithm != 0 iff we have errors in the given input / the algorithm made errors
            errorsOfAlgorithm |= alg->readShipPlan(travel.getShipPlanPath().string());
            errorsOfAlgorithm |= alg->readShipRoute(travel.getShipRoutePath().string());
            if (simulator.cantRunTravel(travelErrors, travel.getOutputPath(), outputMat, algInd, travelInd)) return;

            // -------------------------- run algorithm - travel pair -------------------------- //

            string algorithmErrorString;
            int algActionsCounter = 0;
            errorsOfAlgorithm |= simulator.startTravel(alg.get(), algorithm.first, travel, algorithmErrorString, travel.getOutputPath(), algActionsCounter);

            simulator.writeErrors(errorsOfAlgorithm, travel, outputMat, algInd, travelInd, algorithmErrorString);

            // updating simulation.results' Mat
            outputMat[algInd][travelInd] = algActionsCounter;
            outputMat[algInd][outputMat[algInd].size() - SUM_AND_NUM_ERRORS] += algActionsCounter;

            std::this_thread::yield();
        };
    }
    else return {};
}

//std::optional<std::function<void(void)>> TasksProducer::getTask(vector<vector<int>>& outputMat) {
//    auto task_index = next_task_index_simple();
//    if(task_index) {
//
//        return [task_index, this, &outputMat] {
//            std::lock_guard g{m}; // TODO: make it tighter?
//
//            // initializations
//            auto& algorithm = get<0>(travelAlgVec[*task_index]);
//            unique_ptr<AbstractAlgorithm> alg = algorithm.second();
//            Travel& travel = get<1>(travelAlgVec[*task_index]);
//
//            Simulator simulator{};
//            simulator.setErrorsFileName(travel.getOutputPath() + SEPARATOR + "errors" + SEPARATOR +
//                                        algorithm.first + "_" + to_string(travel.getIndex()) + ".errors");
//
//            // get input for simulator
//            int algInd = get<2>(travelAlgVec[*task_index]).first;
//            int travelInd = get<2>(travelAlgVec[*task_index]).second;
//            int travelErrors = 0;
//            ShipPlan shipPlan{};
//            ShipRoute shipRoute{};
//
//            if(get<0>(shipPlanAndRouteVec[travelInd]) != UNINITIALIZED){
//                simulator.setShipPlan(get<1>(shipPlanAndRouteVec[travelInd]));
//                simulator.setShipRoute(get<2>(shipPlanAndRouteVec[travelInd]));
//            }
//
//            else{
//                travelErrors |= Parser::readShipPlan(shipPlan, travel.getShipPlanPath().string());
//                travelErrors |= Parser::readShipRoute(shipRoute, travel.getShipRoutePath().string());
//                shipPlanAndRouteVec[travelInd] = make_tuple(travelErrors, shipPlan, shipRoute);
//            }
//
//            if (simulator.cantRunTravel(travelErrors, travel.getOutputPath(), outputMat, algInd, travelInd)) return;
//
//            WeightBalanceCalculator _calculator;
//            alg->setWeightBalanceCalculator(_calculator);
//            simulator.setWeightBalanceCalculator(_calculator);
//
//            // get input for algorithm
//            int errorsOfAlgorithm = 0;  // errorsOfAlgorithm != 0 iff we have errors in the given input / the algorithm made errors
//            errorsOfAlgorithm |= alg->readShipPlan(travel.getShipPlanPath().string());
//            errorsOfAlgorithm |= alg->readShipRoute(travel.getShipRoutePath().string());
//            if (simulator.cantRunTravel(travelErrors, travel.getOutputPath(), outputMat, algInd, travelInd)) return;
//
//            // run algorithm - travel pair
//            string algorithmErrorString;
//            int algActionsCounter = 0;
//            errorsOfAlgorithm |= simulator.startTravel(alg.get(), algorithm.first, travel, algorithmErrorString, travel.getOutputPath(), algActionsCounter);
//
//            simulator.writeErrors(errorsOfAlgorithm, travel, outputMat, algInd, travelInd, algorithmErrorString);
//
//            // updating simulation.results' Mat
//            outputMat[algInd][travelInd] = algActionsCounter;
//            outputMat[algInd][outputMat[algInd].size() - SUM_AND_NUM_ERRORS] += algActionsCounter;
//
//            std::this_thread::yield();
//        };
//    }
//    else return {};
//}

//std::optional<std::function<void(void)>> TasksProducer::getTask(vector<vector<int>>& outputMat) {
//    auto task_index = next_task_index_simple();
//    if(task_index) {
//
//        return [task_index, this, &outputMat] {
//            std::lock_guard g{m};
//
//            // initializations
//            auto& algorithm = get<0>(travelAlgVec[*task_index]);
//            unique_ptr<AbstractAlgorithm> alg = algorithm.second();
//            Travel& travel = get<1>(travelAlgVec[*task_index]);
//
//            Simulator simulator{};
//            simulator.setErrorsFileName(travel.getOutputPath() + SEPARATOR + "errors" + SEPARATOR +
//                                        algorithm.first + "_" + to_string(travel.getIndex()) + ".errors");
//
//            // get input for simulator
//            int travelErrors = simulator.getInput(travel.getShipPlanPath().string(), travel.getShipRoutePath().string());
//            int algInd = get<2>(travelAlgVec[*task_index]).first;
//            int travelInd = get<2>(travelAlgVec[*task_index]).second;
//            if (simulator.cantRunTravel(travelErrors, travel.getOutputPath(), outputMat, algInd, travelInd)) return;
//
//            WeightBalanceCalculator _calculator;
//            alg->setWeightBalanceCalculator(_calculator);
//            simulator.setWeightBalanceCalculator(_calculator);
//
//            // get input for algorithm
//            int errorsOfAlgorithm = 0;  // errorsOfAlgorithm != 0 iff we have errors in the given input / the algorithm made errors
//            errorsOfAlgorithm |= alg->readShipPlan(travel.getShipPlanPath().string());
//            errorsOfAlgorithm |= alg->readShipRoute(travel.getShipRoutePath().string());
//            if (simulator.cantRunTravel(travelErrors, travel.getOutputPath(), outputMat, algInd, travelInd)) return;
//
//            // run algorithm - travel pair
//            string algorithmErrorString;
//            int algActionsCounter = 0;
//            errorsOfAlgorithm |= simulator.startTravel(alg.get(), algorithm.first, travel, algorithmErrorString, travel.getOutputPath(), algActionsCounter);
//
//            simulator.writeErrors(errorsOfAlgorithm, travel, outputMat, algInd, travelInd, algorithmErrorString);
//
//            // updating simulation.results' Mat
//            outputMat[algInd][travelInd] = algActionsCounter;
//            outputMat[algInd][outputMat[algInd].size() - SUM_AND_NUM_ERRORS] += algActionsCounter;
//
//            std::this_thread::yield();
//        };
//    }
//    else return {};
//}

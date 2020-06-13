#include "TasksProducer.h"
using std::unique_ptr;
using std::string;
using std::cout;
using std::endl;
using std::get;
#define SEPARATOR string(1, fs::path::preferred_separator)
#define CANNOT_RUN_TRAVEL ((1 << 3) | (1 <<4) | (1 << 7) | (1 << 8))

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
            auto& algorithm = get<0>(travelAlgVec[*task_index]);
            unique_ptr<AbstractAlgorithm> alg = algorithm.second();
            Travel& travel = get<1>(travelAlgVec[*task_index]);

            Simulator simulator{};
            simulator.setErrorsFileName(travel.getOutputPath() + SEPARATOR + "errors" + SEPARATOR +
                                        algorithm.first + "_" + to_string(travel.getIndex()) + ".errors");
            int travelErrors = simulator.getInput(travel.getShipPlanPath().string(), travel.getShipRoutePath().string());
            int algInd = get<2>(travelAlgVec[*task_index]).first;
            int travelInd = get<2>(travelAlgVec[*task_index]).second;

            if (simulator.cantRunTravel(travelErrors, travel.getOutputPath(), outputMat, algInd, travelInd)) return;

            WeightBalanceCalculator _calculator;
            alg->setWeightBalanceCalculator(_calculator);
            simulator.setWeightBalanceCalculator(_calculator);

            // errorsOfAlgorithm != 0 iff we have errors in the given input / the algorithm made errors
            int errorsOfAlgorithm = 0;
            errorsOfAlgorithm |= alg->readShipPlan(travel.getShipPlanPath().string());
            errorsOfAlgorithm |= alg->readShipRoute(travel.getShipRoutePath().string());
            if (simulator.cantRunTravel(travelErrors, travel.getOutputPath(), outputMat, algInd, travelInd)) return;

            string algorithmErrorString;
            int algActionsCounter = 0;
            errorsOfAlgorithm |= simulator.startTravel(alg.get(), algorithm.first, travel, algorithmErrorString, travel.getOutputPath(), algActionsCounter);

            simulator.writeErrors(errorsOfAlgorithm, travel, outputMat, algInd, travelInd, algorithmErrorString);

            // updating simulation.results' Mat
            outputMat[algInd][travelInd] = algActionsCounter;
            outputMat[algInd][outputMat[algInd].size() - 2] += algActionsCounter;

            std::this_thread::yield();
        };
    }
    else return {};
}

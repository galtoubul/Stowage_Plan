#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#include <cstdlib>
#include <iostream>
#include <map>
#include <filesystem>
#include <unordered_map>
#include <dlfcn.h>
#include "Registrar.h"
#include "ThreadPool.h"
using std::cout;
using std::cin;
using std::endl;
using std::unique_ptr;
using std::pair;
using std::get;
using std::function;
using std::unordered_map;
namespace fs = std::filesystem;
#define SEPARATOR string(1, fs::path::preferred_separator)
#define CANNOT_RUN_TRAVEL ((1 << 3) | (1 <<4) | (1 << 7) | (1 << 8))
#define SUM_AND_NUM_ERRORS 2
#define ERROR -1
#define VALID 1

inline void clearData(ShipPlan& shipPlan, ShipRoute& shipRoute){
    const_cast<VVVC&>(shipPlan.getContainers()).clear();
    const_cast<vector<Port>&>(shipRoute.getPortsList()).clear();
}

int dynamicallyLoadAlgorithms(vector<fs::path>& algorithmPaths, Registrar& registrar){
    for (auto& algoPath : algorithmPaths){
        string path = algoPath.string();
        string algorithmName = path.substr(path.find_last_of(SEPARATOR) + 1, path.size() - path.find_last_of(SEPARATOR) - 4);
        string error;
        if (!registrar.loadAlgorithmFromFile(path.c_str(), error, algorithmName)) {
            std::cerr << error << '\n';
            return ERROR;
        }
    }
    return VALID;
}

std::map<int, std::string> Errors::errorsMap;

// --------------------- paths --------------------- //

string getCurrentDir() {
    char buff[FILENAME_MAX]; //create string buffer to hold path
    GetCurrentDir(buff, FILENAME_MAX);
    string currentWorkingDir(buff);
    return currentWorkingDir;
}

vector<fs::path> getAlgorithmsPaths(const string& algorithmPath){
    vector<fs::path> algorithmsPaths;
    std::error_code ec;
    for (const auto & entry : fs::directory_iterator(algorithmPath, ec)){
        string fileName = entry.path().string();
        if(string(".so") == (fileName.string::substr(fileName.size() - string(".so").size())))
            algorithmsPaths.emplace_back(fileName);
    }
    return algorithmsPaths;
}

vector<fs::path> getPaths (int argc, char** argv, string& travelPath, string& algorithmPath, string& output, int& numThreads){
    vector<string> args(argv, argv + argc);
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i] == "-algorithm_path")
            algorithmPath = args[i + 1];
        if (args[i] == "-output")
            output = args[i + 1];
        if (args[i] == "-travel_path")
            travelPath = args[i + 1];
        if (args[i] == "-num_threads")
            numThreads = stoi(args[i + 1]);
    }

    if (numThreads < 1) numThreads = 1;

    if (output.empty())
        output = getCurrentDir();

    if (algorithmPath.empty())
        algorithmPath = getCurrentDir();

    return getAlgorithmsPaths(algorithmPath);
}

int checkValidTravelPath(const string& travelPath){
    if(travelPath.empty()){
        cout << "Fatal Error: missing -travel_path argument. Exiting..." << endl;
        return ERROR;
    }
    return VALID;
}

// --------------------- simulation results --------------------- //

bool checkIfValidTravel(vector<vector<int>>& outputMat, unsigned travelInd){
    for (int row = 0; row < (int) outputMat.size(); ++row)
        if (outputMat[row][travelInd] == NON_VALID_TRAVEL)
            return false;
    return true;
}

ofstream initSimulationResults(const string& output, vector<Travel>& travels, vector<vector<int>>& outputMat){
    ofstream simulationResults(output + SEPARATOR + "simulation.results");
    if(simulationResults.is_open()){
        simulationResults << "RESULTS,";
        for(int travelInd = 0; travelInd < (int) travels.size(); travelInd++)
            if(checkIfValidTravel(outputMat, travelInd))
                simulationResults << travels[travelInd].getName() +",";

        simulationResults << "SUM,";
        simulationResults << "NumErrors\n";
    }
    return simulationResults;
}

// --------------------- multi threading --------------------- //

void initTravelAlgVec(vector<tuple<pair<string,function<unique_ptr<AbstractAlgorithm>()>>, Travel, pair<int,int>>>& travelAlgVec,
                      unordered_map<string,function<unique_ptr<AbstractAlgorithm>()>>& algorithmMap,
                      vector<Travel>& travelsVec, vector<tuple<string,vector<int>,int,int>>& outputVector){
    int algorithmInd = 0, travelInd;
    for(auto & it : algorithmMap){
        travelInd = 0;
        for (auto & travel : travelsVec){
            travelAlgVec.push_back({{it.first,it.second}, travel, {algorithmInd, travelInd}});
            travelInd++;
            get<0>(outputVector[algorithmInd]) = it.first;
        }
        algorithmInd++;
    }
}

// deleting columns of non legal travels
void updateOutputMat(vector<vector<int>>& outputMat){
    if(outputMat.empty()) return;
    for (int col = 0; col < (int) outputMat[0].size(); ++col) {
        for (int row = 0; row < (int) outputMat.size(); ++row) {
            if(outputMat[row][col] == NON_VALID_TRAVEL){
                unsigned columnToDelete = col;
                for (int i = 0; i < (int) outputMat.size(); ++i)
                    if (outputMat[i].size() > columnToDelete)
                        outputMat[i].erase(outputMat[i].begin() + columnToDelete);
            }
        }
    }
}

void updateOutputVector(vector<vector<int>>& outputMat, vector<tuple<string,vector<int>,int,int>>& outputVector){
    updateOutputMat(outputMat);
    for (int i = 0; i < (int) outputMat.size(); ++i){
        get<1>(outputVector[i]).assign(outputMat[0].size() - 2, 0);
        int j = 0;
        for (j = 0; j < (int) outputMat[0].size() - 2; ++j)
            get<1>(outputVector[i])[j] = (outputMat[i][j]);

        get<2>(outputVector[i]) = outputMat[i][j]; // sum of operations
        get<3>(outputVector[i]) = outputMat[i][++j]; // num of errors
    }
}

void runOnlyMain(vector<tuple<pair<string,function<unique_ptr<AbstractAlgorithm>()>>, Travel, pair<int,int>>> travelAlgVec, vector<vector<int>> outputMat){
    for(int task_index = 0; task_index < (int) travelAlgVec.size(); task_index++){
        auto& algorithm = get<0>(travelAlgVec[task_index]);
        auto& travel = get<1>(travelAlgVec[task_index]);
        unique_ptr<AbstractAlgorithm> alg = algorithm.second();
        Simulator simulator{};
        simulator.setErrorsFileName(travel.getOutputPath() + SEPARATOR + "errors" + SEPARATOR +
                                    algorithm.first + "_" + to_string(travel.getIndex()) + ".errors");
        int travelErrors = simulator.getInput(travel.getShipPlanPath().string(), travel.getShipRoutePath().string());

        int algInd = get<2>(travelAlgVec[task_index]).first;
        int travelInd = get<2>(travelAlgVec[task_index]).second;

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
    }
}

// --------------------- main --------------------- //

int main(int argc, char** argv) {
    // ----------------------- initializations ----------------------- //

    string travelPath, algorithmPath, output;
    int numThreads = 1;
    vector<fs::path> algorithmPaths = getPaths(argc, argv, travelPath, algorithmPath, output, numThreads);
    if(checkValidTravelPath(travelPath) == ERROR) return EXIT_FAILURE;
    Errors::populateErrorsMap();

    vector<Travel> travelsVec;
    initTravelsVec(travelsVec, travelPath, output);

    // ----------------------- loading algorithms ----------------------- //

    auto& registrar = Registrar::getRegistrar();
    if(dynamicallyLoadAlgorithms(algorithmPaths, registrar) == ERROR) return EXIT_FAILURE;
    unordered_map<string,function<unique_ptr<AbstractAlgorithm>()>> algorithmMap = registrar.getAlgorithmMap();

    // ----------------------- thread pool preparations ----------------------- //

    // containers for the simulator's output
    vector<tuple<string,vector<int>,int,int>> outputVector(algorithmMap.size());
    vector<vector<int>> outputMat(algorithmMap.size(), vector<int>(travelsVec.size() + SUM_AND_NUM_ERRORS, 0));

    vector<tuple<pair<string,function<unique_ptr<AbstractAlgorithm>()>>, Travel, pair<int,int>>> travelAlgVec;
    initTravelAlgVec(travelAlgVec, algorithmMap, travelsVec, outputVector);

    // ----------------------- execution ----------------------- //

    if(numThreads > 1){
        size_t numTasks = travelsVec.size() * algorithmMap.size();
        ThreadPool threadPool {TasksProducer{NumTasks{numTasks}, travelAlgVec},
                               NumThreads{numThreads}, outputMat };
        threadPool.start();
        threadPool.wait_till_finish();
    } else runOnlyMain(travelAlgVec, outputMat);

    // ----------------------- creating simulations results file ----------------------- //

    ofstream simulationResults = initSimulationResults(output, travelsVec, outputMat);
    updateOutputVector(outputMat, outputVector);
    createSimulationResults(simulationResults, outputVector);

    return EXIT_SUCCESS;
}

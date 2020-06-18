/**
 * Class for Tasks Producer
 * Creates tasks for the thread pool
 * task = algorithm - travel pair run
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <optional>
#include <functional>
#include <chrono>
#include  "Named.h"
#include "../Interfaces/AbstractAlgorithm.h"
#include "Simulation.h"
using std::function;
using std::pair;
using std::vector;
using std::unique_ptr;
using std::tuple;
//#define UNINITIALIZED -200

class TasksProducer {
    const int numTravels = 0;
    const int numAlgorithms = 0;
    int numTasks = -1;
    vector<tuple<pair<string,function<unique_ptr<AbstractAlgorithm>()>>, Travel, pair<int,int>>> travelAlgVec;
    std::atomic_int task_counter = 0;
    std::mutex m;
    std::unordered_map<int,tuple<int,ShipPlan,ShipRoute>> shipPlanAndRouteMap;


    std::optional<int> next_task_index_simple();

public:
    TasksProducer(NumTravels _numTravels, NumAlgorithms _numAlgorithms,
                  vector<tuple<pair<string,function<unique_ptr<AbstractAlgorithm>()>>, Travel, pair<int,int>>> _travelAlgVec);

    TasksProducer(TasksProducer&& other) //TODO: why do we need this?
            : numTravels(other.numTravels), numAlgorithms(other.numAlgorithms), numTasks(other.numTasks),
              travelAlgVec(std::move(other.travelAlgVec)), task_counter(other.task_counter.load()) {}

    std::optional<std::function<void(void)>> getTask(vector<vector<int>>& outputMat);
};


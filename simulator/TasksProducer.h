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

class TasksProducer {
    const int numTasks = -1;
    std::atomic_int task_counter = 0;
    vector<tuple<pair<string,function<unique_ptr<AbstractAlgorithm>()>>, Travel, pair<int,int>>> travelAlgVec;
    std::mutex m;

    std::optional<int> next_task_index_simple();

public:
    TasksProducer(NumTasks _numTasks,
                  vector<tuple<pair<string,function<unique_ptr<AbstractAlgorithm>()>>, Travel, pair<int,int>>> _travelAlgVec)
            : numTasks(_numTasks), travelAlgVec(std::move(_travelAlgVec)) {}

    TasksProducer(TasksProducer&& other)
            : numTasks(other.numTasks), task_counter(other.task_counter.load()), travelAlgVec(std::move(other.travelAlgVec)) {}

    std::optional<std::function<void(void)>> getTask(vector<vector<int>>& outputMat);
};


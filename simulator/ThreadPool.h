/**
 * Class for Thread Pool
 * Runs algorithm - travel pair given by the tasks producer
 * Runs num of threads as given in its initialization
 */

#include<iostream>
#include<thread>
#include<mutex>
#include<atomic>
#include<vector>
#include<optional>
#include<functional>
#include<chrono>
#include "TasksProducer.h"
using std::cout;
using std::endl;

template<typename Producer>
class ThreadPool {
    Producer producer;
    const int numThreads = -1;
    std::vector<std::thread> workers;
    std::atomic_bool running = false;
    std::atomic_bool stopped = false;
    static thread_local int num_tasks_finished;
    std::atomic_int total_num_tasks_finished { 0 };
    vector<vector<int>>& outputMat;

    void worker_function() {
        while(!stopped) {
            auto task = producer.getTask(outputMat);
            if(!task) break;
            (*task)();
            ++num_tasks_finished;
            ++total_num_tasks_finished;
        }
    }

public:
    ThreadPool(Producer producer, NumThreads numThreads, vector<vector<int>>& _outputMat)
            : producer(std::move(producer)), numThreads(numThreads), outputMat(_outputMat) {
        workers.reserve(numThreads);
    }

    bool start() {
        bool running_status = false;
        if(!running.compare_exchange_strong(running_status, true))
            return false;

        for(int i = 0; i < numThreads; ++i)
            workers.push_back(std::thread([this]{worker_function();}));

        return true;
    }

    void wait_till_finish() {
        for(auto& t : workers)
            t.join();
    }
};

template<typename Producer>
thread_local int ThreadPool<Producer>::num_tasks_finished { 0 };

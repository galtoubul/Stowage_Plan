/**
 * Class for the representation of the Registrar
 * The registrar is in charge of registration of all algorithms at the the -algorithm_path
 */

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include "../Interfaces/AbstractAlgorithm.h"
using std::function;
using std::unique_ptr;
using std::vector;

class Registrar{
    struct DlCloser{
        void operator()(void *dlhandle) const noexcept;
    };

    static Registrar registrar;
    vector<function<unique_ptr<AbstractAlgorithm>()>> factoryVec;
    std::map<std::string, std::unique_ptr<void, DlCloser>> handles;
    vector<std::string> algNamesVec;
    std::unordered_map<std::string, function<unique_ptr<AbstractAlgorithm>()>> algorithmMap;
    vector<std::string> notRegisteredAlgVec;

    // iteration over factories
    typedef decltype(factoryVec)::const_iterator const_iterator;

    Registrar() = default;
    Registrar(const Registrar&) = delete;
    Registrar& operator=(const Registrar&) = delete;
    ~Registrar();

    // AlgorithmRegistration needs to internally registerFactory
    friend class AlgorithmRegistration;

    inline void registerAlgorithmFactory(function<unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
        factoryVec.push_back(algorithmFactory);
    }
public:
    static Registrar& getRegistrar();

    // Returns an iterator to the first algorithm factory
    inline const_iterator begin() const { return factoryVec.begin(); }

    // Returns an iterator that points to the end
    inline const_iterator end() const { return factoryVec.end(); }

    // Loads an algorithm from given shared object file into the registrar
    // parameters:
    //   file_path - shared object file path to load
    //   error - On failure, it will be filled with a Human-readable error message.
    // Returns:
    //   true if given shared object's algorithm was registered successfully, false oterwise
    bool loadAlgorithmFromFile(const char *file_path, std::string& error, const std::string& algName);

    vector<function<unique_ptr<AbstractAlgorithm>()>>& getAlgorithmFactoryVector(){
        return factoryVec;
    }

    std::unordered_map<std::string, function<unique_ptr<AbstractAlgorithm>()>> getAlgorithmMap(){ return algorithmMap;}
};

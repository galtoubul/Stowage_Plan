/**
 * Base class for algorithms: _308394642_a and _308394642_b
 * Contains the functions for:
 * 1. Getting input for algorithms
 * 2. Calculating algorithms operations: loading and unloading containers
 * 3. Writhing instructions to files
 */

#pragma once

#include <tuple>
#include <string>
#include <vector>
#include "../Interfaces/AbstractAlgorithm.h"
#include "../Common/Parser.h"
using std::vector;
using std::tuple;
using std::string;

class AlgorithmsBaseClass : public AbstractAlgorithm {
protected:
    ShipPlan shipPlan;
    ShipRoute shipRoute;
    WeightBalanceCalculator calculator;
    int errors;

public:
    AlgorithmsBaseClass() : shipPlan(), shipRoute(), calculator() {}

    //------------ Overridden functions from AbstractAlgorithm--------------//

    int readShipPlan(const std::string& full_path_and_file_name) override{
        errors |= Parser::readShipPlan(shipPlan, full_path_and_file_name);
        return errors;
    }

    int readShipRoute(const std::string& full_path_and_file_name) override{
        errors |= Parser::readShipRoute(shipRoute, full_path_and_file_name);
        return errors;
    }

    int setWeightBalanceCalculator(WeightBalanceCalculator& calculator) override;
    int getInstructionsForCargo(const std::string& input_full_path_and_file_name, const std::string& output_full_path_and_file_name) override;

    //------------ main functions --------------//

    void getUnloadingInstructions(vector<INSTRUCTION>& instructions, int currPortIndex);
    void unloadToPort(Container& container, vector<INSTRUCTION>& instructions, Port& port);
    void getLoadingInstructions(vector<INSTRUCTION>& instructions, vector<Container>& containersAwaitingAtPort, int currPortIndex);
    void loadToShip(Container& container, vector<INSTRUCTION>& instructions, int currPortIndex);

    // This function is virtual because it makes the difference between the 2 algorithms: the way they chose an empty spot
    virtual tuple<int,int,int> findEmptySpot(int x, int y) = 0;

    //------------ utility --------------//

    int calcVisitNum (const string& input_full_path_and_file_name);
    tuple<int,int,int> findLoc (const string& containerID);
};

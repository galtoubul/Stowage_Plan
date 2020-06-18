/**
 * Class for the representation of the Simulator
 * Contains all of its related functions:
 * 1. Finding travels' paths
 * 2. Getting input
 * 3. Running algorithm - travel pair
 * 4. Validation of algorithm's actions
 * 5. Creating of output files: simulations.results and errors
 */

#pragma once

#include <vector>
#include <tuple>
#include <string>
#include <memory>
#include <functional>
#include <string>
#include "../Interfaces/AbstractAlgorithm.h"
#include "../Common/Parser.h"
using std::vector;
using std::tuple;
using std::unique_ptr;
#define NON_VALID_TRAVEL -100

class Simulator{
    ShipPlan shipPlan;
    ShipRoute shipRoute;
    WeightBalanceCalculator calculator;
    string errorsFileName;

public:
    Simulator() : shipPlan(), shipRoute() {}

    // ------------------- setters ----------------------- //

    void setShipPlan(const ShipPlan& _shipPlan);
    void setShipRoute(const ShipRoute& _shipRoute);
    void setWeightBalanceCalculator(WeightBalanceCalculator& _calculator);
    void setErrorsFileName(const string& _errorsFileName){ errorsFileName = _errorsFileName; }

    // ------------------- getters ----------------------- //

    string getErrorsFileName(){ return errorsFileName; }
    int getInput(const string& shipPlanFileName, const string& shipRouteFileName);
    ShipPlan& getShipPlan();
    ShipRoute& getShipRoute();

    // ------------------- main functions ----------------------- //

    // Runs a travel for an algorithm - travel pair
    int startTravel (AbstractAlgorithm* algorithm, const string& algName, Travel& travel, string& algorithmErrorString,
                     const string& output, int& algActionsCounter);

    // validations:
    int checkAndCountAlgorithmActions(vector<Container>& containersAwaitingAtPort, const string& outputFileName,
                                      const string& currPortSymbol, string& algorithmErrorString, int currPortIndex, int& algActionsCounter);
    int checkLoadInstruction(int x, int y, int floor, Container& container, string& algorithmErrorString);
    int checkUnloadInstruction(int x, int y, int floor, Container& container, string& algorithmErrorString);
    int checkMoveInstruction(int x1, int y1, int floor1, int x2, int y2, int floor2, Container& container, string& algorithmErrorString);

    // errors:
    void makeTravelError(int travelErrors, const string& output, vector<vector<int>>& outputMat, int algInd, int travelInd);
    void makeAlgorithmError(ofstream& errorsFile, const string& algorithmErrorString, vector<vector<int>>& outputMat, int algInd, int travelInd);
    bool cantRunTravel(int travelErrors, const string& output, vector<vector<int>>& outputMat, int algInd, int travelInd);
    void writeErrors(int errorsOfAlgorithm, Travel& travel, vector<vector<int>>& outputMat, int algInd, int travelInd, const string& algorithmErrorString);

    // returns the number of free spots on the ship
    int freeSlotsInShip ();
};

// Returns the path for a file ending with lookFor at entry directory
fs::path getPath(fs::directory_entry entry, const string& lookFor);

// Returns a vector with all travels' paths
vector<Travel>& initTravelsVec(vector<Travel>& travelsVec, const string& travelsPath, const string& output);

// updates simulationResults with the simulation's results from outputVector
void createSimulationResults(ofstream& simulationResults, vector<tuple<string,vector<int>,int,int>> outputVector);

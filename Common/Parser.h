/**
 * Module for common functions used by the simulator and the algorithms
 * Most functions related to parsing text, validation, writing to files
 */

#include <fstream>
#include <vector>
#include <list>
#include <tuple>
#include <string>
#include "ShipRoute.h"
#include "Travel.h"
#include "../Interfaces/WeightBalanceCalculator.h"
#include "Errors.h"
#include "ISO6346.h"
using std::ifstream;
using std::ofstream;
using std::tuple;
#define INSTRUCTION tuple<char,string,int,int,int,int,int,int>

// ------------------------ sub main functions -----------------------------------//

// Updates inputFileName and outputFileName for the port with the given portId
void getPortFilesName(string& inputFileName, string& outputFileName, const string& portId, const int portVisitNum, Travel& travel, const string& dir);

// Returns the port index related to the port with the given portSymbol(=port ID)
int findPortIndex(ShipRoute& shipRoute, const string& portSymbol, int currPortIndex);

int findCurrPortIndex(ShipRoute& shipRoute, const string& portSymbol, int visitNum);

// Update instructions to contain the instructions written in outputFileName
void getInstructionsForPort(const string& outputFileName, vector<INSTRUCTION>& instructions);

// returns the number of free spots on the ship
int freeSlotsInShip (ShipPlan& shipPlan);

void orderContainersByDest(vector<Container>& containersAwaitingAtPort, vector<Container>& sortedContainersAwaitingAtPort,
                           ShipRoute& shipRoute, int currPortIndex, int emptySlots);

// ------------------------ main functions -----------------------------------//

// Functions for getting input. Used by both, simulator and algorithms
namespace Parser{
    int readShipPlan (ShipPlan& shipPlan, const string& shipPlanFileName);

    int readShipRoute(ShipRoute& shipRoute, const string& shipPlanFileName);
}

// Writing the instructions from instructions to output_full_path_and_file_name
void writeInstructionsToFile(vector<tuple<char, string, int, int, int, int, int, int>> &instructions, const string& output_full_path_and_file_name);

// updating containersAwaitingAtPort with the containers from inputFileName
int readContainersAwaitingAtPort (const string& inputFileName, bool isFinalPort, vector<Container>& containersAwaitingAtPort,
                                  ShipPlan& shipPlan, ShipRoute& shipRoute, int currPortIndex);

#include <string>
#include <iostream>
#include <filesystem>
#include "Simulation.h"
#include <cstddef>
using std::function;
using std::vector;
using std::tuple;
using std::get;
using std::ofstream;
using std::string;
using std::cout;
using std::endl;
#define ERROR -1
#define VALID 1
#define CANNOT_RUN_TRAVEL ((1 << 3) | (1 <<4) | (1 << 7) | (1 << 8))
#define SEPARATOR string(1, fs::path::preferred_separator)

// -------------------------- utilities ----------------------------- //

inline void clearData(ShipPlan& shipPlan, ShipRoute& shipRoute){
    const_cast<VVVC&>(shipPlan.getContainers()).clear();
    const_cast<vector<Port>&>(shipRoute.getPortsList()).clear();
}

inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v"){
    s.erase(0, s.find_first_not_of(t));
    return s;
}

int Simulator::freeSlotsInShip() {
    int counter = 0;
    for (int x = 0; x < this->shipPlan.getPivotXDimension(); x++)
        for (int y = 0; y < this->shipPlan.getPivotYDimension(); y++)
            for (int floor = 0; floor < this->shipPlan.getFloorsNum(); floor++)
                if (this->shipPlan.getContainers()[x][y][floor] == nullptr)
                    counter++;
    return counter;
}

bool compareAlgoTuples(tuple<string,vector<int>,int,int> x, tuple<string,vector<int>,int,int> y){
    if (get<3>(x) == get<3>(y)){
        return (get<2>(x) < get<2>(y));
    }
    else{
        return (get<3>(x) < get<3>(y));
    }
}

// -------------------------- getters --------------------------- //

fs::path getPath(fs::directory_entry entry, const string& lookFor){
    std::error_code ec;
    for (const auto& file : fs::directory_iterator(entry, ec)) {
        string fileName = file.path().string();
        if(fileName.substr(fileName.size() - lookFor.size()) == lookFor)
            return file;
    }
    return "";
}

ShipPlan& Simulator::getShipPlan(){
    return shipPlan;
}

ShipRoute& Simulator::getShipRoute(){
    return shipRoute;
}

// -------------------------- setters --------------------------- //

void Simulator::setWeightBalanceCalculator(WeightBalanceCalculator& _calculator){
    this->calculator = _calculator;
}

vector<Travel>& initTravelsVec(vector<Travel>& travelsVec, const string& travelsPath, const string& output){
    std::error_code ec;
    int index = 1;
    for (const auto& entry : fs::directory_iterator(travelsPath, ec)){
        if(entry.is_directory()){
            fs::path shipPlanPath = getPath(entry, ".ship_plan");
            fs::path shipRoutePath = getPath(entry, ".route");
            if(shipPlanPath.string().empty() || shipRoutePath.string().empty())
                continue;
            auto found = entry.path().string().find_last_of(SEPARATOR);
            string name = entry.path().string().substr(found + 1);
            travelsVec.emplace_back(index, shipPlanPath, shipRoutePath, entry, output, name);
            index++;
        }
    }
    return travelsVec;
}

int Simulator::getInput(const string& shipPlanFileName, const string& shipRouteFileName){
    int errors = 0;
    errors |= Parser::readShipPlan(shipPlan, shipPlanFileName);
    errors |= Parser::readShipRoute(shipRoute, shipRouteFileName);
    return errors;
}

// -------------------------- validations --------------------------- //

int Simulator::checkAndCountAlgorithmActions(vector<Container>& containersAwaitingAtPort, const string& outputFileName,
                                             const string& currPortSymbol, string& algorithmErrorString, int currPortIndex, int& algActionsCounter){
    vector<INSTRUCTION> instructions;
    if(!std::filesystem::exists(outputFileName)){
        algorithmErrorString =  "ERROR: Unable to open file: " + outputFileName + "\nLeaving current travel...\n";
        return ERROR;
    }
    getInstructionsForPort(outputFileName, instructions);

    for (INSTRUCTION& instruction : instructions) {
        char instructionType;
        string containerIdBeforeTrim;
        int x1, y1, floor1, x2, y2, floor2;
        std::tie(instructionType, containerIdBeforeTrim, floor1, x1, y1, floor2, x2, y2) = instruction;
        string containerId = ltrim(containerIdBeforeTrim);

        Container* container = nullptr;
        if (instructionType == 'R')
            continue;
        else if (instructionType == 'L'){
            algActionsCounter += 5;

            // look for the loaded container at containersAwaitingAtPort
            for (auto& _container : containersAwaitingAtPort) {
                if (_container.getId() == ltrim(containerId)) {
                    container = &_container;
                    break;
                }
            }

            if (container == nullptr) {
                algorithmErrorString = Errors::buildNotLegalOperationError("Loading", containerId, floor1, x1, y1, "this container isn't exist at " + currPortSymbol);
                return ERROR;
            }

            if (checkLoadInstruction(x1, y1, floor1, *container, algorithmErrorString) == ERROR)
                return ERROR;

            containersAwaitingAtPort.erase(find(containersAwaitingAtPort.begin(), containersAwaitingAtPort.end(), *container));
            continue;
        }
        else if (instructionType == 'U'){
            algActionsCounter += 5;
            if (shipPlan.getContainers()[x1][y1][floor1] == nullptr) {
                algorithmErrorString = Errors::buildNotLegalOperationError("Unloading", containerId, floor1, x1, y1, "this container isn't exist at Ship");
                return ERROR;
            }
            if (checkUnloadInstruction(x1, y1, floor1, *shipPlan.getContainers()[x1][y1][floor1], algorithmErrorString) == ERROR)
                return ERROR;
            else{
                containersAwaitingAtPort.emplace_back(*shipPlan.getContainers()[x1][y1][floor1]);
                shipPlan.removeContainer(x1, y1, floor1);
            }
        }
        else if(instructionType == 'M'){
            algActionsCounter += 3;
            if (shipPlan.getContainers()[x1][y1][floor1] == nullptr) {
                algorithmErrorString = Errors::buildNotLegalOperationError("Moving", containerId, floor1, x1, y1, "this container isn't exist at Ship");
                return ERROR;
            }
            if (checkMoveInstruction(x1, y1, floor1, x2, y2, floor2, *shipPlan.getContainers()[x1][y1][floor1], algorithmErrorString) == ERROR)
                return ERROR;
        }
    }
    // look for containers that were unloaded at the current port and shouldn't
    for (auto& _container : containersAwaitingAtPort) {
        if (findPortIndex(shipRoute, currPortSymbol, currPortIndex) != NOT_IN_ROUTE &&
            _container.getDestination() != currPortSymbol && freeSlotsInShip() > 0){
            algorithmErrorString = Errors::buildContainerForgottenError(currPortSymbol);
            return ERROR;
        }
    }

    // look for containers that weren't unloaded at the current port and should
    for (int x = 0; x < shipPlan.getPivotXDimension(); x++){
        for (int y = 0; y < shipPlan.getPivotYDimension(); y++){
            for (int floor = 0; floor < shipPlan.getFloorsNum(); floor++){
                if (shipPlan.getContainers()[x][y][floor] != nullptr &&
                    shipPlan.getContainers()[x][y][floor]->getDestination() == currPortSymbol){
                    algorithmErrorString = Errors::buildContainerWasntDroppedError(currPortSymbol);
                    return ERROR;
                }
            }
        }
    }
    return VALID;
}

int Simulator::checkLoadInstruction(int x, int y, int floor, Container& container, string& algorithmErrorString){
    if(shipPlan.getContainers()[x][y][floor] != nullptr && shipPlan.getContainers()[x][y][floor]->isFutile()){
        algorithmErrorString = Errors::buildNotLegalOperationError("Loading", container.getId(), floor, x, y, "this location is blocked");
        return ERROR;
    } else if (shipPlan.getContainers()[x][y][floor] != nullptr && !shipPlan.getContainers()[x][y][floor]->isFutile()){
        algorithmErrorString = Errors::buildNotLegalOperationError("Loading", container.getId(), floor, x, y, "this location is occupied by another container");
        return ERROR;
    }else if (shipPlan.getContainers()[x][y][floor - 1] == nullptr){
        algorithmErrorString = Errors::buildNotLegalOperationError("Loading", container.getId(), floor, x, y, "there isn't any container under the loaded container");
        return ERROR;
    } else if (calculator.tryOperation('L', container.getWeight(), x, y) != WeightBalanceCalculator::APPROVED){
        algorithmErrorString = Errors::buildNotLegalOperationError("Loading", container.getId(), floor, x, y, "the operation isn't approved by the weight balance calculator");
        return ERROR;
    } else{
        shipPlan.setContainers(x, y, floor, container);
        container.setLocation(x, y, floor);
        return VALID;
    }
}

int Simulator::checkUnloadInstruction(int x, int y, int floor, Container& container, string& algorithmErrorString){
    if (shipPlan.getContainers()[x][y][floor]->isFutile()){
        algorithmErrorString = Errors::buildNotLegalOperationError("Unloading", container.getId(), floor, x, y, "this location is blocked");
        return ERROR;
    } else if (floor != (int)shipPlan.getContainers()[x][y].size() - 1 && shipPlan.getContainers()[x][y][floor + 1] != nullptr){
        algorithmErrorString = Errors::buildNotLegalOperationError("Unloading", container.getId(), floor, x, y, "there are containers above the unloaded container");
        return ERROR;
    } else if (calculator.tryOperation('U', container.getWeight(), x, y) != WeightBalanceCalculator::APPROVED){
        algorithmErrorString = Errors::buildNotLegalOperationError("Unloading", container.getId(), floor, x, y, "the operation isn't approved by the weight balance calculator");
        return ERROR;
    } else return VALID;
}

int Simulator::checkMoveInstruction(int x1, int y1, int floor1, int x2, int y2, int floor2, Container& container, string& algorithmErrorString){
    if (shipPlan.getContainers()[x1][y1][floor1]->isFutile()){
        algorithmErrorString = Errors::buildNotLegalOperationError("Moving", container.getId(), floor1, x1, y1, "this location is blocked");
        return ERROR;
    } else if (floor1 != (int)shipPlan.getContainers()[x1][y1].size() - 1 && shipPlan.getContainers()[x1][y1][floor1 + 1] != nullptr){
        algorithmErrorString = Errors::buildNotLegalOperationError("Moving", container.getId(), floor1, x1, y1, "there are containers above the unloaded container");
        return ERROR;
    } else if (calculator.tryOperation('U', container.getWeight(), x1, y1) != WeightBalanceCalculator::APPROVED){
        algorithmErrorString = Errors::buildNotLegalOperationError("Unload", container.getId(), floor1, x1, y1, "the operation isn't approved by the weight balance calculator");
        return ERROR;
    } else if(shipPlan.getContainers()[x2][y2][floor2] != nullptr && shipPlan.getContainers()[x2][y2][floor2]->isFutile()){
        algorithmErrorString = Errors::buildNotLegalOperationError("Moving", container.getId(), floor2, x2, y2, "this location is blocked");
        return ERROR;
    } else if (shipPlan.getContainers()[x2][y2][floor2] != nullptr && !shipPlan.getContainers()[x2][y2][floor2]->isFutile()){
        algorithmErrorString = Errors::buildNotLegalOperationError("Moving", container.getId(), floor2, x2, y2, "this location is occupied by another container");
        return ERROR;
    }else if (shipPlan.getContainers()[x2][y2][floor2 - 1] == nullptr){
        algorithmErrorString = Errors::buildNotLegalOperationError("Moving", container.getId(), floor2, x2, y2, "there isn't any container under the loaded container");
        return ERROR;
    } else if (calculator.tryOperation('L', container.getWeight(), x2, y2) != WeightBalanceCalculator::APPROVED){
        algorithmErrorString = Errors::buildNotLegalOperationError("Loading", container.getId(), floor2, x2, y2, "the operation isn't approved by the weight balance calculator");
        return ERROR;
    } else{
        int newX, newY, newZ;std::tie(newX, newY, newZ) = shipPlan.getContainers()[x1][y1][floor1]->getLocation();
        shipPlan.setContainers(x2, y2, floor2, *shipPlan.getContainers()[x1][y1][floor1]);
        shipPlan.removeContainer(x1, y1, floor1);
        std::tie(newX, newY, newZ) = shipPlan.getContainers()[x2][y2][floor2]->getLocation();
        return VALID;
    }
}


// -------------------------- errors --------------------------- //

void Simulator::makeTravelError(int travelErrors, const string& output, vector<vector<int>>& outputMat, int algInd, int travelInd){
    std::error_code ec;
    fs::create_directory(output + SEPARATOR + "errors", ec);
    ofstream errorsFile(errorsFileName);
    for (int i = 1; i <= (1 << 18); i *= 2){
        if ((i & travelErrors) > 0)
            errorsFile << Errors::errorsMap[i] << "\n";
    }
    errorsFile << "Travel errors occurred. Skipping travel.";
    errorsFile.close();
    clearData(shipPlan, shipRoute);

    outputMat[algInd][travelInd] = -1;
    outputMat[algInd][outputMat[algInd].size() - 1]++;
}

void createSimulationResults(ofstream& simulationResults, vector<tuple<string,vector<int>,int,int>> outputVector){
    sort(outputVector.begin(), outputVector.end(), compareAlgoTuples);
    if (simulationResults.is_open()){
        for (auto& alg : outputVector){
            simulationResults << get<0>(alg) + ","; // algorithm name
            for (int algActionNum : get<1>(alg)){ // algorithm's num of operations
                simulationResults << to_string(algActionNum) + ",";
            }
            simulationResults << to_string(get<2>(alg)) + "," // algorithm's sum of operations
                              << to_string(get<3>(alg)) + "\n"; // // algorithm's num of errors
        }
    }
    simulationResults.close();
}

void Simulator::writeErrors(int errorsOfAlgorithm, Travel& travel, vector<vector<int>>& outputMat, int algInd, int travelInd, const string& algorithmErrorString){
    // we have errors in the given input / the algorithm made errors
    if (errorsOfAlgorithm != 0) {
        if(!fs::exists(travel.getOutputPath() + SEPARATOR + "errors")){
            std::error_code ec;
            fs::create_directory(travel.getOutputPath() + SEPARATOR + "errors", ec);
        }

        ofstream errorsFile(getErrorsFileName());
        if(errorsFile.is_open()) {
            // writing the errors from the given input
            for (int i = 1; i <= (1 << 18); i *= 2)
                if ((i & errorsOfAlgorithm) > 0)
                    errorsFile << Errors::errorsMap[i] << "\n";

            // the algorithm made errors -> updating simulation.results' Mat
            if ((errorsOfAlgorithm & (1 << 19)) > 0) {
                errorsFile << algorithmErrorString;
                outputMat[algInd][travelInd] = -1;
                outputMat[algInd][outputMat[algInd].size() - 1]++;
            }
            errorsFile.close();
        }
    }
}

bool Simulator::cantRunTravel(int travelErrors, const string& output, vector<vector<int>>& outputMat, int algInd, int travelInd){
    if ((CANNOT_RUN_TRAVEL & travelErrors) != 0) {
        makeTravelError(travelErrors, output, outputMat, algInd, travelInd);
        return true;
    }
    return false;
}

// -------------------------- run algorithm - travel pair --------------------------- //

int Simulator::startTravel (AbstractAlgorithm* algorithm, const string& algName, Travel& travel,
                            string& algorithmErrorString, const string& output, int& algActionsCounter) {

    string travelAlgorithmDir = output + SEPARATOR + algName + "_" + travel.getName() + "_crane_instructions";
    fs::create_directory(travelAlgorithmDir);

    int errors = 0;
    int currPortIndex = -1;
    algActionsCounter = 0;
    for (const Port& port : shipRoute.getPortsList()) {
        currPortIndex++;

        //finding portVisitNum
        int portVisitNum = 0;
        for (int i = 0; i <= currPortIndex; ++i)
            if (shipRoute.getPortsList()[i].getPortId() == port.getPortId())
                portVisitNum++;

        string inputFileName, outputFileName;
        getPortFilesName(inputFileName, outputFileName, port.getPortId(), portVisitNum, travel, travelAlgorithmDir);

        // simulator is reading which containers are waiting on port
        vector<Container> containersAwaitingAtPort;
        bool isFinalPort = currPortIndex == (int)shipRoute.getPortsList().size();
        readContainersAwaitingAtPort(inputFileName, isFinalPort, containersAwaitingAtPort, shipPlan, shipRoute, currPortIndex);

        // algorithm is reading the input and making actions on his ship plan
        //Errors here will be written in the same func of the next step
        errors |= algorithm->getInstructionsForCargo(inputFileName, outputFileName);

        int status = checkAndCountAlgorithmActions(containersAwaitingAtPort, outputFileName, port.getPortId(), algorithmErrorString, currPortIndex, algActionsCounter);

        if (status == VALID)
            continue;
        else{
            errors |= (1 << 19); //It means that there is a bad algorithm behavior, error string is algorithmErrorString
            return errors;
        }
    }
    return errors;
}

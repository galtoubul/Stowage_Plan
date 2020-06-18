#include "AlgorithmsBaseClass.h"
#include <filesystem>
#include <string>
#define PORT_SYMBOL_LENGTH 5
#define SEPARATOR std::filesystem::path::preferred_separator
using std::string;
using std::cout;
using std::vector;
using std::tuple;
using std::endl;
using std::get;

int AlgorithmsBaseClass::setWeightBalanceCalculator(WeightBalanceCalculator& _calculator){
    this->calculator = _calculator;
    return 0;
}

int AlgorithmsBaseClass::getInstructionsForCargo(const std::string& input_full_path_and_file_name, const std::string& output_full_path_and_file_name){
    string portSymbol = input_full_path_and_file_name.substr(input_full_path_and_file_name.find_last_of(SEPARATOR) + 1, PORT_SYMBOL_LENGTH);
    int visitNum = calcVisitNum (input_full_path_and_file_name);
    size_t currPortIndex = findCurrPortIndex(shipRoute, portSymbol, visitNum);
    vector<INSTRUCTION> instructions;

    getUnloadingInstructions(instructions, currPortIndex);

    bool isFinalPort = currPortIndex == shipRoute.getPortsList().size() - 1;
    vector<Container> containersAwaitingAtPort;
    errors |= readContainersAwaitingAtPort(input_full_path_and_file_name, isFinalPort, containersAwaitingAtPort,
                                           shipPlan, shipRoute, currPortIndex);

    errors |= getLoadingInstructions(instructions, containersAwaitingAtPort, currPortIndex);

    writeInstructionsToFile(instructions, output_full_path_and_file_name);
    return errors;
}

int AlgorithmsBaseClass::getLoadingInstructions(vector<INSTRUCTION>& instructions, vector<Container>& containersAwaitingAtPort
        , int currPortIndex){
    for (auto& container : containersAwaitingAtPort){
        string portDest = container.getDestination();
        if (findPortIndex(this->shipRoute, portDest, currPortIndex) == NOT_IN_ROUTE)
            instructions.emplace_back('R', container.getId(), NOT_IN_ROUTE, NOT_IN_ROUTE, NOT_IN_ROUTE, -1, -1, -1);
    }
    vector<Container> sortedContainersAwaitingAtPort;
    orderContainersByDest(containersAwaitingAtPort, sortedContainersAwaitingAtPort, shipRoute, currPortIndex); //More efficient, for algorithm B
    int errors = 0;
    for (auto& container : sortedContainersAwaitingAtPort){
        if (container.isRejected()){
            instructions.emplace_back('R', container.getId(), NOT_IN_ROUTE, NOT_IN_ROUTE, NOT_IN_ROUTE, -1, -1, -1);
            continue;
        }
        errors |= loadToShip(container, instructions, currPortIndex);
    }
}

int AlgorithmsBaseClass::loadToShip(Container& container, vector<INSTRUCTION>& instructions, int currPortIndex){
    string portDest = container.getDestination();
    int portDestIndex = findPortIndex(shipRoute, portDest, currPortIndex);
    if (portDestIndex == NOT_IN_ROUTE) {
        instructions.emplace_back('R', container.getId(), NOT_IN_ROUTE, NOT_IN_ROUTE, NOT_IN_ROUTE, -1, -1, -1);
        return 0;
    }

    // locate containers on ship
    int idealSpotX = -1, idealSpotY = -1, idealSpotF = -1, defaultX = -1, defaultY = -1, defaultF = -1;
    bool isContainerBelowBefore = false; // if the container below is for a port before the current (max port is still before) we will want to take the default if possible
    int maxPortIndex = -1;
    for (int x = 0; x < shipPlan.getPivotXDimension(); x++){
        for (int y = 0; y < shipPlan.getPivotYDimension(); y++){
            for (int floor = 0; floor < shipPlan.getFloorsNum(); floor++){
                if(shipPlan.getContainers()[x][y][floor] == nullptr &&
                   calculator.tryOperation('L', container.getWeight(), x, y) == WeightBalanceCalculator::APPROVED){
                    if (floor != 0 && !shipPlan.getContainers()[x][y][floor-1]->isFutile() && findPortIndex(shipRoute, shipPlan.getContainers()[x][y][floor-1]->getDestination(), currPortIndex) > maxPortIndex){
                        idealSpotX = x;
                        idealSpotY = y;
                        idealSpotF = floor;
                        maxPortIndex = findPortIndex(shipRoute, shipPlan.getContainers()[x][y][floor-1]->getDestination(), currPortIndex);

                        if (maxPortIndex < portDestIndex)
                            isContainerBelowBefore = true;
                    }
                    else if ((floor == 0 || shipPlan.getContainers()[x][y][floor-1]->isFutile()) && defaultX == -1){ //It means that there is not default
                        defaultX = x;
                        defaultY = y;
                        defaultF = floor;
                    }
                    break; //because we got to air
                }
            }
        }
    }

    if (idealSpotX == -1 && defaultX == -1){
        instructions.emplace_back('R', container.getId(), -1, -1, -1, -1, -1, -1); //No spot on ship
        return (1 << 18);
    }
    if ((idealSpotX == -1 && defaultX != -1) || (isContainerBelowBefore && defaultX != -1)){
        idealSpotX = defaultX;
        idealSpotY = defaultY;
        idealSpotF = defaultF;
    }
    instructions.emplace_back('L', container.getId(), idealSpotF, idealSpotX, idealSpotY, -1, -1, -1);

    shipPlan.setContainers(idealSpotX, idealSpotY, idealSpotF, container);

    (const_cast<Port&>(shipRoute.getPortsList()[findPortIndex(shipRoute, portDest, currPortIndex)])).addContainerToUnloadToPort(container);
    return 0;
}

void AlgorithmsBaseClass::getUnloadingInstructions(vector<INSTRUCTION>& instructions, int currPortIndex) {
    for (auto &container : shipRoute.getPortsList()[currPortIndex].getContainersToUnload()) {
        if (shipRoute.getPortsList()[currPortIndex].isStillOnPort(container.getId()))
            unloadToPort(container, instructions, shipRoute.getPortsList()[currPortIndex]);
    }
}

void AlgorithmsBaseClass::unloadToPort(Container& container, vector<INSTRUCTION>& instructions, Port& port){
    int floorOfContainer, x, y;
    std::tie(x, y, floorOfContainer) = findLoc(container.getId());
    int currFloor = (int) shipPlan.getContainers()[x][y].size() - 1; //start from highest floor of x,y
    vector<INSTRUCTION> containersToLoadBack;
    std::unique_ptr<Container> currContainer;
    string currPort = shipPlan.getContainers()[x][y][floorOfContainer]->getDestination();

    // unloading all containers above the desired container
    while (shipPlan.getContainers()[x][y][currFloor] == nullptr)
        currFloor--;
    while(currFloor != floorOfContainer){
        if(calculator.tryOperation('U', shipPlan.getContainers()[x][y][currFloor]->getWeight(), x, y) == WeightBalanceCalculator::APPROVED) {
            if (shipPlan.getContainers()[x][y][currFloor]->getDestination() == currPort) { // unload the container
                instructions.emplace_back('U', shipPlan.getContainers()[x][y][currFloor]->getId(), currFloor, x, y, -1, -1, -1);
                port.removeContainer(shipPlan.getContainers()[x][y][currFloor]->getId());
                shipPlan.removeContainer(x, y, currFloor);
                currFloor--;
            } else { // move the container
                int row, col, floor;
                std::tie(row, col, floor) = findEmptySpot(x, y);
                while (calculator.tryOperation('L', shipPlan.getContainers()[x][y][currFloor]->getWeight(), row, col) != WeightBalanceCalculator::APPROVED)
                    std::tie(row, col, floor) = findEmptySpot(x, y);
                if (row == -1){
                    instructions.emplace_back('U',shipPlan.getContainers()[x][y][currFloor]->getId(), currFloor, x, y, -1, -1, -1);
                    containersToLoadBack.emplace_back('L', shipPlan.getContainers()[x][y][currFloor]->getId(), currFloor - 1, x, y, -1, -1, -1);
                    currFloor--;
                    continue;
                }
                instructions.emplace_back('M', shipPlan.getContainers()[x][y][currFloor]->getId(), currFloor, x, y, floor, row, col);
                shipPlan.setContainers(row, col, floor, *shipPlan.getContainers()[x][y][currFloor]);
                shipPlan.removeContainer(x, y, currFloor);
                currFloor--;
            }
        }
    }
    // unloading the desired container
    instructions.emplace_back('U', shipPlan.getContainers()[x][y][currFloor]->getId(), currFloor, x, y, -1, -1, -1);
    shipPlan.removeContainer(x, y, currFloor);

    int i = (int) containersToLoadBack.size() - 1;
    currFloor++;
    while (currFloor != (int) shipPlan.getContainers()[x][y].size() && i >= 0) {
        shipPlan.setContainers(x, y, currFloor - 1, *shipPlan.getContainers()[x][y][currFloor]);
        instructions.push_back(containersToLoadBack[i]);
        i--;
        currFloor++;
    }
}

int AlgorithmsBaseClass::calcVisitNum (const string& input_full_path_and_file_name){
    string visitNumString;
    visitNumString = input_full_path_and_file_name.substr(input_full_path_and_file_name.size() -
                                                          string("x.cargo_data").size(), 1);
    return stoi(visitNumString);
}

tuple<int,int,int> AlgorithmsBaseClass::findLoc (const string& containerID){
    for (int x = 0; x < shipPlan.getPivotXDimension(); x++)
        for (int y = 0; y < shipPlan.getPivotYDimension(); y++)
            for (int floor = 0; floor < shipPlan.getFloorsNum(); floor++)
                if(shipPlan.getContainers()[x][y][floor] != nullptr && shipPlan.getContainers()[x][y][floor]->getId() == containerID)
                    return {x, y, floor};
    return {-1, -1, -1};
}

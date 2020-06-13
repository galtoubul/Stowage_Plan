#include <iostream>
#include <algorithm>
#include <regex>
#include <filesystem>
#include "Parser.h"
using std::string;
using std::cout;
using std::vector;
using std::tuple;
using std::endl;
using std::list;
using std::ofstream;
using std::get;
namespace fs = std::filesystem;
#define SEPERATOR string(1, fs::path::preferred_separator)
#define NOT_A_COMMENT_LINE 0
#define COMMENT_LINE 1

// ------------------------ utilities -----------------------------------//

void split(vector<string>& elems, const string &s, char delim) {
    auto result = std::back_inserter(elems);
    std::istringstream iss(s);
    string item;
    while (std::getline(iss, item, delim)) {
        if (!item.empty())
            *result++ = item;
    }
}

// trim white spaces from left
inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim white spaces from right
inline std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim white spaces from left & right
inline std::string& trim(std::string& s, const char* t = " \t\n\r\f\v")
{
    return ltrim(rtrim(s, t), t);
}

inline bool fileExists (const std::string& fileName) {
    ifstream f(fileName);
    return f.good();
}

int isCommentOrWS (const string& line){
    const std::regex regex("\\s*[#].*");
    const std::regex regexWS(R"(\s*\t*\r*\n*)");
    if (std::regex_match(line, regex) || std::regex_match(line, regexWS))
        return COMMENT_LINE;
    return NOT_A_COMMENT_LINE;
}

// ------------------------ validations -----------------------------------//

bool isShipPlanLineValid (const string& line){
    const std::regex regex("\\s*[0-9]\\s*[,]\\s*[0-9]\\s*[,]\\s*[0-9]\\s*");
    return std::regex_match(line, regex);
}

int checkIfValidPortId(string port){
    //has to be in model of: XXXXX - size 5
    const std::regex regex("\\s*[a-zA-z]{5}\\s*");
    if (!(std::regex_match(port, regex)))
        return (1 << 13); // error code for "containers at port"
    return 0;
}

int validateWeight (const string& line){
    const std::regex regex("\\s*[0-9]*\\s*");
    if (!(std::regex_match(line, regex)))
        return (1 << 12);

    return 0;
}

int validateContainersAwaitingAtPortLine (vector<string>& line) {
    int validIdError = 0;

    if (line.size() != 3)
        validIdError |= (1 << 14);

    for (int i = 0; i < 3; ++i)
        trim(line[i]);

    if (!ISO6346::isValidId(line[0]))
        validIdError |= (1 << 15);

    return (validIdError | validateWeight(line[1]) | checkIfValidPortId(line[2]));
}

// ------------------------ sub main functions -----------------------------------//

void getPortFilesName(string& inputFileName, string& outputFileName, const string& portId, const int portVisitNum, Travel& travel, const string& dir){
    inputFileName = travel.getDir().string() + SEPERATOR + portId + "_" + to_string(portVisitNum) + ".cargo_data";
    if (!fs::exists(inputFileName))
        ofstream outfile (inputFileName);

    outputFileName = dir + SEPERATOR + portId + "_" + to_string(portVisitNum) + ".crane_instructions";
}

int findPortIndex(ShipRoute& shipRoute, const string& portSymbol, int currPortIndex){
    for (int i = currPortIndex + 1; i < (int)shipRoute.getPortsList().size(); i++) {
        if (shipRoute.getPortsList()[i].getPortId() == portSymbol)
            return i;
    }
    return NOT_IN_ROUTE;
}

int findCurrPortIndex(ShipRoute& shipRoute, const string& portSymbol, int visitNum){
    int counter = 0;
    int currPortIndex = -1;

    for (const Port& port : shipRoute.getPortsList()){
        currPortIndex++;
        if (portSymbol == port.Port::getPortId()){
            counter++;
            if (counter == visitNum)
                break;
        }
    }
    return currPortIndex;
}

void orderContainersByDest(vector<Container>& containersAwaitingAtPort,
                           vector<Container>& sortedContainersAwaitingAtPort, ShipRoute& shipRoute, int currPortIndex){
    for (size_t i = currPortIndex + 1; i < shipRoute.getPortsList().size(); i++){
        for (auto& container : containersAwaitingAtPort){
            string destPort = container.getDestination();
            if (findPortIndex(shipRoute, destPort, (int)currPortIndex) == (int)i)
                sortedContainersAwaitingAtPort.emplace_back(container);
        }
    }
}

void getInstructionsForPort(const string& outputFileName, vector<INSTRUCTION>& instructions) {
    ifstream outputFile(outputFileName);
    string line;
    if (outputFile.is_open()) {
        while (getline(outputFile, line)) {
            vector<string> temp;
            split(temp, line, ',');

            if(temp.size() == 5) // Load / Unload / Reject
                instructions.emplace_back(temp[0].at(0), temp[1], stoi(temp[2]), stoi(temp[3]), stoi(temp[4]), -1, -1, -1);

            if(temp.size() == 8) // Move
                instructions.emplace_back(temp[0].at(0), temp[1], stoi(temp[2]), stoi(temp[3]),
                                          stoi(temp[4]), stoi(temp[5]), stoi(temp[6]), stoi(temp[7]));
        }
        outputFile.close();
    }
}

// ------------------------ main functions -----------------------------------//

int Parser::readShipPlan (ShipPlan& shipPlan, const string& shipPlanFileName){
    int errors = 0;
    ifstream shipPlanInputFile(shipPlanFileName);
    vector<tuple<int, int, int>> vecForShipPlan;
    string line;
    int floorsNum, dimX, dimY;

    if (shipPlanInputFile.is_open()) {
        while (getline(shipPlanInputFile, line)) { //validating first line, if the format is ok we will continue
            if (isCommentOrWS(line))
                continue;
            if(!isShipPlanLineValid(line)){
                errors |= (1 << 3);
                return errors;
            } else{
                vector<string> temp;
                split(temp, line, ',');
                floorsNum = stoi(temp[0]);
                dimX = stoi(temp[1]);
                dimY = stoi(temp[2]);
                break;
            }
        }
        while (getline(shipPlanInputFile, line)) {
            if(isCommentOrWS(line))
                continue;
            if(!isShipPlanLineValid(line)){
                errors |= (1 << 2);
                continue;
            }

            vector<string> temp;
            split(temp, line, ',');

            bool isDuplicateAppearanceWithSameData  = false;
            for(tuple<int, int, int> data : vecForShipPlan){
                if (stoi(temp[0]) == get<0>(data) && stoi(temp[1]) == get<1>(data)){
                    if (stoi(temp[2]) != get<2>(data)){
                        errors |= (1 << 4);
                        return errors;
                    }
                    else {
                        errors |= (1 << 2);
                        isDuplicateAppearanceWithSameData = true;
                        break;
                    }
                }
            }
            if (isDuplicateAppearanceWithSameData)
                continue;

            vecForShipPlan.emplace_back(stoi(temp[0]), stoi(temp[1]), stoi(temp[2]));
        }
        shipPlanInputFile.close();
    } else{
        errors |= (1 << 3);
        return errors;
    }

    shipPlan.init(dimX, dimY, floorsNum);

    for (size_t i = 0; i < vecForShipPlan.size(); ++i) {
        int blockedFloors = floorsNum - get<2>(vecForShipPlan[i]);
        if (blockedFloors <= 0){
            errors |= (1 << 0);
            continue;
        }
        if (get<0>(vecForShipPlan[i]) >= dimX || get<1>(vecForShipPlan[i]) >= dimY){
            errors |= (1 << 1);
            continue;
        }
        for (int j = 0; j < blockedFloors; j++)
            shipPlan.addFutileContainer(get<0>(vecForShipPlan[i]), get<1>(vecForShipPlan[i]), j);
    }
    return errors;
}

int Parser::readShipRoute(ShipRoute& shipRoute, const string& shipPlanFileName){
    int errors = 0;
    ifstream shipRouteInputFile (shipPlanFileName);
    string line;
    int currPortInd = 0;
    if (shipRouteInputFile.is_open()){
        while (getline(shipRouteInputFile,line)){
            if(isCommentOrWS(line))
                continue;

            line = trim(line);
            if (checkIfValidPortId(line) != 0){ //has to be in model of: XXXXX - size 5
                errors |= (1 << 6);
                continue;
            }

            std::transform(line.begin(), line.end(), line.begin(), [](unsigned char c){ return std::toupper(c);});

            //the same port can't appear in two consecutive lines
            if(!shipRoute.getPortsList().empty() && line == shipRoute.getPortsList()[currPortInd - 1].getPortId()){
                errors |= (1 << 5);
                continue;
            }
            shipRoute.addPort(line);
        }
        shipRouteInputFile.close();
    }
    else errors |= (1 << 7);

    if (shipRoute.getPortsList().empty())
        errors |= (1 << 7);

    if (shipRoute.getPortsList().size() == 1)
        errors |= (1 << 8);

    return errors;
}

int readContainersAwaitingAtPort (const string& inputFileName, bool isFinalPort, vector<Container>& containersAwaitingAtPort,
                                  ShipPlan& shipPlan, ShipRoute& shipRoute, int currPortIndex){
    int errors = 0;
    ifstream inputFile (inputFileName);
    string line;
    if (inputFile.is_open()){
        while (getline(inputFile, line)){
            bool rejected = false;

            vector<string> temp;
            split(temp, line, ',');
            if(isCommentOrWS(line))
                continue;

            if (isFinalPort) {
                errors |= (1 << 17);
                return errors;
            }

            //port id to uppercase
            std::transform(temp[2].begin(), temp[2].end(), temp[2].begin(),
                           [](unsigned char c){ return std::toupper(c);});

            //check duplicate ID on port
            for (auto& container : containersAwaitingAtPort){
                if (!(container.isRejected()) && container.getId() == temp[0]){
                    errors |= (1 << 10);
                    rejected = true;
                }
            }
            int validate = validateContainersAwaitingAtPortLine(temp);
            if(validate != 0){
                errors |= validate;
                rejected = true;
            }
            if (findPortIndex(shipRoute, temp[2], currPortIndex) == NOT_IN_ROUTE) {
                errors |= (1 << 13);
                rejected = true;
            }

            // check for "ID already on ship"
            for (int x = 0; x < shipPlan.getPivotXDimension(); x++) {
                for (int y = 0; y < shipPlan.getPivotYDimension(); y++) {
                    for (int floor = 0; floor < shipPlan.getFloorsNum(); floor++) {
                        if (shipPlan.getContainers()[x][y][floor] &&
                            !shipPlan.getContainers()[x][y][floor]->isFutile() &&
                            shipPlan.getContainers()[x][y][floor]->getId() == temp[0]) {
                            errors|= (1 << 11);
                            rejected = true;
                        }
                    }
                }
            }

            // container is legal for loading
            containersAwaitingAtPort.emplace_back(stoi(temp[1]), temp[2], temp[0], false, rejected);
        }
        inputFile.close();
    }
    else
        errors |= (1 << 16); // "assuming no cargo to be loaded at this port" - we will get an empty vector and that's ok

    return errors;
}

void writeInstructionsToFile(vector<tuple<char, string, int, int, int, int, int, int>> &instructions, const string& output_full_path_and_file_name) {
    ofstream instructionsForCargoFile(output_full_path_and_file_name);
    for (INSTRUCTION instruction : instructions){
        instructionsForCargoFile << get<0>(instruction) << ", " << get<1>(instruction) << ", " <<
                get<2>(instruction) << ", " << get<3>(instruction) << ", "<< get<4>(instruction);

        if (get<0>(instruction) == 'M')
            instructionsForCargoFile << ", " << get<5>(instruction) << ", " << get<6>(instruction) << ", "<< get<7>(instruction);

        instructionsForCargoFile << endl;
    }
    instructionsForCargoFile << endl;
    instructionsForCargoFile.close();
}

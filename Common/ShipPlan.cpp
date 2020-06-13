#include "ShipPlan.h"

void ShipPlan::init(int _dimX, int _dimY, int _floorsNum){
    dimX = _dimX;
    dimY = _dimY;
    floorsNum = _floorsNum;
    for(int k = 0; k < dimX; k++){
        vector<vector<std::unique_ptr<Container>>> row;
        for(int i = 0; i < dimY; i++){
            vector<std::unique_ptr<Container>> col;
            for (int j = 0; j < _floorsNum; j++)
                col.push_back(std::unique_ptr<Container>());
            row.push_back(std::move(col));
        }
        containers.push_back(std::move(row));
    }
}

int ShipPlan::getFloorsNum() const{
    return floorsNum;
}

int ShipPlan::getPivotXDimension() const {
    return dimX;
}

int ShipPlan::getPivotYDimension() const{
    return dimY;
}

const VVVC& ShipPlan::getContainers() const{
    return containers;
}

void ShipPlan::setContainers(int x, int y, int floor, Container& container){
    container.setLocation(x, y, floor);
    containers[x][y][floor] = std::make_unique<Container>(container);
}

void ShipPlan::addFutileContainer(int x, int y, int floor){
    containers[x][y][floor] = std::make_unique<Container>(Container());
}

void ShipPlan::removeContainer (int x, int y, int floor){
    containers[x][y][floor].reset();
}

void ShipPlan::printShipPlan () const {
    for (size_t i = 0; i < containers.size(); ++i) {
        for (size_t j = 0; j < containers[0].size(); ++j) {
            for (size_t k = 0; k <containers[0][0].size() ; ++k) {
                if (containers[i][j][k] != nullptr)
                    std::cout << "containers[" << i << "][" << j << "][" << k << "] = " << *containers[i][j][k]<< std::endl;
            }
        }
    }
}
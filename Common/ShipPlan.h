/**
 * ShipPlan Summary:
 *
 * Contains the class for ship plan and its related functions:
 *
 * ShipPlan ctor                -  empty ctor which initializes the ship dimensions to uninitialized (-2)
 * init                         -  initializes a given ship with the given dimensions
 * getFloorsNum                 -  returns ship's number of floors
 * getPivotXDimension           -  returns ship's number of rows
 * getPivotYDimension           -  returns ship's number of columns
 * getContainers                -  returns ship's containers
 * setContainers                -  inserts a given container to a given spot in the ship
 * addFutileContainer           -  inserts a futile container to a given spot in the ship. Futile container = blocked place on ship
 * removeContainer              -  removes a container from a given spot in the ship
 */

#include <iostream>
#include <vector>
#include <memory>
#include "Container.h"
using std::vector;
typedef vector<vector<vector<std::unique_ptr<Container>>>> VVVC;
#define NOT_IN_ROUTE -2

class ShipPlan{
    int dimX;
    int dimY;
    int floorsNum;
    VVVC containers;

public:
    explicit ShipPlan() : dimX(NOT_ON_SHIP), dimY(NOT_ON_SHIP), floorsNum(NOT_ON_SHIP) {}
    void init(int _dimX, int _dimY, int _floorsNum);

    //ShipPlan(const ShipPlan& other) = delete;
    ShipPlan(const ShipPlan& other);
    ShipPlan& operator=(const ShipPlan&);

    int getFloorsNum() const;
    int getPivotXDimension() const;
    int getPivotYDimension() const;
    const VVVC& getContainers() const;

    void setContainers(int x, int y, int floor, Container& container);

    void addFutileContainer(int x, int y, int floor);

    void removeContainer (int x, int y, int floorNum);

    void printShipPlan () const;
};
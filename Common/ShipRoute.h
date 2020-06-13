/**
 * ShipRoute Summary:
 *
 * Contains the class for ship route and its related functions:
 *
 * ShipRoute ctor               -  empty ctor which initializes portsList vector
 * getPortsList                 -  returns portsList
 * addPort                      -  adds the given port to portsList
 */

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "Port.h"
using std::vector;

class ShipRoute{
    vector<Port> portsList;

public:
    ShipRoute () : portsList() {}

    vector<Port>& getPortsList();

    void addPort(const string& portId);

    friend std::ostream& operator<<(std::ostream& out, const ShipRoute& shipRoute);
};

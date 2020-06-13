#include <iostream>
#include <string>
#include "ShipRoute.h"
using std::unordered_map;
using std::vector;

vector<Port>& ShipRoute::getPortsList(){
    return this->portsList;
}

void ShipRoute::addPort(const string& portId){
    this->portsList.emplace_back(portId);
}

std::ostream& operator<<(std::ostream& out, const ShipRoute& shipRoute) {
    out << "route: ";
    for (auto const& p : shipRoute.portsList)
        out << p.getPortId() << ' ';
    return out;
}

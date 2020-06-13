#include "Port.h"
using std::vector;

const string& Port::getPortId() const{
    return this->id;
}

vector<Container>& Port::getContainersToUnload(){
    return containersToUnload;
}

void Port::addContainerToUnloadToPort(Container& container) {
    containersToUnload.push_back(container);
}

void Port::removeContainer(const string& containerID){
    for (int i = 0; i < (int) containersToUnload.size(); ++i)
        if(containersToUnload[i].getId() == containerID)
            containersToUnload.erase(containersToUnload.begin() + i);
}

bool Port::isStillOnPort(const string& containerID){
    for (int i = 0; i < (int) containersToUnload.size(); ++i)
        if(containersToUnload[i].getId() == containerID)
            return true;
    return false;
}

std::ostream&operator<<(std::ostream& out, const Port& port){
    out << "id: " << port.id;
    return out;
}
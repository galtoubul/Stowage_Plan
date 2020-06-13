/**
 * Port Summary:
 *
 * Contains the class of Port and its related functions:
 *
 * Container ctor               -  custom ctor which acts as empty ctor as well
 * getPortId                    -  returns the port's id (symbol)
 * getContainersToUnload        -  returns the container's weight
 * addContainerToUnloadToPort   -  returns the container's weight
 * removeContainer              -  removing a given container from port
 * isStillOnPort                -  returns true iff the given container is still on port
 */

#include <iostream>
#include <string>
#include <vector>
#include "ShipPlan.h"
using std::string;
using std::vector;

class Port {
    string id;
    vector<Container> containersToUnload;

public:
    Port(const string& _portId = "UNINITIALIZED") : id(_portId) {}
    const string& getPortId() const;
    vector<Container>& getContainersToUnload();
    void addContainerToUnloadToPort(Container& container);
    void removeContainer(const string& containerID);
    bool isStillOnPort(const string& containerID);
    friend std::ostream& operator<<(std::ostream& out, const Port& port);
};

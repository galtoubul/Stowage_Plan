#include "Errors.h"

void Errors::populateErrorsMap(){
    errorsMap.insert({(1 << 0),
                      " ship plan: a position has an equal number of floors, or more, than the number of floors provided in the first line"});
    errorsMap.insert({(1 << 1), "ship plan:a given position exceeds the X/Y ship limits"});
    errorsMap.insert({(1 << 2),
                      "ship plan: bad line format after first line or duplicate x,y appearance with same data"});
    errorsMap.insert({(1 << 3),
                      "ship plan: travel error - bad first line or file cannot be read altogether - cannot run this travel"});
    errorsMap.insert({(1 << 4),
                      "ship plan: travel error - duplicate x,y appearance with different data - cannot run this travel"});
    errorsMap.insert({(1 << 5), "travel route: a port appears twice or more consecutively"});
    errorsMap.insert({(1 << 6), "travel route: bad port symbol format"});
    errorsMap.insert({(1 << 7),
                      "travel route: travel error - empty file or file cannot be read altogether - cannot run this travel"});
    errorsMap.insert({(1 << 8),
                      "travel route: travel error - file with only a single valid port - cannot run this travel"});
    errorsMap.insert({(1 << 9), "reserved"});
    errorsMap.insert({(1 << 10), "containers at port: duplicate ID on port"});
    errorsMap.insert({(1 << 11), "containers at port: ID already on ship"});
    errorsMap.insert({(1 << 12), "containers at port: bad line format, missing or bad weight"});
    errorsMap.insert({(1 << 13), "containers at port: bad line format, missing or bad port dest"});
    errorsMap.insert({(1 << 14), "containers at port: bad line format, ID cannot be read"});
    errorsMap.insert({(1 << 15), "containers at port: illegal ID check ISO 6346"});
    errorsMap.insert({(1 << 16), "containers at port: file cannot be read altogether"});
    errorsMap.insert({(1 << 17), "containers at port: last port has waiting containers"});
    errorsMap.insert({(1 << 18), "containers at port: total containers amount exceeds ship capacity"});
}


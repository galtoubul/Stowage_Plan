#include "Container.h"
using std::tuple;
using std::get;

// --------------------------- getters --------------------------- //

int Container::getWeight (){
    return weight;
}

string Container::getDestination () const{
    return destination;
}

string Container::getId () const{
    return id;
}

bool Container::isFutile (){
    return futile;
}

bool Container::isRejected (){
    return rejected;
}

tuple<int,int,int> Container::getLocation(){
    return loc;
}

// --------------------------- setters --------------------------- //

void  Container::setLocation (int x, int y, int floor) {
    loc = std::make_tuple(x, y, floor);
}

std::ostream& operator<<(std::ostream& out, const Container& container){
    if(container.futile)
        out << "futile container";
    else
        out << "id: " << container.id << ", destination: " << container.destination << ", weight: " << container.weight
            << ", location: [" << get<0>(container.loc) << "][" << get<1>(container.loc) << "][" << get<2>(container.loc) << "]";

    return out;
}
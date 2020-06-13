/**
 * Container Summary:
 *
 * Contains the class of Container and its related functions:
 *
 * Container ctors:   -  1) custom ctor which acts as empty ctor as well
 *                       2) copy ctor which gets another container
 * getWeight          -  returns the container's weight
 * getDestination     -  returns the container's weight
 * getId              -  returns the container's weight
 * getLocation        -  returns the container's weight
 * isFutile           -  returns true if the container is a futile container. Otherwise, returns false.
 * setLocation        -  sets the container's location with the given location.
 */

#include <string>
#include <iostream>
#include <tuple>
using std::string;
using std::tuple;
#define NOT_ON_SHIP -1

class Container{
    int weight;
    string destination;
    string id;
    bool futile;
    tuple<int,int,int> loc;
    bool rejected;

public:
    Container(int _weight = 0, const string& _destination = "", const string& _id = "", bool _futile = true, bool _rejected = false,
              int _x = NOT_ON_SHIP, int _y = NOT_ON_SHIP, int _floorNum = NOT_ON_SHIP) :
            weight(_weight), destination(_destination), id(_id), futile(_futile), loc(std::make_tuple(_x, _y, _floorNum)), rejected(_rejected) {}

    explicit Container (const Container* other) :
            weight(other->weight), destination(other->destination), id(other->id), futile(other->futile), loc(other->loc) {}

    int getWeight ();

    string getDestination () const;

    string getId () const;

    bool isFutile ();

    bool isRejected ();

    tuple<int,int,int> getLocation();

    void setLocation(int x, int y, int floor);

    friend std::ostream& operator<<(std::ostream& out, const Container& container);

    bool operator==(const Container& container2) const{
        return id == container2.getId();
    }
};
#include <filesystem>
#include <string>
#include "_308394642_b.h"
#define SEPARATOR std::filesystem::path::preferred_separator
#define PORT_SYMBOL_LENGTH 6
using std::string;
using std::cout;
using std::vector;
using std::tuple;
using std::endl;
using std::list;
using std::get;

REGISTER_ALGORITHM(_308394642_b)

std::tuple<int,int,int> _308394642_b::findEmptySpot(int x, int y){
    for (int i = 0; i < shipPlan.getPivotXDimension(); i++) {
        for (int j = 0; j < shipPlan.getPivotYDimension(); j++) {
            for (int k = 0; k < shipPlan.getFloorsNum(); k++) {
                if (i != x && j != y && shipPlan.getContainers()[i][j][k] == nullptr)
                    return {i, j, k};
            }
        }
    }
    return {-1, -1, -1};
}


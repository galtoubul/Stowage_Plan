/**
 * Class for algorithm _308394642_b
 * Derives most of its functionality from AlgorithmBaseClass
 * Its uniqueness is at findEmptySpot function
 */
 #pragma once

#include <tuple>
#include <string>
#include <vector>
#include "../Interfaces/AlgorithmRegistration.h"
#include "AlgorithmsBaseClass.h"
using std::vector;
using std::tuple;
using std::string;

class _308394642_b : public AlgorithmsBaseClass {
public:
    // Finds an empty spot on the ship
    // Starts searching from the first row, first column, first floor
    // and keep going till last row, last column, last floor
    std::tuple<int,int,int> findEmptySpot(int x, int y) override;
};

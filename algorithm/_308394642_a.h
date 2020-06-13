/**
 * Class for algorithm _308394642_a
 * Derives most of its functionality from AlgorithmBaseClass
 * Its uniqueness is at findEmptySpot function
 */
#pragma once

#include <tuple>
#include <string>
#include <vector>
#include "AlgorithmsBaseClass.h"
#include "../Interfaces/AlgorithmRegistration.h"
using std::vector;
using std::tuple;
using std::string;

class _308394642_a : public AlgorithmsBaseClass {
public:
    // Finds an empty spot on the ship
    // Starts searching from the last row, first column, first floor
    // and keep going till first row, last column, last floor
    tuple<int,int,int> findEmptySpot(int x, int y) override;
};

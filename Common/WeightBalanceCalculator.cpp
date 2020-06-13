#include <string>
#include "../Interfaces/WeightBalanceCalculator.h"
WeightBalanceCalculator::BalanceStatus WeightBalanceCalculator::tryOperation(char loadUnload, int kg, int X, int Y){
    if(loadUnload || kg || X || Y)
        return APPROVED;
    else
        return APPROVED;
}

int WeightBalanceCalculator::readShipPlan(const std::string& full_path_and_file_name){
    if (!full_path_and_file_name.empty())
        return 0;
    return 0;
}

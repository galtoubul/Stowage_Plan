#pragma once

class WeightBalanceCalculator {
public:
    enum BalanceStatus {
        APPROVED , X_IMBALANCED , Y_IMBALANCED , X_Y_IMBALANCED
    };
    int readShipPlan(const std::string& full_path_and_file_name);

    BalanceStatus tryOperation(char loadUnload, int kg, int X, int Y);
};
#pragma once

#include <cmath>
#include <string>
#include <iostream>
#include <vector>

class ISystemModel {
public:
    virtual ~ISystemModel() = default;
    virtual double calculateNext(double inputValue, int stepIndex) = 0;
    virtual std::string fetchName() const = 0;
    virtual void clearState() = 0;
    virtual bool checkStability() const { return true; }
    virtual void displayStabilityReport() const {
        std::cout << "  Stability analysis not applicable for this model." << std::endl;
    }

protected:
    std::vector<double> stateHistory;
    std::vector<double> controlHistory;
    int iterationIndex = 0;
};
#pragma once
#include "model.h"

class Model1_4 : public ISystemModel {
public:
    Model1_4(double a, double b) : gainA(a), gainB(b) { flushState(); }
    double calculateNext(double u, int t) override {
        double y = stateHistory.empty() ? 0.0 : stateHistory.back();
        double uPrev = controlHistory.size() >= 2 ? controlHistory[controlHistory.size()-2] : 0.0;
        double yNext = gainA * y + gainB * (u - uPrev);
        stateHistory.push_back(yNext);
        controlHistory.push_back(u);
        return yNext;
    }
    std::string fetchName() const override { return "Model 1.4"; }
    void clearState() override { flushState(); }
    bool checkStability() const override { return std::abs(gainA) < 1.0; }
    void displayStabilityReport() const override {
        std::cout << "Model 1.4: z=" << gainA << " |z|=" << std::abs(gainA);
        std::cout << (checkStability() ? " STABLE" : " UNSTABLE") << std::endl;
    }
private:
    void flushState() { stateHistory={0.0}; controlHistory={0.0}; iterationIndex=0; }
    double gainA, gainB;
};
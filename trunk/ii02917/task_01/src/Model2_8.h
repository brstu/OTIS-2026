#pragma once
#include "model.h"

class Model2_8 : public ISystemModel {
public:
    Model2_8(double a, double b, double c) : alpha(a), beta(b), gamma(c) { flushState(); }
    double calculateNext(double u, int t) override {
        double y = stateHistory.empty() ? 0.1 : stateHistory.back();
        double yPrev = stateHistory.size() >= 2 ? stateHistory[stateHistory.size()-2] : 0.1;
        double yNext = alpha*y*(1.0-y) + beta*u + gamma*std::sin(yPrev*u);
        stateHistory.push_back(yNext);
        controlHistory.push_back(u);
        return yNext;
    }
    std::string fetchName() const override { return "Model 2.8"; }
    void clearState() override { flushState(); }
    bool checkStability() const override {
        if (stateHistory.size() < 10) return true;
        double mx = 0;
        for (size_t i = stateHistory.size()-10; i < stateHistory.size(); ++i)
            if (std::abs(stateHistory[i]) > mx) mx = std::abs(stateHistory[i]);
        return mx < 100.0;
    }
    void displayStabilityReport() const override {
        std::cout << "Model 2.8: non-linear, numerical check" << std::endl;
    }
private:
    void flushState() { stateHistory={0.1,0.1}; controlHistory={0.0}; iterationIndex=0; }
    double alpha, beta, gamma;
};
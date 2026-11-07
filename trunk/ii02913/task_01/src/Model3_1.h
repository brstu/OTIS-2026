#pragma once
#include "Model.h"

class Model3_1 : public Model {
private:
    double decayRateA;
    double stepDt;
    double originY;
    double stateY;

public:
    Model3_1(double aVal, double dtVal, double yInit = 10.0)
        : Model("Model 3.1"),
          decayRateA(aVal),
          stepDt(dtVal),
          originY(yInit),
          stateY(yInit) {}

    double computeStep(double /*inputSignal*/) override {
        double derivative = -decayRateA * stateY;
        stateY += stepDt * derivative;
        return stateY;
    }

    void resetState() override {
        stateY = originY;
    }
};
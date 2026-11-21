#pragma once
#include "Model.h"

class Model1_3 : public Model {
private:
    double coeffA1;
    double coeffA2;
    double coeffB;
    double initialY;
    double stateCurr;
    double statePrev;

public:
    Model1_3(double a1Val, double a2Val, double bVal, double yStart = 0.0)
        : Model("Model 1.3"),
          coeffA1(a1Val),
          coeffA2(a2Val),
          coeffB(bVal),
          initialY(yStart),
          stateCurr(yStart),
          statePrev(yStart) {}

    double computeStep(double inputSignal) override {
        double stateNext = coeffA1 * stateCurr + coeffA2 * statePrev + coeffB * inputSignal;
        statePrev = stateCurr;
        stateCurr = stateNext;
        return stateNext;
    }

    void resetState() override {
        stateCurr = initialY;
        statePrev = initialY;
    }
};
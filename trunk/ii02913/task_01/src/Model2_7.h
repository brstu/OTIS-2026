#pragma once
#include "Model.h"

class Model2_7 : public Model {
private:
    double paramA;
    double paramB;
    double thresholdEps;
    double startY;
    double currentY;

    double evaluateRelay(double uVal, double yVal) const {
        if (uVal > thresholdEps || (uVal >= -thresholdEps && yVal > 0.0)) {
            return 1.0;
        }
        return -1.0;
    }

public:
    Model2_7(double aVal, double bVal, double epsVal, double yInit = 0.0)
        : Model("Model 2.7"),
          paramA(aVal),
          paramB(bVal),
          thresholdEps(epsVal),
          startY(yInit),
          currentY(yInit) {}

    double computeStep(double inputSignal) override {
        double nextY = paramA * currentY + paramB * evaluateRelay(inputSignal, currentY);
        currentY = nextY;
        return nextY;
    }

    void resetState() override {
        currentY = startY;
    }
};
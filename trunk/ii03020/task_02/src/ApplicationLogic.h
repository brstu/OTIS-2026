#ifndef APPLICATION_LOGIC_H
#define APPLICATION_LOGIC_H
#include <vector>
#include "Simulation.h"

class IInputSignal {
public:
    virtual ~IInputSignal() = default;
    virtual double getValue(int step) = 0;
};

// Контроллер симуляции
class ApplicationLogic {
public:
    static std::vector<double> simulate(Model& model, IInputSignal& signalSource, int totalSteps) {
        std::vector<double> history;
        history.reserve(totalSteps);
        for (int step = 0; step < totalSteps; ++step) {
            double u = signalSource.getValue(step);
            history.push_back(model.step(u));
        }
        return history;
    }
};

#endif
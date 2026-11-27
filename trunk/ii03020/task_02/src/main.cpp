#include <iostream>
#include "Simulation.h"
#include "StabilityAnalyzer.h"
#include "ApplicationLogic.h"
#include <vector>

class StepSignal : public IInputSignal {
public:
    double getValue(int /*step*/) override { return 1.0; }
};

int main() {
    std::cout << "--- OTIS Task 02: Simulation and Stability ---\n";

    double a = 0.5;
    double b = 1.0;
    StabilityAnalyzer::isStableFirstOrder(a, true);

    Model1_1 linModel(a, b, 0.0);
    StepSignal input;

    auto trajectory = ApplicationLogic::simulate(linModel, input, 5);
    for (size_t i = 0; i < trajectory.size(); ++i) {
        std::cout << "Step " << i << ": y = " << trajectory[i] << "\n";
    }

    return 0;
}
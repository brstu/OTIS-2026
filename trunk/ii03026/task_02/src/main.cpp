#include <iomanip>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include "Models.h"

void simulateModel(const std::string& title, Model& model, double u, int steps) {
    std::cout << "=== " << title << " ===\n";
    model.printStabilityInfo();
    std::cout << "Stability: " << (model.isStable() ? "YES" : "NO") << "\n";

    double y = 0.0;
    std::cout << std::fixed << std::setprecision(4);
    for (int k = 1; k <= steps; ++k) {
        y = model.calculateNextStep(u, y);
        std::cout << "Step " << std::setw(2) << k << ": y = " << y << "\n";
    }
    std::cout << "\n";
}

int main() {
    std::vector<std::pair<std::string, std::unique_ptr<Model>>> models;
    models.emplace_back("First-order linear model", std::make_unique<LinearModel1_1>(0.8, 0.2));
    models.emplace_back("Nonlinear model (deadzone)", std::make_unique<NonLinearModel2_3>(0.7, 0.3, 0.2));
    models.emplace_back("Second-order differential model", std::make_unique<DifferentialModel>(0.5, 0.2, 0.3));

    const double inputSignal = 1.0;
    const int simulationSteps = 10;

    for (auto& entry : models) {
        simulateModel(entry.first, *entry.second, inputSignal, simulationSteps);
    }

    return 0;
}
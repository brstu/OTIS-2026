#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

#include "Model1_3.h"
#include "Model2_7.h"
#include "Model3_1.h"

namespace {

std::unique_ptr<Model> createSelectedModel(int modelChoice) {
    if (modelChoice == 1) {
        double a1 = 0.0;
        double a2 = 0.0;
        double b = 0.0;
        double y0 = 0.0;
        std::cout << "Enter coefficients for Model 1.3 (a1 a2 b y0): ";
        std::cin >> a1 >> a2 >> b >> y0;
        return std::make_unique<Model1_3>(a1, a2, b, y0);
    }
    if (modelChoice == 2) {
        double a = 0.0;
        double b = 0.0;
        double eps = 0.0;
        double y0 = 0.0;
        std::cout << "Enter parameters for Model 2.7 (a b eps y0): ";
        std::cin >> a >> b >> eps >> y0;
        return std::make_unique<Model2_7>(a, b, eps, y0);
    }
    double a = 0.0;
    double dt = 0.0;
    double y0 = 0.0;
    std::cout << "Enter parameters for Model 3.1 (a dt y0): ";
    std::cin >> a >> dt >> y0;
    return std::make_unique<Model3_1>(a, dt, y0);
}

double evaluateSignal(int signalType, int stepIdx) {
    if (signalType == 1) {
        return 1.0; // Ступенчатый сигнал (u = const)
    }
    if (signalType == 2) {
        return (stepIdx == 0) ? 1.0 : 0.0; // Импульсный сигнал
    }
    return std::sin(static_cast<double>(stepIdx)); // Гармонический сигнал u = sin(tau)
}

std::string getSignalLabel(int signalType) {
    if (signalType == 1) {
        return "Step";
    }
    if (signalType == 2) {
        return "Impulse";
    }
    return "Harmonic";
}

} // namespace

int main() {
    std::cout << "=== Select Mathematical Model (Variant 13) ===\n";
    std::cout << "1 - Linear Model 1.3\n";
    std::cout << "2 - Non-linear Model 2.7 (Relay with Hysteresis)\n";
    std::cout << "3 - Differential Equation 3.1 (Linear Decay)\n";
    std::cout << "Your choice (1-3): ";

    int modelChoice = 1;
    std::cin >> modelChoice;
    if (modelChoice < 1 || modelChoice > 3) {
        std::cerr << "Invalid model selection.\n";
        return 1;
    }

    std::unique_ptr<Model> activeModel = createSelectedModel(modelChoice);

    std::cout << "\n=== Select Control Signal Type ===\n";
    std::cout << "1 - Step (u = 1)\n";
    std::cout << "2 - Impulse (u0 = 1, u > 0 = 0)\n";
    std::cout << "3 - Harmonic (u = sin(tau))\n";
    std::cout << "Your choice (1-3): ";

    int signalChoice = 1;
    std::cin >> signalChoice;
    if (signalChoice < 1 || signalChoice > 3) {
        std::cerr << "Invalid signal selection.\n";
        return 1;
    }

    int totalSteps = 30;
    std::cout << "\nEnter number of simulation steps (n): ";
    std::cin >> totalSteps;
    if (totalSteps <= 0) {
        std::cerr << "Number of steps must be positive.\n";
        return 1;
    }

    std::ofstream csvFile("simulation_data.csv");
    if (!csvFile.is_open()) {
        std::cerr << "Error opening simulation_data.csv for writing.\n";
        return 1;
    }

    std::string sigLabel = getSignalLabel(signalChoice);
    csvFile << "Step,InputU,OutputY,ModelTitle,SignalType\n";

    std::cout << "\n--- Simulation Results (" << activeModel->getTitle()
              << " | Signal: " << sigLabel << ") ---\n";
    std::cout << std::setw(8) << "Tau"
              << std::setw(14) << "u(Tau)"
              << std::setw(14) << "y(Tau)" << "\n";

    activeModel->resetState();
    for (int tau = 0; tau < totalSteps; ++tau) {
        double uVal = evaluateSignal(signalChoice, tau);
        double yVal = activeModel->computeStep(uVal);

        std::cout << std::setw(8) << tau
                  << std::setw(14) << std::fixed << std::setprecision(4) << uVal
                  << std::setw(14) << yVal << "\n";

        csvFile << tau << ","
                << uVal << ","
                << yVal << ","
                << activeModel->getTitle() << ","
                << sigLabel << "\n";
    }

    csvFile.close();
    std::cout << "\nResults saved to simulation_data.csv\n";
    return 0;
}
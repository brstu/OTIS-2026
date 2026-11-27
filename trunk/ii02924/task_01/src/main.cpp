#include <iostream>
#include <fstream>
#include <memory>
#include <cmath>
#include <iomanip>
#include <cstdlib>
#include <limits>

#include "Model.h"
#include "Model1_8.h"
#include "Model2_3.h"
#include "Model3_3.h"

void waitForExit() {
    std::cout << "Press Enter to exit...";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

double getSignal(int signalType, int step, double stepAmplitude) {
    if (signalType == 1) {
        return stepAmplitude;
    }
    if (signalType == 2) {
        return (step == 0) ? 1.0 : 0.0;
    }
    return std::sin(static_cast<double>(step));
}

int main() {
    std::cout << "Select model:\n"
              << "1 - Model 1.8 (Linear)\n"
              << "2 - Model 2.3 (Non-linear Dead-Zone)\n"
              << "3 - Model 3.3 (Differential Equation)\n"
              << "Choice (1-3): ";
    int modelChoice = 0;
    if (!(std::cin >> modelChoice) || modelChoice < 1 || modelChoice > 3) {
        std::cerr << "Error: Invalid model choice. Please enter 1, 2, or 3.\n";
        waitForExit();
        return 1;
    }

    double y0 = 0.0;
    std::cout << "\nEnter initial value y0: ";
    std::cin >> y0;

    std::unique_ptr<Model> model;

    if (modelChoice == 1) {
        double a1 = 0.0;
        double a2 = 0.0;
        double b1 = 0.0;
        double b2 = 0.0;
        std::cout << "Enter a1, a2, b1, b2 (separated by space): ";
        std::cin >> a1 >> a2 >> b1 >> b2;
        model = std::make_unique<Model1_8>(a1, a2, b1, b2, y0);
    } else if (modelChoice == 2) {
        double a = 0.0;
        double b = 0.0;
        double delta = 0.0;
        std::cout << "Enter a, b, delta (separated by space): ";
        std::cin >> a >> b >> delta;
        model = std::make_unique<Model2_3>(a, b, delta, y0);
    } else {
        double a = 0.0;
        double b = 0.0;
        double dt = 0.1;
        std::cout << "Enter a, b, dt (separated by space): ";
        std::cin >> a >> b >> dt;
        model = std::make_unique<Model3_3>(a, b, dt, y0);
    }

    std::cout << "\nSelect input signal u(t):\n"
              << "1 - Step (u = const)\n"
              << "2 - Impulse (u(0) = 1, u(t>0) = 0)\n"
              << "3 - Harmonic (u(t) = sin(t))\n"
              << "Choice (1-3): ";
    int signalChoice = 0;
    if (!(std::cin >> signalChoice) || signalChoice < 1 || signalChoice > 3) {
        std::cerr << "Error: Invalid signal choice. Please enter 1, 2, or 3.\n";
        waitForExit();
        return 1;
    }

    double stepAmp = 1.0;
    if (signalChoice == 1) {
        std::cout << "Enter step amplitude u_const: ";
        std::cin >> stepAmp;
    }

    int steps = 20;
    std::cout << "\nEnter number of simulation steps n: ";
    if (!(std::cin >> steps) || steps <= 0) {
        std::cerr << "Error: Number of steps must be greater than 0.\n";
        waitForExit();
        return 1;
    }

    std::ofstream csv("results.csv");
    csv << "step,u,y\n";

    std::cout << "\n--- Simulation: " << model->getName() << " ---\n\n";
    std::cout << std::setw(8) << "Step" << std::setw(15) << "u(t)" << std::setw(15) << "y(t+1)\n\n";

    for (int t = 0; t < steps; ++t) {
        double u = getSignal(signalChoice, t, stepAmp);
        double y = model->step(u);

        std::cout << std::setw(8) << t
                  << std::setw(15) << std::fixed << std::setprecision(4) << u
                  << std::setw(15) << std::fixed << std::setprecision(4) << y << "\n";

        csv << t << "," << u << "," << y << "\n";
    }
    csv.close();

    std::system("python plot.py");
    return 0;
}
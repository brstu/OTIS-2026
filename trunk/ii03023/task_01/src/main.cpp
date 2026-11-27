#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <string>
#include "models.h"

// Прогоняет одну модель с одним входом n шагов, печатает таблицу и пишет в CSV
void simulate(Model& model, const Input& input, int n, std::ofstream& csv) {
    model.reset();
    std::cout << "\n=== " << model.name() << " | input: " << input.name() << " ===\n";
    std::cout << std::setw(6) << "tau" << std::setw(12) << "u" << std::setw(14) << "y" << "\n";
    std::cout << std::string(32, '-') << "\n";

    for (int tau = 0; tau < n; ++tau) {
        double u = input.value(tau);
        double y = model.next(u);

        std::cout << std::setw(6) << tau
                  << std::setw(12) << std::fixed << std::setprecision(4) << u
                  << std::setw(14) << y << "\n";
        csv << model.name() << "," << input.name() << "," << tau << "," << u << "," << y << "\n";
    }
}

// Читает дробное число. Пустой ввод (просто Enter) или ошибка -> значение по умолчанию
double readDouble(const std::string& prompt, double fallback) {
    std::cout << "  " << prompt << " [" << fallback << "]: ";
    std::string line;
    if (!std::getline(std::cin, line) || line.empty()) {
        return fallback;
    }
    std::istringstream stream(line);
    double value = 0.0;
    if (!(stream >> value)) {
        std::cout << "  Invalid number, using " << fallback << "\n";
        return fallback;
    }
    return value;
}

// Читает целое число из диапазона [low, high]. Пустой ввод или ошибка -> значение по умолчанию
int readInt(const std::string& prompt, int low, int high, int fallback) {
    std::cout << prompt << " [" << fallback << "]: ";
    std::string line;
    if (!std::getline(std::cin, line) || line.empty()) {
        return fallback;
    }
    std::istringstream stream(line);
    int value = 0;
    if (!(stream >> value) || value < low || value > high) {
        std::cout << "Invalid input, using " << fallback << "\n";
        return fallback;
    }
    return value;
}

int main() {
    std::cout << "Select model:\n"
              << "  1 - Model 1.10 (linear)\n"
              << "  2 - Model 2.4 (nonlinear)\n"
              << "  3 - Model 3.8 (differential, Euler)\n"
              << "  4 - all models\n";
    int modelChoice = readInt("Your choice", 1, 4, 4);

    std::cout << "\nSelect input signal:\n"
              << "  1 - step      u = c\n"
              << "  2 - impulse   u(0) = 1, then 0\n"
              << "  3 - harmonic  u = sin(tau)\n"
              << "  4 - all inputs\n";
    int inputChoice = readInt("Your choice", 1, 4, 4);

    int n = readInt("\nNumber of simulation steps n", 1, 100000, 20);

    std::cout << "\nEnter values (press Enter to keep the default in brackets):\n";
    double y0 = readDouble("initial value y0", 0.0);

    double stepLevel = 1.0;
    if (inputChoice == 1 || inputChoice == 4) {
        stepLevel = readDouble("step level c", 1.0);
    }

    bool useLinear = modelChoice == 1 || modelChoice == 4;
    bool useNonlinear = modelChoice == 2 || modelChoice == 4;
    bool useDiff = modelChoice == 3 || modelChoice == 4;

    double la1 = 0.7;
    double lb1 = 0.5;
    double lb2 = 0.3;
    if (useLinear) {
        std::cout << " Model 1.10: y(t+1) = a1*y(t) + b1*u(t) + b2*u(t-2)\n";
        la1 = readDouble("a1", la1);
        lb1 = readDouble("b1", lb1);
        lb2 = readDouble("b2", lb2);
    }

    double na = 0.8;
    double nb = 0.5;
    if (useNonlinear) {
        std::cout << " Model 2.4: y(t+1) = a*y(t)*cos(y(t)) + b*ln(1 + |u(t)|)\n";
        na = readDouble("a", na);
        nb = readDouble("b", nb);
    }

    double da1 = 0.5;
    double da2 = 0.1;
    double db = 1.0;
    double dt = 0.1;
    if (useDiff) {
        std::cout << " Model 3.8: dy/dt = -a1*y - a2*y^2 + b*u  (Euler, step dt)\n";
        da1 = readDouble("a1", da1);
        da2 = readDouble("a2", da2);
        db = readDouble("b", db);
        dt = readDouble("dt", dt);
    }

    LinearModel linear(la1, lb1, lb2, y0);
    NonlinearModel nonlinear(na, nb, y0);
    DiffModel diff(da1, da2, db, dt, y0);

    //  проверка устойчивости линейной модели перед симуляцией
    if (useLinear) {
        if (!linear.isStable()) {
            std::cout << "\nWARNING: root of characteristic equation z = " << linear.root()
                      << ", |z| >= 1 -> system is UNSTABLE (process will diverge)!\n";
        } else {
            std::cout << "\nRoot z = " << linear.root() << ", |z| < 1 -> system is stable.\n";
        }
    }

    std::vector<Model*> models;
    if (useLinear) models.push_back(&linear);
    if (useNonlinear) models.push_back(&nonlinear);
    if (useDiff) models.push_back(&diff);

    std::vector<std::unique_ptr<Input>> inputs;
    if (inputChoice == 1 || inputChoice == 4) inputs.push_back(std::make_unique<StepInput>(stepLevel));
    if (inputChoice == 2 || inputChoice == 4) inputs.push_back(std::make_unique<ImpulseInput>());
    if (inputChoice == 3 || inputChoice == 4) inputs.push_back(std::make_unique<HarmonicInput>());

    std::ofstream csv("results.csv");
    csv << "model,input,tau,u,y\n";
    for (Model* m : models)
        for (const auto& in : inputs)
            simulate(*m, *in, n, csv);

    std::cout << "\nResults saved to results.csv\n";
    return 0;
}

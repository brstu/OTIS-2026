#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <memory>
#include "InputSignal.h"
#include "Model.h"
#include "Model1_9.h"
#include "Model2_1.h"
#include "Model3_3.h"

// Безопасное ограничение количества шагов симуляции
int getSafeSteps(int raw_steps) {
    constexpr int min_steps = 1;
    constexpr int max_steps = 1000;
    constexpr int default_steps = 30;

    if (raw_steps < min_steps || raw_steps > max_steps) {
        return default_steps;
    }
    return raw_steps;
}

// Вспомогательная функция для ввода коэффициентов со значениями по умолчанию
double getDoubleInput(const std::string& prompt, double default_value) {
    std::cout << prompt << " (default " << default_value << "): ";
    if (std::string input; std::getline(std::cin, input) && !input.empty()) {
        std::stringstream ss(input);
        double val = 0.0;
        if (ss >> val) {
            return val;
        }
    }
    return default_value;
}

// Выбор модели
int getModelChoice() {
    std::cout << "Select model:\n"
              << "  1. Model 1.9 (Three-Step Moving Average: y[t+1] = a1*y[t] + a2*y[t-1] + a3*y[t-2] + b1*u[t])\n"
              << "  2. Model 2.1 (Quadratic Feedback: y[t+1] = a*y[t] - b*y[t-1]^2 + c*u[t] + d*sin(u[t-1]))\n"
              << "  3. Model 3.3 (Standard First-Order Process: dy/dt = -a*y + b*u)\n"
              << "Enter choice (1-3, default 1): ";
    if (std::string input; std::getline(std::cin, input) && !input.empty()) {
        std::stringstream ss(input);
        int val = 1;
        if (ss >> val && (val >= 1 && val <= 3)) {
            return val;
        }
    }
    return 1;
}

// Выбор входного сигнала
SignalType getSignalChoice() {
    std::cout << "\nSelect input signal:\n"
              << "  1. Step (u = A)\n"
              << "  2. Impulse (u(0) = A, u(t>0) = 0)\n"
              << "  3. Harmonic (u = A * sin(t))\n"
              << "Enter choice (1-3, default 1): ";
    if (std::string input; std::getline(std::cin, input) && !input.empty()) {
        std::stringstream ss(input);
        int val = 1;
        if (ss >> val) {
            switch (val) {
                case 2:
                    return SignalType::IMPULSE;
                case 3:
                    return SignalType::HARMONIC;
                case 1:
                default:
                    return SignalType::STEP;
            }
        }
    }
    return SignalType::STEP;
}

int main(int argc, char* argv[]) {
    int parsed_steps = 30;

    if (argc > 1) {
        try {
            parsed_steps = getSafeSteps(std::stoi(argv[1]));
        } catch (...) {
            parsed_steps = 30;
        }
    } else {
        std::cout << "Enter simulation steps (default 30): ";
        if (std::string input; std::getline(std::cin, input) && !input.empty()) {
            std::stringstream ss(input);
            int val = 0;
            if (ss >> val) {
                parsed_steps = getSafeSteps(val);
            }
        }
    }

    const int steps = parsed_steps;
    const int model_choice = getModelChoice();
    const SignalType selected_signal = getSignalChoice();

    std::unique_ptr<Model> active_model;
    std::string model_name;

    std::cout << "\n--- Parameters configuration ---\n";

    switch (model_choice) {
        case 2: {
            const double a = getDoubleInput("Enter parameter a", 0.5);
            const double b = getDoubleInput("Enter parameter b", 0.1);
            const double c = getDoubleInput("Enter parameter c", 0.4);
            const double d = getDoubleInput("Enter parameter d", 0.2);
            const double y0 = getDoubleInput("Enter initial y0", 0.0);
            active_model = std::make_unique<Model2_1>(a, b, c, d, y0);
            model_name = "Model 2.1";
            break;
        }
        case 3: {
            const double a = getDoubleInput("Enter parameter a", 0.5);
            const double b = getDoubleInput("Enter parameter b", 0.8);
            const double dt = getDoubleInput("Enter time step dt", 0.1);
            const double y0 = getDoubleInput("Enter initial y0", 0.0);
            active_model = std::make_unique<Model3_3>(a, b, dt, y0);
            model_name = "Model 3.3";
            break;
        }
        case 1:
        default: {
            const double a1 = getDoubleInput("Enter parameter a1", 0.4);
            const double a2 = getDoubleInput("Enter parameter a2", 0.3);
            const double a3 = getDoubleInput("Enter parameter a3", 0.1);
            const double b1 = getDoubleInput("Enter parameter b1", 0.5);
            const double y0 = getDoubleInput("Enter initial y0", 0.0);
            active_model = std::make_unique<Model1_9>(a1, a2, a3, b1, y0);
            model_name = "Model 1.9";
            break;
        }
    }

    const std::string sig_name = InputSignal::getSignalName(selected_signal);

    std::ofstream out("results.csv");
    if (!out.is_open()) {
        std::cerr << "Failed to open results.csv\n";
        return 1;
    }

    out << "step,signal,u,y,model_name\n";

    std::cout << "\n============================================\n";
    std::cout << "Simulation started for selected configuration\n";
    std::cout << "============================================\n";

    std::cout << std::setw(6) << "Step"
              << std::setw(12) << "Input u"
              << std::setw(16) << "Output y" << "\n";

    for (int t = 0; t < steps; ++t) {
        const double u = InputSignal::getSignal(selected_signal, t);
        const double y = active_model->step(u);

        std::cout << std::setw(6) << t
                  << std::fixed << std::setprecision(4)
                  << std::setw(12) << u
                  << std::setw(16) << y << "\n";

        out << t << "," << sig_name << "," << u << ","
            << y << "," << model_name << "\n";
    }

    out.close();
    std::cout << "\nResults saved to results.csv\n";
    return 0;
}
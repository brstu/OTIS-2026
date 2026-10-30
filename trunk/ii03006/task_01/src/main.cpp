#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <memory>
#include "InputSignal.h"
#include "Model.h"
#include "Model1_8.h"
#include "Model2_3.h"
#include "Model3_3.h"

int getSafeSteps(int raw_steps) {
    constexpr int min_steps = 1;
    constexpr int max_steps = 1000;
    constexpr int default_steps = 30;
    return (raw_steps < min_steps || raw_steps > max_steps) ? default_steps : raw_steps;
}

template <typename T>
T promptValue(const std::string& prompt, T default_value, T min_v = {}, T max_v = {}, bool check_range = false) {
    std::cout << prompt << " (default " << default_value << "): ";
    if (std::string input; std::getline(std::cin, input) && !input.empty()) {
        std::stringstream ss(input);
        T val;
        if ((ss >> val) && (!check_range || (val >= min_v && val <= max_v))) {
            return val;
        }
    }
    return default_value;
}

int getModelChoice() {
    std::cout << "Select model:\n"
              << "  1. Model 1.8 (Generalized Autoregressive Linear Model)\n"
              << "  2. Model 2.3 (Dead-Zone Non-linearity)\n"
              << "  3. Model 3.3 (Standard First-Order Process)\n";
    return promptValue<int>("Enter choice (1-3)", 1, 1, 3, true);
}

SignalType getSignalChoice() {
    std::cout << "\nSelect input signal:\n"
              << "  1. Step\n"
              << "  2. Impulse\n"
              << "  3. Harmonic\n";
    const int val = promptValue<int>("Enter choice (1-3)", 1, 1, 3, true);
    if (val == 2) return SignalType::IMPULSE;
    if (val == 3) return SignalType::HARMONIC;
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
        parsed_steps = getSafeSteps(promptValue<int>("Enter simulation steps", 30));
    }

    const int steps = parsed_steps;
    const int model_choice = getModelChoice();
    const SignalType selected_signal = getSignalChoice();

    std::unique_ptr<Model> active_model;
    std::string model_name;

    std::cout << "\n--- Parameters configuration ---\n";

    switch (model_choice) {
        case 2: {
            const double a = promptValue("Enter parameter a", 0.6);
            const double b = promptValue("Enter parameter b", 0.5);
            const double delta = promptValue("Enter dead-zone delta", 0.2);
            const double y0 = promptValue("Enter initial y0", 0.0);
            active_model = std::make_unique<Model2_3>(a, b, delta, y0);
            model_name = "Model 2.3";
            break;
        }
        case 3: {
            const double a = promptValue("Enter parameter a", 0.5);
            const double b = promptValue("Enter parameter b", 0.8);
            const double dt = promptValue("Enter time step dt", 0.1);
            const double y0 = promptValue("Enter initial y0", 0.0);
            active_model = std::make_unique<Model3_3>(a, b, dt, y0);
            model_name = "Model 3.3";
            break;
        }
        case 1:
        default: {
            const double a1 = promptValue("Enter parameter a1", 0.5);
            const double a2 = promptValue("Enter parameter a2", 0.2);
            const double b1 = promptValue("Enter parameter b1", 0.4);
            const double b2 = promptValue("Enter parameter b2", 0.1);
            const double y0 = promptValue("Enter initial y0", 0.0);
            active_model = std::make_unique<Model1_8>(a1, a2, b1, b2, y0);
            model_name = "Model 1.8";
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
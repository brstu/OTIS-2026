#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <memory>
#include <algorithm>
#include "InputSignal.h"
#include "Model.h"
#include "Model1_8.h"
#include "Model2_3.h"
#include "Model3_3.h"

struct ExperimentConfig {
    int total_ticks{30};
    int model_id{1};
    DiscreteSignalPattern signal_type{DiscreteSignalPattern::HeavisideStep};
};

static double readField(const char* label, double fallback) {
    std::cout << label << " [" << fallback << "]: ";
    std::string line;
    if (std::getline(std::cin, line) && !line.empty()) {
        try {
            return std::stod(line);
        } catch (...) {
            return fallback;
        }
    }
    return fallback;
}

static ExperimentConfig configureExperiment(int argc, char* argv[]) {
    ExperimentConfig cfg;

    if (argc > 1) {
        try {
            cfg.total_ticks = std::clamp(std::stoi(argv[1]), 1, 1000);
        } catch (...) {
            cfg.total_ticks = 30;
        }
    } else {
        std::cout << "Enter simulation steps [30]: ";
        if (std::string line; std::getline(std::cin, line) && !line.empty()) {
            try {
                cfg.total_ticks = std::clamp(std::stoi(line), 1, 1000);
            } catch (...) {
                cfg.total_ticks = 30;
            }
        }
    }

    std::cout << "\nChoose dynamic system:\n"
              << " 1) Linear ARX (Model 1.8)\n"
              << " 2) Dead-zone non-linear (Model 2.3)\n"
              << " 3) Continuous ODE (Model 3.3)\n"
              << "Selection: ";
    if (std::string line; std::getline(std::cin, line) && !line.empty()) {
        try {
            cfg.model_id = std::clamp(std::stoi(line), 1, 3);
        } catch (...) {
            cfg.model_id = 1;
        }
    }

    std::cout << "\nChoose excitation type:\n"
              << " 1) Heaviside Step\n"
              << " 2) Dirac Impulse\n"
              << " 3) Sinusoidal Wave\n"
              << "Selection: ";
    if (std::string line; std::getline(std::cin, line) && !line.empty()) {
        try {
            const int sig_id = std::stoi(line);
            if (sig_id == 2) cfg.signal_type = DiscreteSignalPattern::DiracImpulse;
            else if (sig_id == 3) cfg.signal_type = DiscreteSignalPattern::SinusoidalWave;
            else cfg.signal_type = DiscreteSignalPattern::HeavisideStep;
        } catch (...) {
            cfg.signal_type = DiscreteSignalPattern::HeavisideStep;
        }
    }

    return cfg;
}

int main(int argc, char* argv[]) {
    const ExperimentConfig cfg = configureExperiment(argc, argv);

    std::unique_ptr<Model> system;
    std::string system_tag;

    std::cout << "\n=== Dynamic Parameters Setup ===\n";
    if (cfg.model_id == 2) {
        const double a = readField("Coeff a", 0.6);
        const double b = readField("Coeff b", 0.5);
        const double d = readField("Dead-zone delta", 0.2);
        const double y0 = readField("Init y0", 0.0);
        system = std::make_unique<Model2_3>(a, b, d, y0);
        system_tag = "Model 2.3";
    } else if (cfg.model_id == 3) {
        const double a = readField("Coeff a", 0.5);
        const double b = readField("Coeff b", 0.8);
        const double dt = readField("Step dt", 0.1);
        const double y0 = readField("Init y0", 0.0);
        system = std::make_unique<Model3_3>(a, b, dt, y0);
        system_tag = "Model 3.3";
    } else {
        const double a1 = readField("Coeff a1", 0.5);
        const double a2 = readField("Coeff a2", 0.2);
        const double b1 = readField("Coeff b1", 0.4);
        const double b2 = readField("Coeff b2", 0.1);
        const double y0 = readField("Init y0", 0.0);
        system = std::make_unique<Model1_8>(a1, a2, b1, b2, y0);
        system_tag = "Model 1.8";
    }

    std::ofstream export_file("results.csv");
    if (!export_file.is_open()) {
        std::cerr << "Cannot open export destination 'results.csv'\n";
        return 1;
    }

    export_file << "step,signal,u,y,model_name\n";
    const std::string_view sig_label = SignalGenerator::nameOf(cfg.signal_type);

    std::cout << "\n" << std::string(36, '=') << "\n";
    std::cout << " Simulation in progress...\n";
    std::cout << std::string(36, '=') << "\n";
    std::cout << std::setw(6) << "Tick"
              << std::setw(12) << "Control (u)"
              << std::setw(16) << "Output (y)" << "\n";

    for (int k = 0; k < cfg.total_ticks; ++k) {
        const double u_k = SignalGenerator::sample(cfg.signal_type, k);
        const double y_k = system->step(u_k);

        std::cout << std::setw(6) << k
                  << std::fixed << std::setprecision(4)
                  << std::setw(12) << u_k
                  << std::setw(16) << y_k << "\n";

        export_file << k << "," << sig_label << "," << u_k << "," << y_k << "," << system_tag << "\n";
    }

    export_file.close();
    std::cout << "\nData recorded to 'results.csv'.\n";
    return 0;
}
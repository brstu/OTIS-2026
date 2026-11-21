#include "Application.h"

#include <iostream>

#include "ConsoleInput.h"
#include "InputSignals.h"
#include "Model1_7.h"
#include "Model2_9.h"
#include "Model3_1.h"
#include "Report.h"
#include "Simulation.h"

namespace {
constexpr double DEFAULT_A = 0.5;
constexpr double DEFAULT_B1 = 0.4;
constexpr double DEFAULT_B2 = 0.2;
constexpr double DEFAULT_B3 = 0.1;
constexpr double DEFAULT_A_NONLINEAR = 0.6;
constexpr double DEFAULT_B = 0.5;
constexpr double DEFAULT_A_DECAY = 0.8;
constexpr double DEFAULT_DT = 0.05;
constexpr int DEFAULT_N = 20;
}  // namespace

std::unique_ptr<InputSignal> makeSignal(int choice) {
    switch (choice) {
        case 1: return std::make_unique<StepInput>();
        case 2: return std::make_unique<ImpulseInput>();
        case 3: return std::make_unique<HarmonicInput>();
        default: return nullptr;
    }
}

std::unique_ptr<Model> makeModel(int choice, std::istream& in, std::ostream& out) {
    if (choice == 1) {
        const double a = readDouble(in, out, "Enter coefficient a", DEFAULT_A);
        const double b1 = readDouble(in, out, "Enter coefficient b1", DEFAULT_B1);
        const double b2 = readDouble(in, out, "Enter coefficient b2", DEFAULT_B2);
        const double b3 = readDouble(in, out, "Enter coefficient b3", DEFAULT_B3);
        return std::make_unique<Model1_7>(a, b1, b2, b3);
    }
    if (choice == 2) {
        const double a = readDouble(in, out, "Enter coefficient a", DEFAULT_A_NONLINEAR);
        const double b = readDouble(in, out, "Enter coefficient b", DEFAULT_B);
        return std::make_unique<Model2_9>(a, b);
    }
    if (choice == 3) {
        const double a = readDouble(in, out, "Enter coefficient a", DEFAULT_A_DECAY);
        const double dt = readDouble(in, out, "Enter time step dt", DEFAULT_DT);
        return std::make_unique<Model3_1>(a, dt);
    }
    return nullptr;
}

int runApplication(std::istream& in, std::ostream& out, const std::string& csvPath) {
    try {
        out << "===========================================\n"
            << " Lab #2 (OTIS) - ii02914\n"
            << " Modeling of a controlled object + stability\n"
            << "===========================================\n\n";

        out << "Choose a model:\n"
            << "  1) Model 1.7 - Linear (Multi-Step Control History)\n"
            << "  2) Model 2.9 - Non-linear (Square Root Modulated Action)\n"
            << "  3) Model 3.1 - Diff. equation (Pure Linear Decay)\n";
        const int modelChoice = readChoice(in, out, "Your choice (1-3): ", 1, 3);
        std::unique_ptr<Model> model = makeModel(modelChoice, in, out);

        out << "\nModel: " << model->name() << "\n";

        if (printStabilityWarning(*model, out)) {
            out << "Continue simulation? (y/n): ";
            std::string ans;
            if (!std::getline(in, ans)) throw InputClosed();
            if (ans != "y" && ans != "Y") {
                out << "Cancelled by user.\n";
                return 0;
            }
        }

        out << "\nChoose input signal:\n"
            << "  1) Step\n  2) Impulse\n  3) Harmonic\n";
        const int sigChoice = readChoice(in, out, "Your choice (1-3): ", 1, 3);
        const std::unique_ptr<InputSignal> signal = makeSignal(sigChoice);
        out << "Signal: " << signal->name() << "\n";

        const int n = readInt(in, out, "\nEnter number of simulation steps n", DEFAULT_N);
        if (n <= 0) {
            out << "n must be > 0\n";
            return 1;
        }

        const auto steps = simulate(*model, *signal, n);
        out << "\n===========================================\n";
        printTable(out, steps);
        out << "-------------------------------------------\n";

        if (!saveCsv(csvPath, steps)) {
            out << "Could not write " << csvPath << "\n";
            return 1;
        }
        out << "Results saved to " << csvPath << "\nDone.\n";
        return 0;
    } catch (const InputClosed&) {
        out << "\nInput closed, aborting.\n";
        return 1;
    }
}

#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>
#include <string>
#include <cstdlib> 

#include "DynamicModel.h"
#include "SignalGenerator.h"
#include "ModelCreator.h"

static int readInt(const std::string& prompt, int minValue)
{
    int value;
    while (true)
    {
        std::cout << prompt;
        if (std::cin >> value && value >= minValue) return value;
        std::cout << "  Invalid input. Try again.\n";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }
}

static std::unique_ptr<InputSignal> selectSignal(int choice)
{
    switch (choice)
    {
    case 1: return std::make_unique<StepSignal>(1.0);
    case 2: return std::make_unique<ImpulseSignal>();
    case 3: return std::make_unique<HarmonicSignal>();
    default: return nullptr;
    }
}

int main()
{
    std::cout << " OTIS-2026 | Lab #1 | Variant 27\n";
    std::cout << " Student: ii02927\n\n";

    std::cout << "Select a model:\n";
    std::cout << "  1) Model 1.6 (Third-Order System)\n";
    std::cout << "  2) Model 2.8 (Chaotic Logistic Map)\n";
    std::cout << "  3) Model 3.10 (External Constant Offset)\n";
    const int modelChoice = readInt("Your choice (1-3): ", 1);

    if (modelChoice > 3) { std::cerr << "Invalid model.\n"; return 1; }

    std::cout << "\nEnter model parameters:\n";
    std::unique_ptr<DynamicModel> model = ModelCreator::create(modelChoice);
    if (!model) return 1;

    std::cout << "\nModel: " << model->describe() << "\n";

    std::cout << "\nSelect input signal:\n";
    std::cout << "  1) Step\n";
    std::cout << "  2) Impulse\n";
    std::cout << "  3) Harmonic\n";
    const int signalChoice = readInt("Your choice (1-3): ", 1);

    std::unique_ptr<InputSignal> signal = selectSignal(signalChoice);
    if (!signal) return 1;
    std::cout << "Signal: " << signal->name() << "\n";

    const int steps = readInt("\nNumber of simulation steps n: ", 1);

    std::cout << "\n============================================\n";
    std::cout << std::setw(6) << "tau" << std::setw(14) << "u_tau" << std::setw(14) << "y_tau" << "\n";
    std::cout << "--------------------------------------------\n";

    std::ofstream csv("result.csv");
    csv << "tau,u_tau,y_tau\n";

    model->reset();
    for (int tau = 1; tau <= steps; ++tau)
    {
        const double u = signal->value(tau);
        const double y = model->advance(u);

        std::cout << std::setw(6) << tau
            << std::setw(14) << std::fixed << std::setprecision(5) << u
            << std::setw(14) << std::fixed << std::setprecision(5) << y << "\n";

        csv << tau << "," << u << "," << y << "\n";
    }
    csv.close();

    std::cout << "--------------------------------------------\n";
    std::cout << "Results saved to result.csv\n";

    std::cout << "Generating plot with Python...\n";
    const std::string plotCmd = "python ../../plot_results.py result.csv plot.png \"Simulation Result\"";
    if (const int result = std::system(plotCmd.c_str()); result == 0) // NOSONAR
    {
        std::cout << "[OK] Plot generated. Opening in default viewer...\n";
        std::system("start plot.png");
    }
    else
    {
        std::cout << "[WARNING] Python failed. Skipping plot.\n";
    }

    std::cout << "Done.\n";
    return 0;
}
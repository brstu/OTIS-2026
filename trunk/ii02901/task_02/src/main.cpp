#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <iomanip>
#include <stdexcept>
#include "model.h"
#include "input.h"

namespace {
    constexpr int TABLE_WIDTH = 50;
    
    std::unique_ptr<model> createModel(int choice) {
        switch (choice) {
            case 1: {
                double param_a;
				double param_b;
                std::cout << "\n- Model 1.1 (First-Order Inertial System) -\n";
                std::cout << "Enter a1 b1: (|a| < 1)";
                if (!(std::cin >> param_a >> param_b)) {
                    throw std::runtime_error("Invalid input for model 1.1 parameters.");
                }
                auto mdl = std::make_unique<model1_1>(param_a, param_b);
                if (mdl->isStable()) {
                    std::cout << "Result: STABLE (root is inside the unit circle)\n";
                } else {
                    std::cout << "Result: UNSTABLE (root is on/outside the unit circle)\n";
                }
                return mdl;
            }
            case 2: {
                double param_a;
				double param_b;
				double param_delta;
                std::cout << "\n- Model 2.3 (Dead-Zone Non-linearity) -\n";
                std::cout << "Enter a2 b2 delta: ";
                if (!(std::cin >> param_a >> param_b >> param_delta)) {
                    throw std::runtime_error("Invalid input for model 2.3 parameters.");
                }
                return std::make_unique<model2_3>(param_a, param_b, param_delta);
            }
            case 3: {
                double param_b;
				double param_dt;
                std::cout << "\n- Model 3.5 (Harmonic Driving Force) -\n";
                std::cout << "Enter b3 dt: ";
                if (!(std::cin >> param_b >> param_dt)) {
                    throw std::runtime_error("Invalid input for model 3.5 parameters.");
                }
                return std::make_unique<model3_5>(param_b, param_dt);
            }
            default:
                throw std::invalid_argument("Unknown model choice.");
        }
    }
    
    std::unique_ptr<InputGenerator> createInputGenerator(int choice) {
        switch (choice) {
            case 1: return std::make_unique<StepInput>();
            case 2: return std::make_unique<ImpulseInput>();
            case 3: return std::make_unique<HarmonicInput>();
            default:
                throw std::invalid_argument("Unknown input type.");
        }
    }
    
    std::string getInputName(int choice) {
        switch (choice) {
            case 1: return "Step";
            case 2: return "Impulse";
            case 3: return "Harmonic";
            default: return "Unknown";
        }
    }
    
    void printTableHeader() {
        std::cout << "\n" << std::setw(6) << "tau"
                  << " | " << std::setw(12) << "u(tau)"
                  << " | " << std::setw(12) << "Input"
                  << " | " << std::setw(12) << "y(tau)" << "\n";
        std::cout << std::string(TABLE_WIDTH, '-') << "\n";
    }
    
    unsigned int readSimulationSteps() {
        unsigned int steps;
        std::cout << "Enter number of simulation steps:\n";
        if (!(std::cin >> steps)) {
            throw std::runtime_error("Invalid input for number of steps.");
        }
        return steps;
    }
    
    int readModelChoice() {
        int choice;
        std::cout << "Select Model: (1 - Model 1_1, 2 - Model 2_3, 3 - Model 3_5)\n";
        if (!(std::cin >> choice)) {
            throw std::runtime_error("Invalid model choice.");
        }
        return choice;
    }
    
    int readInputType() {
        int type;
        std::cout << "Select Input Type: (1 - Step, 2 - Impulse, 3 - Harmonic)\n";
        if (!(std::cin >> type)) {
            throw std::runtime_error("Invalid input type choice.");
        }
        return type;
    }
}

int main() {
    try {
        const auto total_steps = readSimulationSteps();
        const auto model_choice = readModelChoice();
        const auto signal_choice = readInputType();
        
        auto system_model = createModel(model_choice);
        auto signal_source = createInputGenerator(signal_choice);
        const auto signal_label = getInputName(signal_choice);
        
        printTableHeader();
        
        std::ofstream output_stream("results.csv");
        if (!output_stream) {
            std::cerr << "Failed to open results.csv for writing.\n";
            return 1;
        }
        
        output_stream << "tau;u(tau);Input;y(tau)\n";
        
        double current_state = 0.0;
        for (unsigned int time_index = 0; time_index < total_steps; ++time_index) {
            const auto control_input = signal_source->generate(static_cast<int>(time_index));
            current_state = system_model->step(current_state, control_input);
            
            std::cout << std::setw(6) << time_index
                      << " | " << std::setw(12) << std::fixed << std::setprecision(4) << control_input
                      << " | " << std::setw(12) << signal_label
                      << " | " << std::setw(12) << current_state << "\n";
            
            output_stream << time_index << ";" << control_input << ";" << signal_label << ";" << current_state << "\n";
        }
        
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << "\n";
        return 1;
    }
}
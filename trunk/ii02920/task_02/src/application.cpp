#include "application.hpp"
#include "Model1_10.h"
#include "Model2_5.h"
#include "Model3_10.h"
#include "simulation.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

namespace {
std::string readLine(std::istream& input) {
    std::string line;
    if (!std::getline(input, line))
        throw SimulationError("Input ended before all parameters were entered.");
    return line;
}

template <class T, class Predicate>
T readNumber(std::istream& input, std::ostream& output, const char* prompt,
             T fallback, Predicate valid) {
    for (;;) {
        output << prompt << " [default " << fallback << "]: ";
        const std::string line = readLine(input);
        if (line.find_first_not_of(" \t\r") == std::string::npos) return fallback;
        std::istringstream parser(line);
        T value{};
        if (char extra{}; (parser >> value) && !(parser >> extra) && valid(value)) return value;
        output << "Invalid value. Please try again.\n";
    }
}
}  // namespace

int runApplication(std::istream& input, std::ostream& output, std::ostream& errors,
                   const std::filesystem::path& csvPath) {
    try {
        auto number = [&](const char* prompt, double fallback) {
            return readNumber(input, output, prompt, fallback,
                              [](double x) { return std::isfinite(x); });
        };
        auto choice = [&]() {
            return readNumber(input, output, "Your choice (1-3)", 1,
                              [](int i) { return i >= 1 && i <= 3; });
        };
        output << "OTIS | Lab 2 | ii02920 | Variant 25\n"
               << "Press Enter for the default value.\n"
               << "1) Model 1.10 - delayed input\n2) Model 2.5 - nonlinear input\n"
               << "3) Model 3.10 - Euler method\n";
        const int type = choice();
        const double a = number(type == 1 ? "Enter a1" : "Enter a", type == 3 ? 1.0 : 0.5);
        const double b = number(type == 1 ? "Enter b1" : "Enter b", type == 3 ? 0.5 : 0.4);
        double b2 = 0;
        double dt = 0.1;
        if (type == 1) b2 = number("Enter b2", 0.1);
        if (type == 3)
            dt = readNumber(input, output, "Enter positive dt", 0.1,
                            [](double x) { return std::isfinite(x) && x > 0.0; });
        const double y0 = number("Enter initial value y0", 0.0);
        std::unique_ptr<Model> model;
        if (type == 1) model = std::make_unique<Model1_10>(a, b, b2, y0);
        else if (type == 2) model = std::make_unique<Model2_5>(a, b, y0);
        else model = std::make_unique<Model3_10>(a, b, dt, y0);

        output << "\nModel: " << model->name() << ", z = " << model->pole() << '\n';
        if (!model->isStable()) {
            output << model->stabilityWarning() << "\nContinue simulation? (y/n): ";
            const std::string answer = readLine(input);
            if (answer != "y" && answer != "Y") {
                output << "Cancelled by user.\n";
                return 0;
            }
        }
        output << "Choose input signal:\n1) Step\n2) Impulse\n3) Harmonic\n";
        const int signalType = choice();
        std::unique_ptr<InputSignal> signal;
        if (signalType == 1)
            signal = std::make_unique<StepInput>(number("Enter step amplitude A", 1.0));
        else if (signalType == 2) signal = std::make_unique<ImpulseInput>();
        else signal = std::make_unique<HarmonicInput>();
        const int steps = readNumber(input, output, "Number of steps (1-10000)", 20,
                                     [](int n) { return n >= 1 && n <= 10000; });
        if (type == 3 && !std::isfinite(dt * steps))
            throw SimulationError("Final time is too large.");

        std::ofstream csv(csvPath);
        if (!csv) throw SimulationError("Cannot create the CSV file.");
        output << "Signal: " << signal->name() << '\n';
        simulate(*model, *signal, steps, csv, output);
        csv.close();
        if (!csv) throw SimulationError("Failed to write the CSV file.");
        output << "Results saved to " << csvPath.string() << '\n';
        return 0;
    } catch (const SimulationError& error) {
        errors << "ERROR: " << error.what() << '\n';
        return 1;
    }
}

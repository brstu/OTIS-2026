#include "control.hpp"

#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#endif

double readDouble(const std::string& prompt) {
    double value = 0.0;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && std::isfinite(value)) return value;
        std::cout << "Incorrect value. Please enter a number.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

int readInt(const std::string& prompt, int minimum, int maximum) {
    int value = 0;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= minimum && value <= maximum) return value;
        std::cout << "Incorrect value. Enter an integer from " << minimum
                  << " to " << maximum << ".\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::unique_ptr<Plant> createPlant(int choice) {
    if (choice == 1) {
        std::cout << "Model 1.9 coefficients:\n";
        LinearPlantParameters p{};
        p.a1 = readDouble("a1 = ");
        p.a2 = readDouble("a2 = ");
        p.a3 = readDouble("a3 = ");
        p.b1 = readDouble("b1 = ");
        return std::make_unique<LinearPlant19>(p);
    }
    if (choice == 2) {
        std::cout << "Model 2.1 coefficients:\n";
        NonlinearPlantParameters p{};
        p.a = readDouble("a = ");
        p.b = readDouble("b = ");
        p.c = readDouble("c = ");
        p.d = readDouble("d = ");
        return std::make_unique<NonlinearPlant21>(p);
    }

    std::cout << "Model 3.3 coefficients:\n";
    DifferentialPlantParameters p{};
    p.a = readDouble("a = ");
    p.b = readDouble("b = ");
    do {
        p.dt = readDouble("Plant integration step dt (> 0) = ");
        if (p.dt <= 0.0) std::cout << "dt must be positive.\n";
    } while (p.dt <= 0.0);
    return std::make_unique<DifferentialPlant33>(p);
}

PIDController createPid() {
    std::cout << "PID controller settings:\n";
    const double kp = readDouble("Kp = ");
    const double ki = readDouble("Ki = ");
    const double kd = readDouble("Kd = ");
    double dt = 0.0;
    do {
        dt = readDouble("PID time step dt (> 0) = ");
        if (dt <= 0.0) std::cout << "dt must be positive.\n";
    } while (dt <= 0.0);

    double minimumControl = 0.0;
    double maximumControl = 0.0;
    do {
        minimumControl = readDouble("Minimum control = ");
        maximumControl = readDouble("Maximum control = ");
        if (minimumControl >= maximumControl)
            std::cout << "Minimum control must be less than maximum control.\n";
    } while (minimumControl >= maximumControl);
    return PIDController(kp, ki, kd, dt, minimumControl, maximumControl);
}

void printTable(const std::vector<Sample>& samples) {
    std::cout << std::setw(7) << "t" << std::setw(12) << "w"
              << std::setw(12) << "e(t)" << std::setw(12) << "u(t)"
              << std::setw(12) << "y(t)" << '\n';
    for (const auto& s : samples) {
        std::cout << std::fixed << std::setprecision(4) << std::setw(7) << s.step
                  << std::setw(12) << s.setpoint << std::setw(12) << s.error
                  << std::setw(12) << s.control << std::setw(12) << s.output << '\n';
    }
}

void openGraph(const std::filesystem::path& path) {
    const std::string absolutePath = std::filesystem::absolute(path).string();
#ifdef _WIN32
    ShellExecuteA(nullptr, "open", absolutePath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#elif defined(__APPLE__)
    const std::string command = "open \"" + absolutePath + "\"";
    std::system(command.c_str());
#else
    const std::string command = "xdg-open \"" + absolutePath + "\" >/dev/null 2>&1 &";
    std::system(command.c_str());
#endif
}

int main(int argc, char* argv[]) {
    try {
        std::cout << "Laboratory work 3, variant 9\n";
        const int choice = readInt(
            "Choose plant: 1 - linear 1.9, 2 - nonlinear 2.1, 3 - differential 3.3: ", 1, 3);
        auto plant = createPlant(choice);
        PIDController pid = createPid();
        const double setpoint = readDouble("Setpoint temperature w = ");
        const int mode = readInt(
            "Mode: 1 - fixed number of steps, 2 - until steady state: ", 1, 2);

        std::vector<Sample> samples;
        if (mode == 1) {
            const int steps = readInt("Number of steps = ", 1, 100000);
            samples = runFixed(*plant, pid, setpoint, steps);
        } else {
            double epsilon = 0.0;
            do {
                epsilon = readDouble("Allowed error epsilon (> 0) = ");
                if (epsilon <= 0.0) std::cout << "epsilon must be positive.\n";
            } while (epsilon <= 0.0);
            samples = runUntilSettled(*plant, pid, setpoint, epsilon, 10000, 5);
            if (samples.size() == 10000)
                std::cout << "Warning: the safety limit of 10000 steps was reached.\n";
            else
                std::cout << "Steady state reached in " << samples.size() << " steps.\n";
        }

        printTable(samples);
        const std::string base = plant->name() + "_pid";
        std::ostringstream title;
        title << "Замкнутая система: w=" << setpoint
              << ", Kp=" << pid.kp() << ", Ki=" << pid.ki() << ", Kd=" << pid.kd();
        saveResults(base, samples, title.str());
        const std::filesystem::path graphPath = std::filesystem::path("results") / (base + ".svg");
        std::cout << "Saved local CSV and graph in results/.\n";

        const bool disableOpening = argc > 1 && std::string(argv[1]) == "--no-open";
        if (!disableOpening) openGraph(graphPath);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}

#include "models.hpp"
#include "stability.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>

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

std::unique_ptr<Model> createModel(int choice, std::string& modelName) {
    if (choice == 1) {
        LinearParameters p{};
        std::cout << "Model 1.9 coefficients:\n";
        p.a1 = readDouble("a1 = ");
        p.a2 = readDouble("a2 = ");
        p.a3 = readDouble("a3 = ");
        p.b1 = readDouble("b1 = ");

        const StabilityResult result = analyzeStability(p);
        std::cout << "Roots of z^3-a1*z^2-a2*z-a3:\n";
        for (const auto& root : result.roots)
            std::cout << "  z = " << root << ", |z| = " << std::abs(root) << '\n';
        if (result.stable)
            std::cout << "The linear model is stable: all |z| < 1.\n";
        else
            std::cout << "WARNING: the linear model is unstable. The result may diverge.\n";

        modelName = "linear_1_9";
        return std::make_unique<LinearModel19>(p);
    }

    if (choice == 2) {
        NonlinearParameters p{};
        std::cout << "Model 2.1 coefficients:\n";
        p.a = readDouble("a = ");
        p.b = readDouble("b = ");
        p.c = readDouble("c = ");
        p.d = readDouble("d = ");
        modelName = "nonlinear_2_1";
        return std::make_unique<NonlinearModel21>(p);
    }

    DifferentialParameters p{};
    std::cout << "Model 3.3 coefficients:\n";
    p.a = readDouble("a = ");
    p.b = readDouble("b = ");
    do {
        p.dt = readDouble("dt (> 0) = ");
        if (p.dt <= 0.0) std::cout << "dt must be positive.\n";
    } while (p.dt <= 0.0);
    modelName = "differential_3_3";
    return std::make_unique<DifferentialModel33>(p);
}

void writeSvg(const std::filesystem::path& path, const std::string& title,
              const std::vector<double>& input, const std::vector<double>& output) {
    constexpr int left = 70;
    constexpr int top = 60;
    constexpr int width = 790;
    constexpr int height = 370;
    double minimum = 0.0;
    double maximum = 0.0;
    for (double value : input) {
        minimum = std::min(minimum, value);
        maximum = std::max(maximum, value);
    }
    for (double value : output) {
        minimum = std::min(minimum, value);
        maximum = std::max(maximum, value);
    }
    if (std::abs(maximum - minimum) < 1e-12) {
        minimum -= 1.0;
        maximum += 1.0;
    }
    const double padding = (maximum - minimum) * 0.1;
    minimum -= padding;
    maximum += padding;
    auto x = [&](std::size_t i) {
        const auto lastIndex = static_cast<double>(input.size() - 1);
        return left + (input.size() < 2 ? 0.0 : static_cast<double>(i) * width / lastIndex);
    };
    auto y = [&](double value) {
        return top + (maximum - value) * height / (maximum - minimum);
    };

    std::ofstream file(path);
    file << R"(<svg xmlns="http://www.w3.org/2000/svg" width="900" height="500">)" << '\n'
         << R"(<rect width="900" height="500" fill="white"/>)" << '\n'
         << R"(<text x="450" y="30" text-anchor="middle" font-family="Arial" font-size="20">)"
         << title << "</text>\n"
         << R"(<rect x="70" y="60" width="790" height="370" fill="#f8fafc" stroke="#334155"/>)" << '\n'
         << R"(<polyline fill="none" stroke="#2563eb" stroke-width="3" points=")";
    for (std::size_t i = 0; i < input.size(); ++i) file << x(i) << ',' << y(input[i]) << ' ';
    file << "\"/>\n<polyline fill=\"none\" stroke=\"#dc2626\" stroke-width=\"3\" points=\"";
    for (std::size_t i = 0; i < output.size(); ++i) file << x(i) << ',' << y(output[i]) << ' ';
    file << "\"/>\n<text x=\"670\" y=\"465\" fill=\"#2563eb\" font-family=\"Arial\">input u(t)</text>\n"
         << "<text x=\"790\" y=\"465\" fill=\"#dc2626\" font-family=\"Arial\">output y(t)</text>\n"
         << "</svg>\n";
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
    std::cout << "Laboratory work 2, variant 9\n";
    const int modelChoice = readInt(
        "Choose model: 1 - linear 1.9, 2 - nonlinear 2.1, 3 - differential 3.3: ", 1, 3);
    std::string modelName;
    auto model = createModel(modelChoice, modelName);

    const int signalChoice = readInt(
        "Choose input: 1 - step, 2 - impulse, 3 - harmonic: ", 1, 3);
    const auto signal = static_cast<SignalType>(signalChoice - 1);
    const double amplitude = readDouble("Signal amplitude = ");
    const int steps = readInt("Number of simulation steps = ", 1, 100000);

    std::filesystem::create_directories("results");
    const std::string base = modelName + "_" + signalName(signal);
    const std::filesystem::path csvPath = std::filesystem::path("results") / (base + ".csv");
    const std::filesystem::path graphPath = std::filesystem::path("results") / (base + ".svg");
    std::ofstream csv(csvPath);
    if (!csv) {
        std::cerr << "Cannot create " << csvPath << ".\n";
        return 1;
    }
    csv << "step,input,output\n";

    std::vector<double> inputs;
    std::vector<double> outputs;
    inputs.reserve(static_cast<std::size_t>(steps));
    outputs.reserve(static_cast<std::size_t>(steps));
    std::cout << '\n' << std::setw(8) << "step" << std::setw(14) << "input"
              << std::setw(14) << "output" << '\n';

    double previousU = 0.0;
    for (int step = 0; step < steps; ++step) {
        const double u = signalValue(signal, step, amplitude);
        const double y = model->next(u, previousU);
        inputs.push_back(u);
        outputs.push_back(y);
        csv << step << ',' << u << ',' << y << '\n';
        std::cout << std::fixed << std::setprecision(6) << std::setw(8) << step
                  << std::setw(14) << u << std::setw(14) << y << '\n';
        previousU = u;
    }
    csv.close();

    writeSvg(graphPath, base, inputs, outputs);
    std::cout << "\nCSV table: " << csvPath << "\nGraph: " << graphPath << "\n";
    if (const bool disableOpening = argc > 1 && std::string(argv[1]) == "--no-open";
        !disableOpening) {
        openGraph(graphPath);
    }
    return 0;
}

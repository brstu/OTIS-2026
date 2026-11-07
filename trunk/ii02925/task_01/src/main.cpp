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

class Model {
public:
    virtual ~Model() = default;
    virtual double next(double u, double previousU) = 0;
    virtual void reset() = 0;
    virtual std::string name() const = 0;
};

class LinearModel19 final : public Model {
public:
    LinearModel19(double a1, double a2, double a3, double b1)
        : a1_(a1), a2_(a2), a3_(a3), b1_(b1) {}

    double next(double u, double) override {
        const double result = a1_ * y0_ + a2_ * y1_ + a3_ * y2_ + b1_ * u;
        y2_ = y1_;
        y1_ = y0_;
        y0_ = result;
        return result;
    }

    void reset() override {
        y0_ = 0.0;
        y1_ = 0.0;
        y2_ = 0.0;
    }
    std::string name() const override { return "linear_1_9"; }

private:
    double a1_;
    double a2_;
    double a3_;
    double b1_;
    double y0_ = 0.0;
    double y1_ = 0.0;
    double y2_ = 0.0;
};

class NonlinearModel21 final : public Model {
public:
    NonlinearModel21(double a, double b, double c, double d)
        : a_(a), b_(b), c_(c), d_(d) {}

    double next(double u, double previousU) override {
        const double result = a_ * y0_ - b_ * y1_ * y1_ + c_ * u
                            + d_ * std::sin(previousU);
        y1_ = y0_;
        y0_ = result;
        return result;
    }

    void reset() override {
        y0_ = 0.0;
        y1_ = 0.0;
    }
    std::string name() const override { return "nonlinear_2_1"; }

private:
    double a_;
    double b_;
    double c_;
    double d_;
    double y0_ = 0.0;
    double y1_ = 0.0;
};

class DifferentialModel33 final : public Model {
public:
    DifferentialModel33(double a, double b, double dt)
        : a_(a), b_(b), dt_(dt) {}

    double next(double u, double) override {
        y_ += dt_ * (-a_ * y_ + b_ * u);
        return y_;
    }

    void reset() override { y_ = 0.0; }
    std::string name() const override { return "differential_3_3"; }

private:
    double a_;
    double b_;
    double dt_;
    double y_ = 0.0;
};

enum class Signal { Step = 1, Impulse = 2, Harmonic = 3 };

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

std::unique_ptr<Model> createModel(int choice) {
    if (choice == 1) {
        std::cout << "Model 1.9: y(t+1)=a1*y(t)+a2*y(t-1)+a3*y(t-2)+b1*u(t)\n";
        const double a1 = readDouble("a1 = ");
        const double a2 = readDouble("a2 = ");
        const double a3 = readDouble("a3 = ");
        const double b1 = readDouble("b1 = ");
        return std::make_unique<LinearModel19>(a1, a2, a3, b1);
    }
    if (choice == 2) {
        std::cout << "Model 2.1: y(t+1)=a*y(t)-b*y(t-1)^2+c*u(t)+d*sin(u(t-1))\n";
        const double a = readDouble("a = ");
        const double b = readDouble("b = ");
        const double c = readDouble("c = ");
        const double d = readDouble("d = ");
        return std::make_unique<NonlinearModel21>(a, b, c, d);
    }

    std::cout << "Model 3.3: dy/dt=-a*y+b*u (Euler method)\n";
    const double a = readDouble("a = ");
    const double b = readDouble("b = ");
    double dt = 0.0;
    do {
        dt = readDouble("dt (> 0) = ");
        if (dt <= 0.0) std::cout << "dt must be positive.\n";
    } while (dt <= 0.0);
    return std::make_unique<DifferentialModel33>(a, b, dt);
}

double signalValue(Signal signal, int step, double amplitude) {
    if (signal == Signal::Step) return amplitude;
    if (signal == Signal::Impulse) return step == 0 ? amplitude : 0.0;
    return amplitude * std::sin(static_cast<double>(step));
}

std::string signalName(Signal signal) {
    if (signal == Signal::Step) return "step";
    if (signal == Signal::Impulse) return "impulse";
    return "harmonic";
}

std::string graphTitle(int modelChoice, Signal signal) {
    std::string model = "Модель 3.3";
    if (modelChoice == 1) model = "Модель 1.9";
    if (modelChoice == 2) model = "Модель 2.1";

    std::string input = "гармонический сигнал";
    if (signal == Signal::Step) input = "ступенчатый сигнал";
    if (signal == Signal::Impulse) input = "импульсный сигнал";
    return model + " — " + input;
}

void writeSvg(const std::filesystem::path& path, const std::string& title,
              const std::vector<double>& input, const std::vector<double>& output) {
    constexpr int left = 90;
    constexpr int top = 65;
    constexpr int plotWidth = 850;
    constexpr int plotHeight = 400;
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
        maximum += 1.0;
        minimum -= 1.0;
    }
    const double padding = (maximum - minimum) * 0.1;
    minimum -= padding;
    maximum += padding;

    auto x = [&](std::size_t i) {
        const auto lastIndex = static_cast<double>(input.size() - 1);
        return left + (input.size() < 2 ? 0.0 : static_cast<double>(i) * plotWidth / lastIndex);
    };
    auto y = [&](double value) {
        return top + (maximum - value) * plotHeight / (maximum - minimum);
    };

    std::ofstream file(path);
    file << R"(<svg xmlns="http://www.w3.org/2000/svg" width="1000" height="570">)" << '\n'
         << R"(<rect width="1000" height="570" fill="white"/>)" << '\n'
         << R"(<text x="500" y="32" text-anchor="middle" font-family="Arial" font-size="20">)"
         << title << "</text>\n"
         << R"(<rect x="90" y="65" width="850" height="400" fill="#f8fafc" stroke="#334155"/>)" << '\n';

    file << std::fixed << std::setprecision(2);
    for (int division = 0; division <= 5; ++division) {
        const double gridY = top + division * plotHeight / 5.0;
        const double value = maximum - division * (maximum - minimum) / 5.0;
        file << R"(<line x1=")" << left << R"(" y1=")" << gridY << R"(" x2=")"
             << left + plotWidth << R"(" y2=")" << gridY
             << R"(" stroke="#cbd5e1" stroke-width="1"/>)" << '\n'
             << R"(<text x="80" y=")" << gridY + 5
             << R"(" text-anchor="end" font-family="Arial" font-size="12">)"
             << value << "</text>\n";
    }
    for (int division = 0; division <= 5; ++division) {
        const double gridX = left + division * plotWidth / 5.0;
        const int step = input.size() < 2 ? 0 :
            static_cast<int>(std::lround(division * static_cast<double>(input.size() - 1) / 5.0));
        file << "<line x1=\"" << gridX << "\" y1=\"" << top << "\" x2=\""
             << gridX << "\" y2=\"" << top + plotHeight
             << R"(" stroke="#cbd5e1" stroke-width="1"/>)" << '\n'
             << R"(<text x=")" << gridX << R"(" y="485" text-anchor="middle" )"
             << R"(font-family="Arial" font-size="12">)" << step << "</text>\n";
    }
    file << R"(<polyline fill="none" stroke="#2563eb" stroke-width="3" points=")";
    for (std::size_t i = 0; i < input.size(); ++i) file << x(i) << ',' << y(input[i]) << ' ';
    file << "\"/>\n<polyline fill=\"none\" stroke=\"#dc2626\" stroke-width=\"3\" points=\"";
    for (std::size_t i = 0; i < output.size(); ++i) file << x(i) << ',' << y(output[i]) << ' ';
    file << "\"/>\n";
    if (input.size() <= 100) {
        for (std::size_t i = 0; i < input.size(); ++i)
            file << "<circle cx=\"" << x(i) << "\" cy=\"" << y(input[i])
                 << "\" r=\"2.5\" fill=\"#2563eb\"/>\n";
        for (std::size_t i = 0; i < output.size(); ++i)
            file << "<circle cx=\"" << x(i) << "\" cy=\"" << y(output[i])
                 << "\" r=\"3\" fill=\"#dc2626\"/>\n";
    }
    file << R"(<text x="700" y="535" fill="#2563eb" font-family="Arial">Вход u(t)</text>)" << '\n'
         << R"(<text x="825" y="535" fill="#dc2626" font-family="Arial">Выход y(t)</text>)" << '\n'
         << R"(<text x="515" y="515" text-anchor="middle" font-family="Arial">Шаг t</text>)" << '\n'
         << R"(<text x="22" y="265" text-anchor="middle" font-family="Arial" )"
         << R"svg(transform="rotate(-90 22 265)">Значение</text>)svg" << "\n</svg>\n";
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
    std::cout << "Laboratory work 1, variant 9\n";
    const int modelChoice = readInt(
        "Choose model: 1 - linear 1.9, 2 - nonlinear 2.1, 3 - differential 3.3: ", 1, 3);
    auto model = createModel(modelChoice);

    const int signalChoice = readInt(
        "Choose input: 1 - step, 2 - impulse, 3 - harmonic: ", 1, 3);
    const auto signal = static_cast<Signal>(signalChoice);
    const double amplitude = readDouble("Signal amplitude = ");
    const int steps = readInt("Number of simulation steps = ", 1, 100000);

    std::filesystem::create_directories("results");
    const std::string base = model->name() + "_" + signalName(signal);
    const std::filesystem::path csvPath = std::filesystem::path("results") / (base + ".csv");
    const std::filesystem::path graphPath = std::filesystem::path("results") / (base + ".svg");

    std::ofstream csv(csvPath);
    if (!csv) {
        std::cerr << "Cannot create " << csvPath << ".\n";
        return 1;
    }
    csv << "step,input,output\n";

    std::cout << '\n' << std::setw(8) << "step" << std::setw(14) << "input"
              << std::setw(14) << "output" << '\n';
    std::vector<double> inputs;
    std::vector<double> outputs;
    inputs.reserve(static_cast<std::size_t>(steps));
    outputs.reserve(static_cast<std::size_t>(steps));

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
    writeSvg(graphPath, graphTitle(modelChoice, signal), inputs, outputs);
    std::cout << "\nCSV table: " << csvPath << "\nGraph: " << graphPath << "\n";

    if (const bool disableOpening = argc > 1 && std::string(argv[1]) == "--no-open";
        !disableOpening) {
        openGraph(graphPath);
    }
    return 0;
}

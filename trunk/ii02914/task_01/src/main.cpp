#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "InputSignal.h"
#include "Model.h"
#include "Model1_9.h"
#include "Model2_3.h"
#include "Model3_7.h"

namespace {

    constexpr const char* kOutputFile = "results.csv";
    constexpr int kDefaultSteps = 30;

    class Session {
    public:
        void configure(int argc, char** argv);
        void run() const;

    private:
        int readInteger(const char* prompt, int def, int lo, int hi) const;
        double readDouble(const char* prompt, double def) const;
        std::unique_ptr<Model> makeModel(std::string& name) const;

        int steps_ = kDefaultSteps;
        int model_id_ = 1;
        SignalType wave_ = SignalType::STEP;
    };

    int Session::readInteger(const char* prompt, int def, int lo, int hi) const {
        std::cout << prompt << " [" << def << "]: ";
        std::string buf;
        if (!std::getline(std::cin, buf) || buf.empty()) {
            return def;
        }
        std::istringstream in(buf);
        int v = 0;
        if (!(in >> v)) {
            return def;
        }
        if (v < lo) return lo;
        if (v > hi) return hi;
        return v;
    }

    double Session::readDouble(const char* prompt, double def) const {
        std::cout << prompt << " [" << def << "]: ";
        std::string buf;
        if (!std::getline(std::cin, buf) || buf.empty()) {
            return def;
        }
        std::istringstream in(buf);
        double v = 0.0;
        if (!(in >> v)) {
            return def;
        }
        return v;
    }

    void Session::configure(int argc, char** argv) {
        if (argc >= 2) {
            std::istringstream probe(argv[1]);
            int val = 0;
            if (probe >> val) {
                steps_ = std::clamp(val, 1, 1000);
            }
        }
        else {
            steps_ = readInteger("Specify simulation duration", kDefaultSteps, 1, 1000);
        }

        std::cout << "\nVariant 19 - pick a structure:\n"
            << " 1) Model 1.9 - three-step moving average\n"
            << " 2) Model 2.3 - dead-zone nonlinearity\n"
            << " 3) Model 3.7 - exponential scaling process\n";
        model_id_ = readInteger("Structure number", 1, 1, 3);

        std::cout << "\nPick an input wave:\n"
            << " 1) step\n"
            << " 2) impulse\n"
            << " 3) harmonic\n";
        switch (readInteger("Wave number", 1, 1, 3)) {
        case 2: wave_ = SignalType::IMPULSE; break;
        case 3: wave_ = SignalType::HARMONIC; break;
        default: wave_ = SignalType::STEP; break;
        }
    }

    std::unique_ptr<Model> Session::makeModel(std::string& name) const {
        std::cout << "\n--- Parameters ---\n";
        switch (model_id_) {
        case 2: {
            name = "Model 2.3";
            const double aa = readDouble("a", 0.6);
            const double bb = readDouble("b", 0.4);
            const double dd = readDouble("delta", 0.2);
            const double y0 = readDouble("y0", 0.0);
            return std::make_unique<Model2_3>(aa, bb, dd, y0);
        }
        case 3: {
            name = "Model 3.7";
            const double aa = readDouble("a", 0.5);
            const double bb = readDouble("b", 0.8);
            const double dt = readDouble("dt", 0.1);
            const double y0 = readDouble("y0", 0.0);
            return std::make_unique<Model3_7>(aa, bb, dt, y0);
        }
        default: {
            name = "Model 1.9";
            const double c1 = readDouble("a1", 0.4);
            const double c2 = readDouble("a2", 0.3);
            const double c3 = readDouble("a3", 0.1);
            const double d1 = readDouble("b1", 0.5);
            const double y0 = readDouble("y0", 0.0);
            return std::make_unique<Model1_9>(c1, c2, c3, d1, y0);
        }
        }
    }

    void Session::run() const {
        std::string model_name;
        auto model = makeModel(model_name);

        std::ofstream out(kOutputFile);
        if (!out) {
            std::cerr << "Cannot open " << kOutputFile << "\n";
            std::exit(1);
        }

        out << "step,signal,u,y,model_name\n";
        const std::string wave_name = InputSignal::getSignalName(wave_);

        std::cout << "\n============================================\n"
            << " Simulation started\n"
            << "============================================\n"
            << std::setw(6) << "k"
            << std::setw(12) << "u(k)"
            << std::setw(16) << "y(k)" << "\n";

        for (int k = 0; k < steps_; ++k) {
            const double u = InputSignal::getSignal(wave_, k);
            const double y = model->step(u);

            std::cout << std::setw(6) << k
                << std::fixed << std::setprecision(4)
                << std::setw(12) << u
                << std::setw(16) << y << "\n";

            out << k << ',' << wave_name << ',' << u << ','
                << y << ',' << model_name << '\n';
        }

        std::cout << "\nData written to " << kOutputFile << "\n";
    }

}  // namespace

int main(int argc, char** argv) {
    Session s;
    s.configure(argc, argv);
    s.run();
    return 0;
}
#include "control.hpp"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <stdexcept>

LinearPlant19::LinearPlant19(LinearPlantParameters parameters) : p_(parameters) {}
double LinearPlant19::update(double u) {
    const double next = p_.a1*y0_ + p_.a2*y1_ + p_.a3*y2_ + p_.b1*u;
    y2_ = y1_; y1_ = y0_; y0_ = next; return y0_;
}
void LinearPlant19::reset() { y0_ = y1_ = y2_ = 0.0; }
double LinearPlant19::output() const { return y0_; }
std::string LinearPlant19::name() const { return "linear_1_9"; }

NonlinearPlant21::NonlinearPlant21(NonlinearPlantParameters parameters) : p_(parameters) {}
double NonlinearPlant21::update(double u) {
    const double next = p_.a*y0_ - p_.b*y1_*y1_ + p_.c*u + p_.d*std::sin(previousU_);
    y1_ = y0_; y0_ = next; previousU_ = u; return y0_;
}
void NonlinearPlant21::reset() { y0_ = y1_ = previousU_ = 0.0; }
double NonlinearPlant21::output() const { return y0_; }
std::string NonlinearPlant21::name() const { return "nonlinear_2_1"; }

DifferentialPlant33::DifferentialPlant33(DifferentialPlantParameters parameters) : p_(parameters) {
    if (p_.dt <= 0.0) throw std::invalid_argument("Plant time step must be positive");
}
double DifferentialPlant33::update(double u) {
    y_ += p_.dt*(-p_.a*y_ + p_.b*u);
    return y_;
}
void DifferentialPlant33::reset() { y_ = 0.0; }
double DifferentialPlant33::output() const { return y_; }
std::string DifferentialPlant33::name() const { return "differential_3_3"; }

PIDController::PIDController(double kp, double ki, double kd, double dt,
                             double minimumControl, double maximumControl)
    : kp_(kp), ki_(ki), kd_(kd), dt_(dt), min_(minimumControl), max_(maximumControl) {
    if (dt <= 0.0 || minimumControl >= maximumControl)
        throw std::invalid_argument("Invalid PID time step or control limits");
}

double PIDController::update(double error) {
    const double derivative = first_ ? 0.0 : (error - previousError_) / dt_;
    const double candidateIntegral = integral_ + error * dt_;
    const double raw = kp_*error + ki_*candidateIntegral + kd_*derivative;
    const double limited = std::clamp(raw, min_, max_);
    // Conditional integration: do not grow the integral further into saturation.
    const bool pushesHigh = raw > max_ && error > 0.0;
    const bool pushesLow = raw < min_ && error < 0.0;
    if (!pushesHigh && !pushesLow) integral_ = candidateIntegral;
    previousError_ = error;
    first_ = false;
    return limited;
}

void PIDController::reset() { integral_ = previousError_ = 0.0; first_ = true; }

std::vector<Sample> runFixed(Plant& plant, PIDController& pid,
                             double setpoint, int steps) {
    if (steps <= 0) throw std::invalid_argument("steps must be positive");
    plant.reset(); pid.reset();
    std::vector<Sample> result;
    for (int step = 0; step < steps; ++step) {
        const double error = setpoint - plant.output();
        const double control = pid.update(error);
        const double output = plant.update(control);
        result.push_back({step, setpoint, output, setpoint-output, control});
    }
    return result;
}

std::vector<Sample> runUntilSettled(Plant& plant, PIDController& pid,
                                    double setpoint, double epsilon,
                                    int maximumSteps, int stableSteps) {
    if (epsilon <= 0.0 || maximumSteps <= 0 || stableSteps <= 0)
        throw std::invalid_argument("epsilon and step limits must be positive");
    plant.reset(); pid.reset();
    std::vector<Sample> result;
    int insideCount = 0;
    for (int step = 0; step < maximumSteps; ++step) {
        const double error = setpoint - plant.output();
        const double control = pid.update(error);
        const double output = plant.update(control);
        const double newError = setpoint-output;
        result.push_back({step, setpoint, output, newError, control});
        insideCount = std::abs(newError) <= epsilon ? insideCount + 1 : 0;
        if (insideCount >= stableSteps) break;
    }
    return result;
}

void saveResults(const std::string& baseName, const std::vector<Sample>& samples,
                 const std::string& graphTitle,
                 double scaleMinimum, double scaleMaximum) {
    if (samples.empty()) return;
    std::filesystem::create_directories("results");
    std::ofstream csv("results/" + baseName + ".csv");
    csv << "t,w,e,u,y\n";
    for (const auto& s : samples)
        csv << s.step << ',' << s.setpoint << ',' << s.error << ',' << s.control << ',' << s.output << '\n';

    double low = scaleMinimum, high = scaleMaximum;
    if (scaleMaximum <= scaleMinimum) {
        low = 0.0;
        high = samples.front().setpoint;
        for (const auto& s : samples) {
            low = std::min({low, s.output, s.control});
            high = std::max({high, s.output, s.control});
        }
        const double pad = std::max(0.1, (high-low)*0.1);
        low -= pad;
        high += pad;
    }
    constexpr double left = 90.0, top = 65.0, width = 850.0, height = 400.0;
    auto x = [&](std::size_t i) { return left + (samples.size()<2 ? 0.0 : i*width/(samples.size()-1)); };
    auto y = [&](double value) { return top + (high-value)*height/(high-low); };
    std::ofstream svg("results/" + baseName + ".svg");
    svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"1000\" height=\"570\">"
        << "<rect x=\"0\" y=\"0\" width=\"1000\" height=\"570\" fill=\"white\"/>"
        << "<text x=\"500\" y=\"32\" text-anchor=\"middle\" font-family=\"Arial\" font-size=\"20\">"
        << (graphTitle.empty() ? baseName : graphTitle) << "</text>"
        << "<rect x=\"90\" y=\"65\" width=\"850\" height=\"400\" fill=\"#f8fafc\" stroke=\"#334155\"/>";
    svg << std::fixed << std::setprecision(2);
    for (int division = 0; division <= 5; ++division) {
        const double gridY = top + division * height / 5.0;
        const double value = high - division * (high-low) / 5.0;
        svg << "<line x1=\"" << left << "\" y1=\"" << gridY << "\" x2=\""
            << left+width << "\" y2=\"" << gridY << "\" stroke=\"#cbd5e1\"/>"
            << "<text x=\"80\" y=\"" << gridY+5 << "\" text-anchor=\"end\" "
            << "font-family=\"Arial\" font-size=\"12\">" << value << "</text>";
    }
    for (int division = 0; division <= 5; ++division) {
        const double gridX = left + division * width / 5.0;
        const int step = samples.size() < 2 ? 0 :
            static_cast<int>(std::lround(division * (samples.size()-1) / 5.0));
        svg << "<line x1=\"" << gridX << "\" y1=\"" << top << "\" x2=\""
            << gridX << "\" y2=\"" << top+height << "\" stroke=\"#cbd5e1\"/>"
            << "<text x=\"" << gridX << "\" y=\"485\" text-anchor=\"middle\" "
            << "font-family=\"Arial\" font-size=\"12\">" << step << "</text>";
    }
    auto line = [&](const char* color, auto value) {
        svg << "<polyline fill=\"none\" stroke=\"" << color << "\" stroke-width=\"3\" points=\"";
        for (std::size_t i=0;i<samples.size();++i) svg << x(i) << ',' << y(value(samples[i])) << ' ';
        svg << "\"/>";
    };
    line("#16a34a", [](const Sample& s){return s.setpoint;});
    line("#2563eb", [](const Sample& s){return s.output;});
    line("#dc2626", [](const Sample& s){return s.control;});
    svg << "<text x=\"620\" y=\"535\" fill=\"#16a34a\" font-family=\"Arial\">Задание w</text>"
        << "<text x=\"745\" y=\"535\" fill=\"#2563eb\" font-family=\"Arial\">Выход y</text>"
        << "<text x=\"850\" y=\"535\" fill=\"#dc2626\" font-family=\"Arial\">Управление u</text>"
        << "<text x=\"515\" y=\"515\" text-anchor=\"middle\" font-family=\"Arial\">Шаг t</text>"
        << "<text x=\"22\" y=\"265\" text-anchor=\"middle\" font-family=\"Arial\" "
        << "transform=\"rotate(-90 22 265)\">Значение</text></svg>";
}

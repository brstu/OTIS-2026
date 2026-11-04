#include "control.hpp"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
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

void saveResults(const std::string& baseName, const std::vector<Sample>& samples) {
    if (samples.empty()) return;
    std::filesystem::create_directories("results");
    std::ofstream csv("results/" + baseName + ".csv");
    csv << "t,w,e,u,y\n";
    for (const auto& s : samples)
        csv << s.step << ',' << s.setpoint << ',' << s.error << ',' << s.control << ',' << s.output << '\n';

    double low = 0.0, high = samples.front().setpoint;
    for (const auto& s : samples) {
        low = std::min({low, s.output, s.control});
        high = std::max({high, s.output, s.control});
    }
    const double pad = std::max(0.1, (high-low)*0.1); low -= pad; high += pad;
    auto x = [&](std::size_t i) { return 70.0 + (samples.size()<2 ? 0.0 : i*790.0/(samples.size()-1)); };
    auto y = [&](double value) { return 60.0 + (high-value)*370.0/(high-low); };
    std::ofstream svg("results/" + baseName + ".svg");
    svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"900\" height=\"500\">"
        << "<rect x=\"0\" y=\"0\" width=\"900\" height=\"500\" fill=\"white\"/><text x=\"450\" y=\"30\" text-anchor=\"middle\" font-family=\"Arial\" font-size=\"20\">" << baseName << "</text>"
        << "<rect x=\"70\" y=\"60\" width=\"790\" height=\"370\" fill=\"#f8fafc\" stroke=\"#334155\"/>";
    auto line = [&](const char* color, auto value) {
        svg << "<polyline fill=\"none\" stroke=\"" << color << "\" stroke-width=\"3\" points=\"";
        for (std::size_t i=0;i<samples.size();++i) svg << x(i) << ',' << y(value(samples[i])) << ' ';
        svg << "\"/>";
    };
    line("#16a34a", [](const Sample& s){return s.setpoint;});
    line("#2563eb", [](const Sample& s){return s.output;});
    line("#dc2626", [](const Sample& s){return s.control;});
    svg << "<text x=\"600\" y=\"465\" fill=\"#16a34a\" font-family=\"Arial\">setpoint</text>"
        << "<text x=\"700\" y=\"465\" fill=\"#2563eb\" font-family=\"Arial\">output</text>"
        << "<text x=\"780\" y=\"465\" fill=\"#dc2626\" font-family=\"Arial\">control</text>"
        << "<text x=\"450\" y=\"455\" text-anchor=\"middle\" font-family=\"Arial\">step t</text></svg>";
}

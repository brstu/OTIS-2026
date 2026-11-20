#include "Simulator.h"

#include <fstream>
#include <iomanip>
#include <ostream>
#include <stdexcept>

Simulator::Simulator(Model& model, const InputSignal& signal, double dt)
    : model_(model), signal_(signal), dt_(dt) {
    if (dt_ <= 0.0) {
        throw std::invalid_argument("Шаг интегрирования dt должен быть > 0");
    }
}

std::vector<Sample> Simulator::run(int steps) {
    if (steps <= 0) {
        throw std::invalid_argument("Количество шагов должно быть > 0");
    }

    model_.reset();

    std::vector<Sample> data;
    data.reserve(static_cast<std::size_t>(steps) + 1);
    data.push_back(Sample{0, signal_.value(0), 0.0});

    for (int tau = 0; tau < steps; ++tau) {
        const double u = signal_.value(tau);
        const double y = model_.step(u, dt_);
        data.push_back(Sample{tau + 1, signal_.value(tau + 1), y});
    }
    return data;
}

void Simulator::printTable(std::ostream& os, const std::vector<Sample>& data) {
    os << std::fixed << std::setprecision(4);
    os << "+------+--------------+--------------+\n";
    os << "| tau  |      u(tau)  |      y(tau)  |\n";
    os << "+------+--------------+--------------+\n";
    for (const Sample& s : data) {
        os << "| " << std::setw(4) << s.tau << " | " << std::setw(12) << s.u
           << " | " << std::setw(12) << s.y << " |\n";
    }
    os << "+------+--------------+--------------+\n";
}

void Simulator::writeTable(const std::string& path,
                           const std::vector<Sample>& data) {
    std::ofstream out(path);
    if (!out) {
        throw SimulationError("Не удалось открыть файл для записи: " + path);
    }
    out << "tau\tu\ty\n";
    out << std::fixed << std::setprecision(6);
    for (const Sample& s : data) {
        out << s.tau << '\t' << s.u << '\t' << s.y << '\n';
    }
}

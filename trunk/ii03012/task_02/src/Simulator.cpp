#include "Simulator.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <limits>

std::vector<SimulationRow> Simulator::run(Model& model,
                                          const InputSignal& signal,
                                          int n,
                                          double y0)
{
    std::vector<SimulationRow> rows;
    if (n < 0) throw std::invalid_argument("Negative step count");
    rows.reserve(static_cast<size_t>(n) + 1);

    model.reset(y0);
    rows.push_back({0, signal.value(0), model.currentY()});

    for (int tau = 0; tau < n; ++tau)
    {
        const double y = model.step(rows.back().u);

        if (!std::isfinite(y) || std::fabs(y) > kDivergenceLimit)
        {
            std::cout << "ВНИМАНИЕ: процесс расходится на шаге tau = "
                      << tau + 1 << " (|y| > " << kDivergenceLimit
                      << "), моделирование остановлено.\n";
            break;
        }

        rows.push_back({tau + 1, signal.value(tau + 1), y});
    }

    return rows;
}

void Simulator::printTable(const std::vector<SimulationRow>& rows)
{
    const auto flags = std::cout.flags();
    const auto precision = std::cout.precision();
    std::cout << "   tau             u             y\n"
              << std::string(34, '-') << '\n' << std::fixed << std::setprecision(6);
    for (const auto& [time, input, state] : rows)
        std::cout << std::setw(6) << time << std::setw(14) << input
                  << std::setw(14) << state << '\n';
    std::cout.flags(flags);
    std::cout.precision(precision);
}

bool Simulator::saveCsv(const std::vector<SimulationRow>& rows,
                        const std::string& fileName)
{
    std::ofstream file(fileName);
    if (!file)
    {
        std::cerr << "Не удалось открыть файл " << fileName << '\n';
        return false;
    }

    file << "tau,u,y\n";
    file << std::setprecision(std::numeric_limits<double>::max_digits10);
    for (const SimulationRow& row : rows)
        file << row.tau << ',' << row.u << ',' << row.y << '\n';

    file.close();
    return !file.fail();
}

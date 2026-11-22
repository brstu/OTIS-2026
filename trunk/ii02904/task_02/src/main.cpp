#include "input_signal.h"
#include "models.h"
#include "stability_analyzer.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

namespace {
constexpr double MAX_OUTPUT = 1e9;
constexpr const char* RESULT_FILE = "results.csv";

double readNumber(const std::string& prompt)
{
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cout << "\nВвод закончился, выход.\n";
            std::exit(1);
        }
        std::replace(line.begin(), line.end(), ',', '.');
        try {
            const double value = std::stod(line);
            if (std::isfinite(value)) {
                return value;
            }
        } catch (const std::invalid_argument&) {
        } catch (const std::out_of_range&) {
        }
        std::cout << "Нужно ввести конечное число, попробуйте ещё раз.\n";
    }
}

int readInt(const std::string& prompt, int minimum, int maximum)
{
    while (true) {
        const double value = readNumber(prompt);
        if (value >= minimum && value <= maximum && std::floor(value) == value) {
            return static_cast<int>(value);
        }
        std::cout << "Число должно быть целым от " << minimum << " до " << maximum << ".\n";
    }
}

std::unique_ptr<Model> createModel(int choice, double initialOutput)
{
    if (choice == 1) {
        const double a = readNumber("a = ");
        const double b = readNumber("b = ");
        if (!StabilityAnalyzer::isModel14Stable(a)) {
            std::cout << "\nПРЕДУПРЕЖДЕНИЕ: |a| >= 1, модель 1.4 неустойчива.\n";
        }
        return std::make_unique<Model14>(a, b, initialOutput);
    }

    if (choice == 2) {
        const double a1 = readNumber("a1 = ");
        const double a2 = readNumber("a2 = ");
        const double b = readNumber("b = ");
        return std::make_unique<Model26>(a1, a2, b, initialOutput);
    }

    const double a1 = readNumber("a1 = ");
    const double a2 = readNumber("a2 = ");
    const double b = readNumber("b = ");
    const double timeStep = readNumber("Шаг dt = ");
    return std::make_unique<Model38>(a1, a2, b, timeStep, initialOutput);
}

InputSignal readSignal(double& stepValue)
{
    std::cout << "\nВыберите входной сигнал:\n"
              << "  1 - ступенчатый (u = const)\n"
              << "  2 - импульсный (u(0) = 1, дальше 0)\n"
              << "  3 - гармонический (u = sin(t))\n";
    const int choice = readInt("Ваш выбор: ", 1, 3);
    if (choice == 1) {
        stepValue = readNumber("Значение u = ");
        return InputSignal::Step;
    }
    return choice == 2 ? InputSignal::Impulse : InputSignal::Harmonic;
}

const char* signalName(InputSignal signal)
{
    switch (signal) {
    case InputSignal::Step:
        return "ступенчатый сигнал";
    case InputSignal::Impulse:
        return "импульсный сигнал";
    case InputSignal::Harmonic:
        return "гармонический сигнал";
    }
    throw std::invalid_argument("Unknown input signal.");
}
}  // namespace

int main()
{
    std::cout << "Лабораторная работа 2, вариант 4\n\n"
              << "Выберите модель:\n"
              << "  1 - Модель 1.4 (линейная)\n"
              << "  2 - Модель 2.6 (нелинейная)\n"
              << "  3 - Модель 3.8 (дифференциальное уравнение)\n";
    const int modelChoice = readInt("Ваш выбор: ", 1, 3);
    const double initialOutput = readNumber("Начальное значение y(0) = ");
    std::unique_ptr<Model> model = createModel(modelChoice, initialOutput);

    double stepValue = 1.0;
    const InputSignal signal = readSignal(stepValue);
    const int steps = readInt("\nЧисло шагов n (от 1 до 10000): ", 1, 10000);

    std::ofstream file(RESULT_FILE);
    if (!file) {
        std::cerr << "Не удалось создать файл " << RESULT_FILE << "\n";
        return 1;
    }
    file << model->name() << ", " << signalName(signal) << "\ntau,u,y\n"
         << std::setprecision(10);

    std::cout << "\n+--------+--------------------+--------------------+\n"
              << "|    tau |                  u |                  y |\n"
              << "+--------+--------------------+--------------------+\n"
              << std::fixed << std::setprecision(6);

    double output = initialOutput;
    bool diverged = false;
    int divergedStep = 0;
    for (int tau = 0; tau <= steps; ++tau) {
        const double input = inputAt(signal, tau, stepValue);
        std::cout << "| " << std::setw(6) << tau << " | " << std::setw(18) << input
                  << " | " << std::setw(18) << output << " |\n";
        file << tau << ',' << input << ',' << output << '\n';

        if (tau < steps) {
            output = model->nextStep(input);
            if (!std::isfinite(output) || std::abs(output) > MAX_OUTPUT) {
                diverged = true;
                divergedStep = tau + 1;
                break;
            }
        }
    }
    std::cout << "+--------+--------------------+--------------------+\n";

    if (diverged) {
        std::cout << "\nПРЕДУПРЕЖДЕНИЕ: процесс расходится (шаг " << divergedStep
                  << "), расчёт остановлен.\n";
    }
    std::cout << "\nРезультаты сохранены в файл " << RESULT_FILE << "\n";
    return 0;
}

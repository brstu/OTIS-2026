// ЛР №2: расширенная версия программы ЛР №1 - анализ устойчивости модели 1.1
// (предупреждение перед симуляцией), без привязки к Windows и Gnuplot.
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

#include "InputSignal.h"
#include "Simulator.h"
#include "StabilityAnalyzer.h"
#include "model11.h"
#include "model28.h"
#include "model36.h"

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    std::cout << "ОТИС-2026 | Лабораторная работа №2 | Вариант 26\n\n";
    std::cout << "Модель: 1 - Model 1.1 (линейная), 2 - Model 2.8 (нелинейная), 3 - Model 3.6 (диф. ур.)\nВыбор: ";
    int modelChoice = 0;
    std::cin >> modelChoice;

    std::unique_ptr<Model> model;
    std::string prefix;
    if (modelChoice == 1) {
        double a;
        double b;
        std::cout << "a = "; std::cin >> a;
        std::cout << "b = "; std::cin >> b;
        // Анализ устойчивости: предупреждение выводится ДО начала симуляции.
        StabilityAnalyzer::warnIfUnstable(a, std::cout);
        model = std::make_unique<Model11>(a, b);
        prefix = "Model11";
    } else if (modelChoice == 2) {
        double a;
        double b;
        double c;
        std::cout << "a = "; std::cin >> a;
        std::cout << "b = "; std::cin >> b;
        std::cout << "c = "; std::cin >> c;
        model = std::make_unique<Model28>(a, b, c);
        prefix = "Model28";
    } else if (modelChoice == 3) {
        double a;
        double b;
        std::cout << "a = "; std::cin >> a;
        std::cout << "b = "; std::cin >> b;
        model = std::make_unique<Model36>(a, b);
        prefix = "Model36";
    } else {
        std::cout << "Неверный выбор!\n";
        return 1;
    }

    std::cout << "Воздействие: 1 - ступенчатое, 2 - импульсное, 3 - гармоническое\nВыбор: ";
    int signalChoice = 0;
    std::cin >> signalChoice;

    double amplitude = 1.0;
    int n = 10;
    std::cout << "Амплитуда A = "; std::cin >> amplitude;
    std::cout << "Количество шагов n = "; std::cin >> n;

    const double dt = 0.1;
    std::unique_ptr<ISignal> signal;
    std::string signalName;
    if (signalChoice == 1)      { signal = std::make_unique<StepSignal>(amplitude);          signalName = "Step"; }
    else if (signalChoice == 2) { signal = std::make_unique<PulseSignal>(amplitude);         signalName = "Pulse"; }
    else                        { signal = std::make_unique<HarmonicSignal>(amplitude, dt);  signalName = "Harmonic"; }

    const auto samples = simulate(*model, *signal, n, dt);

    const std::string filename = prefix + "_" + signalName + ".csv";
    std::ofstream csv(filename);
    csv << "tau,u,y\n";

    std::cout << "\n" << std::setw(8) << "tau" << std::setw(15) << "u" << std::setw(15) << "y" << "\n";
    for (const auto& s : samples) {
        std::cout << std::setw(8) << s.tau << std::setw(15) << std::fixed << std::setprecision(6) << s.u
                  << std::setw(15) << s.y << "\n";
        csv << s.tau << "," << s.u << "," << s.y << "\n";
    }
    std::cout << "\nРезультаты сохранены в " << filename << "\n";
    return 0;
}

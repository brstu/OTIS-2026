#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "Model_new.h"
#include "InputSignals.h"

namespace {
    constexpr double DEFAULT_DT = 0.01;
    constexpr int    DEFAULT_N  = 20;
    constexpr int    PRECISION  = 5;
}

// ---------------------------------------------------------------------------
//  readDouble — ввод числа с плавающей точкой
// ---------------------------------------------------------------------------
static double readDouble(const std::string& prompt, double def)
{
    std::cout << prompt << " [по умолчанию " << def << "]: ";
    std::string line;
    std::getline(std::cin, line);
    if (line.empty()) {
        return def;
    }
    try {
        return std::stod(line);
    } catch (const std::invalid_argument&) {
        std::cout << "  Не число, использую значение по умолчанию.\n";
        return def;
    } catch (const std::out_of_range&) {
        std::cout << "  Слишком большое число, использую значение по умолчанию.\n";
        return def;
    }
}

// ---------------------------------------------------------------------------
//  readInt — ввод целого числа
// ---------------------------------------------------------------------------
static int readInt(const std::string& prompt, int def)
{
    std::cout << prompt << " [по умолчанию " << def << "]: ";
    std::string line;
    std::getline(std::cin, line);
    if (line.empty()) {
        return def;
    }
    try {
        return std::stoi(line);
    } catch (const std::invalid_argument&) {
        std::cout << "  Не число, использую значение по умолчанию.\n";
        return def;
    } catch (const std::out_of_range&) {
        std::cout << "  Слишком большое число, использую значение по умолчанию.\n";
        return def;
    }
}

// ---------------------------------------------------------------------------
//  readChoice — ввод выбора из диапазона [lo, hi]
// ---------------------------------------------------------------------------
static int readChoice(const std::string& prompt, int lo, int hi)
{
    while (true) {
        std::cout << prompt;
        std::string line;
        std::getline(std::cin, line);
        try {
            const int v = std::stoi(line);
            if (v >= lo && v <= hi) {
                return v;
            }
        } catch (const std::invalid_argument&) {
            // fall through
        } catch (const std::out_of_range&) {
            // fall through
        }
        std::cout << "  Введите число от " << lo << " до " << hi << ".\n";
    }
}

// ---------------------------------------------------------------------------
//  main
// ---------------------------------------------------------------------------
int main()
{
    std::cout << "===========================================\n";
    std::cout << " Лабораторная работа №1 (ОТИС) — ii02919\n";
    std::cout << " Моделирование управляемого объекта\n";
    std::cout << " Вариант 17\n";
    std::cout << "===========================================\n\n";

    std::cout << "Выберите модель:\n";
    std::cout << "  1) Model 1.7 — Multi-Step Control History\n";
    std::cout << "  2) Model 2.1 — Quadratic Feedback\n";
    std::cout << "  3) Model 3.5 — Harmonic Driving Force\n";
    const int modelChoice = readChoice("Ваш выбор (1-3): ", 1, 3);
    std::cout << "\n";

    double p1 = 0.0;
    double p2 = 0.0;
    double p3 = 0.0;
    double p4 = 0.0;

    if (modelChoice == 1) {
        p1 = readDouble("a  (коэф. затухания)", 0.9);
        p2 = readDouble("b1 (коэф. текущего входа)", 1.0);
        p3 = readDouble("b2 (коэф. входа с задержкой 1)", 0.0);
        p4 = readDouble("b3 (коэф. входа с задержкой 2)", 0.0);
    } else if (modelChoice == 2) {
        p1 = readDouble("a (линейная обратная связь)", 0.5);
        p2 = readDouble("b (квадратичная обратная связь)", 0.1);
        p3 = readDouble("c (коэф. текущего входа)", 1.0);
        p4 = readDouble("d (амплитуда гармоники)", 0.5);
    } else {
        p1 = readDouble("b  (амплитуда)", 1.0);
        p2 = readDouble("dt (шаг интегрирования)", DEFAULT_DT);
    }

    std::unique_ptr<DynamicProcess> model =
        ProcessFactory::create(modelChoice, p1, p2, p3, p4);

    if (!model) {
        std::cout << "Ошибка: не удалось создать модель.\n";
        return 1;
    }

    std::cout << "\nМодель: " << model->title() << "\n";

    if (!model->isStableSystem()) {
        const std::string warn = model->alertText();
        if (!warn.empty()) {
            std::cout << "\n!!! ВНИМАНИЕ !!!\n" << warn << "\n";
        }
        std::cout << "Продолжить симуляцию? (y/n): ";
        std::string ans;
        std::getline(std::cin, ans);
        if (ans != "y" && ans != "Y") {
            std::cout << "Отменено пользователем.\n";
            return 0;
        }
    }

    std::cout << "\nВыберите входной сигнал:\n";
    std::cout << "  1) Ступенчатое\n";
    std::cout << "  2) Импульсное\n";
    std::cout << "  3) Гармоническое\n";
    const int sigChoice = readChoice("Ваш выбор (1-3): ", 1, 3);

    std::unique_ptr<InputSignal> signal;
    switch (sigChoice) {
        case 1:
            signal = std::make_unique<UnitStep>(1.0);
            break;
        case 2:
            signal = std::make_unique<SingleImpulse>();
            break;
        case 3:
            signal = std::make_unique<Sinusoid>();
            break;
        default:
            signal = std::make_unique<UnitStep>(1.0);
            break;
    }
    std::cout << "Сигнал: " << signal->label() << "\n";

    const int n = readInt("\nВведите количество шагов n", DEFAULT_N);
    if (n <= 0) {
        std::cout << "n должно быть > 0\n";
        return 1;
    }

    std::cout << "\n===========================================\n";
    std::cout << std::setw(6)  << "tau"
              << std::setw(14) << "u_tau"
              << std::setw(14) << "y_tau" << "\n";
    std::cout << "-------------------------------------------\n";

    std::ofstream csv("result.csv");
    csv << "tau,u_tau,y_tau\n";

    model->reset();
    for (int tau = 1; tau <= n; ++tau) {
        const double u = signal->value(tau);
        const double y = model->advance(u);

        std::cout << std::setw(6)  << tau
                  << std::setw(14) << std::fixed << std::setprecision(PRECISION) << u
                  << std::setw(14) << std::fixed << std::setprecision(PRECISION) << y
                  << "\n";

        csv << tau << "," << u << "," << y << "\n";
    }

    csv.close();
    std::cout << "-------------------------------------------\n";
    std::cout << "Результаты сохранены в result.csv\n";
    std::cout << "Готово.\n";

    return 0;
}

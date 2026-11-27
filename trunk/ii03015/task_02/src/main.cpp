#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif

#include "SignalSource.h"
#include "Model.h"
#include "Model1_6.h"
#include "Model2_8.h"
#include "Model3_10.h"
#include "Simulation.h"
#include "StabilityAnalyzer.h"

namespace
{
int readInt(const std::string& prompt, int minValue)
{
    int value;
    while (true)
    {
        std::cout << prompt;
        if (std::cin >> value && value >= minValue)
            return value;
        std::cout << "Ошибка ввода. Повторите.\n";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }
}

double readDouble(const std::string& prompt)
{
    double value;
    while (true)
    {
        std::cout << prompt;
        if (std::cin >> value && std::isfinite(value))
            return value;
        std::cout << "Ошибка ввода. Повторите.\n";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }
}

InputKind readInputKind()
{
    while (true)
    {
        if (const int choice = readInt("\nТип входного воздействия:\n1 - ступенчатое\n2 - импульсное\n3 - гармоническое\nВаш выбор: ", 1); choice <= 3)
        {
            return static_cast<InputKind>(choice);
        }

        std::cout << "Нет такого варианта. Повторите.\n";
    }
}

struct ModelSetup
{
    std::unique_ptr<Model> model;
    bool hasStability = false;
    StabilityReport stability;
    double dt = 1.0;
};

ModelSetup createModel()
{
    while (true)
    {
        const int choice = readInt(
            "\nВыберите модель:\n"
            "1 - Model 1.6 (линейная, с анализом устойчивости)\n"
            "2 - Model 2.8 (нелинейная)\n"
            "3 - Model 3.10 (метод Эйлера)\n"
            "Ваш выбор: ", 1);

        switch (choice)
        {
        case 1:
        {
            const double a1 = readDouble("a1 = ");
            const double a2 = readDouble("a2 = ");
            const double a3 = readDouble("a3 = ");
            const double b  = readDouble("b  = ");

            ModelSetup setup;
            setup.model.reset(new Model1_6(a1, a2, a3, b));
            setup.stability = StabilityAnalyzer::analyzeModel1_6(a1, a2, a3);
            setup.hasStability = true;
            return setup;
        }
        case 2:
        {
            const double a = readDouble("a = ");
            const double b = readDouble("b = ");
            const double c = readDouble("c = ");

            ModelSetup setup;
            setup.model.reset(new Model2_8(a, b, c));
            return setup;
        }
        case 3:
        {
            const double a = readDouble("a = ");
            const double b = readDouble("b = ");
            const double dt = readDouble("Шаг интегрирования dt = ");

            ModelSetup setup;
            setup.model.reset(new Model3_10(a, b));
            setup.dt = dt;
            return setup;
        }
        default:
            std::cout << "Нет такой модели. Повторите выбор.\n";
        }
    }
}

void saveCsv(const std::vector<SimStep>& rows)
{
    std::ofstream file("results.csv");
    if (!file)
    {
        std::cerr << "Не удалось открыть results.csv для записи.\n";
        return;
    }
    file << "tau,u,y\n";
    for (const SimStep& row : rows)
        file << row.tau << ',' << row.u << ',' << row.y << '\n';
    std::cout << "\nРезультаты сохранены в results.csv\n";
}

void printTable(const std::vector<SimStep>& rows)
{
    std::cout << "\n" << std::setw(8) << "tau"
              << "\t" << "u"
              << "\t" << "y" << '\n';
    std::cout << std::string(38, '-') << '\n';
    for (const SimStep& row : rows)
    {
        std::cout << "\t" << row.tau
                  << "\t" << row.u
                  << "\t" << row.y << '\n';
    }
}
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif

    std::cout << "============================================================================\n";
    std::cout << "  ОТИС-2026 | Лабораторная работа №2\n";
    std::cout << "  Дисциплина: Общая теория интеллектуальных систем\n";
    std::cout << "  Тема: \"Модульное тестирование и анализ устойчивости моделей\"\n";
    std::cout << "  Вариант 6 | Студент: Низамутдинов М.А. | Группа: ИИ-30\n";
    std::cout << "============================================================================\n";

    const ModelSetup setup = createModel();

    const InputKind signal = readInputKind();
    const double amplitude = readDouble("Амплитуда A = ");
    const int n = readInt("Количество шагов n = ", 1);
    const double y0 = readDouble("Начальное значение y0 = ");

    if (setup.hasStability)
    {
        std::cout << "\n----------------------------------------------------------------------------\n";
        std::cout << "  АНАЛИЗ УСТОЙЧИВОСТИ (критерий Шура, Z-плоскость)\n";
        std::cout << "----------------------------------------------------------------------------\n";
        std::cout << StabilityAnalyzer::describe(setup.stability);
        StabilityAnalyzer::printWarningIfNeeded(setup.stability, std::cout);
        std::cout << "----------------------------------------------------------------------------\n";
    }

    const SimulationConfig config{n, y0, setup.dt};
    const SignalInputSource input(signal, amplitude);
    const auto rows = simulate(*setup.model, config, input);

    std::cout << "\n----------------------------------------------------------------------------\n";
    std::cout << "  РЕЗУЛЬТАТЫ СИМУЛЯЦИИ\n";
    std::cout << "----------------------------------------------------------------------------\n";
    std::cout << "Модель:       " << setup.model->label() << '\n';
    std::cout << "Сигнал:       ";
    switch (signal)
    {
    case InputKind::Step:      std::cout << "ступенька (u = A)"; break;
    case InputKind::Impulse:   std::cout << "импульс (u(0)=A, u>0=0)"; break;
    case InputKind::Harmonic:  std::cout << "гармоника (u = A*sin(t))"; break;
    }
    std::cout << "\nАмплитуда:    " << amplitude << '\n';
    std::cout << "Шагов:        " << n << '\n';
    std::cout << "Начальное y0: " << y0 << '\n';
    std::cout << "----------------------------------------------------------------------------\n";

    printTable(rows);
    saveCsv(rows);

    std::cout << "\n----------------------------------------------------------------------------\n";
    std::cout << "  ПОСТРОЕНИЕ ГРАФИКА\n";
    std::cout << "----------------------------------------------------------------------------\n";
    std::cout << "Запуск: python plot.py\n";
    if (int plot_result = system("python plot.py >nul 2>&1"); plot_result == 0)
        std::cout << "График сохранён в файл: result.png\n";
    else
        std::cout << "Не удалось построить график (убедитесь, что Python и matplotlib установлены).\n";
    std::cout << "----------------------------------------------------------------------------\n";

    std::cout << "\n============================================================================\n";
    std::cout << "  Работа завершена. Нажмите Enter для выхода...\n";
    std::cout << "============================================================================\n";
    std::cin.ignore();
    std::cin.get();

    return 0;
}

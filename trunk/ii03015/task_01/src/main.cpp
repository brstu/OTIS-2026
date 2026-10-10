#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif

#include "InputSignal.h"
#include "Model.h"
#include "Model1_6.h"
#include "Model2_8.h"
#include "Model3_10.h"

struct Row
{
    int tau;
    double u;
    double y;
};

static int readInt(const std::string& prompt, int minValue)
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

static double readDouble(const std::string& prompt)
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

static SignalType readSignalType()
{
    while (true)
    {
        const int choice = readInt(
            "\nТип входного воздействия:\n"
            "1 - ступенчатое\n"
            "2 - импульсное\n"
            "3 - гармоническое\n"
            "Ваш выбор: ", 1);

        if (choice <= 3)
            return static_cast<SignalType>(choice);

        std::cout << "Нет такого варианта. Повторите.\n";
    }
}

static std::unique_ptr<Model> createModel()
{
    while (true)
    {
        const int choice = readInt(
            "\nВыберите модель:\n"
            "1 - Model 1.6\n"
            "2 - Model 2.8\n"
            "3 - Model 3.10\n"
            "Ваш выбор: ", 1);

        switch (choice)
        {
        case 1:
        {
            const double a1 = readDouble("a1 = ");
            const double a2 = readDouble("a2 = ");
            const double a3 = readDouble("a3 = ");
            const double b  = readDouble("b  = ");
            return std::make_unique<Model1_6>(a1, a2, a3, b);
        }
        case 2:
        {
            const double a = readDouble("a = ");
            const double b = readDouble("b = ");
            const double c = readDouble("c = ");
            return std::make_unique<Model2_8>(a, b, c);
        }
        case 3:
        {
            const double a = readDouble("a = ");
            const double b = readDouble("b = ");
            return std::make_unique<Model3_10>(a, b);
        }
        default:
            std::cout << "Нет такой модели. Повторите выбор.\n";
        }
    }
}

static void saveCsv(const std::vector<Row>& rows)
{
    std::ofstream file("results.csv");
    if (!file)
    {
        std::cerr << "Не удалось открыть results.csv для записи.\n";
        return;
    }
    file << "tau,u,y\n";
    for (const Row& row : rows)
        file << row.tau << ',' << row.u << ',' << row.y << '\n';
    std::cout << "\nРезультаты сохранены в results.csv\n";
}

static void printTable(const std::vector<Row>& rows)
{
    std::cout << "\n" << std::setw(8) << "tau"
              << std::setw(15) << "u"
              << std::setw(15) << "y" << '\n';
    std::cout << std::string(38, '-') << '\n';
    for (const Row& row : rows)
    {
        std::cout << std::setw(8) << row.tau
                  << std::setw(15) << std::fixed << std::setprecision(6) << row.u
                  << std::setw(15) << row.y << '\n';
    }
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif
    std::cout << "============================================\n";
    std::cout << " OTIS-2026 | Лабораторная работа №1\n";
    std::cout << " Вариант 6, Низамутдинов М.А.\n";
    std::cout << "============================================\n";

    std::unique_ptr<Model> model = createModel();
    const SignalType signal = readSignalType();
    const double amplitude = readDouble("Амплитуда A = ");
    const int n = readInt("Количество шагов n = ", 1);
    const double y0 = readDouble("Начальное значение y0 = ");

    double dt = 1.0;
    if (std::string(model->getName()).find("Model 3.10") != std::string::npos)
        dt = readDouble("Шаг интегрирования dt = ");

    std::vector<Row> rows;
    rows.reserve(n + 1);
    double y = y0;
    double yPrev = y0;
    double yPrev2 = y0;

    rows.push_back({0, getInput(signal, 0, amplitude), y});

    for (int tau = 0; tau < n; ++tau)
    {
        const double u = getInput(signal, tau, amplitude);
        const double nextY = model->calculateNext(y, yPrev, yPrev2, u, dt);
        yPrev2 = yPrev;
        yPrev = y;
        y = nextY;
        rows.push_back({tau + 1, getInput(signal, tau + 1, amplitude), y});
    }

    std::cout << "\nМодель: " << model->getName() << '\n';
    printTable(rows);
    saveCsv(rows);

    system("python plot.py");

    std::cout << "\nНажмите Enter для выхода...";
    std::cin.ignore();
    std::cin.get();

    return 0;
}

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

#include "InputSignal.h"
#include "Model.h"
#include "Model1_9.h"
#include "Model2_6.h"
#include "Model3_9.h"

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

        if (choice >= 1 && choice <= 3)
            return static_cast<SignalType>(choice);

        std::cout << "Выберите значение от 1 до 3.\n";
    }
}

static std::unique_ptr<Model> createModel()
{
    while (true)
    {
        const int choice = readInt(
            "\nВыберите модель:\n"
            "1 - Model 1.9\n"
            "2 - Model 2.6\n"
            "3 - Model 3.9\n"
            "Ваш выбор: ", 1);

        switch (choice)
        {
        case 1:
        {
            std::cout << "\nModel 1.9:\n";
            std::cout << "y(t+1) = a1*y(t) + a2*y(t-1) + a3*y(t-2) + b1*u(t)\n";

            const double a1 = readDouble("a1 = ");
            const double a2 = readDouble("a2 = ");
            const double a3 = readDouble("a3 = ");
            const double b1 = readDouble("b1 = ");

            return std::make_unique<Model1_9>(a1, a2, a3, b1);
        }

        case 2:
        {
            std::cout << "\nModel 2.6:\n";
            std::cout << "y(t+1) = a1*y(t)^3 - a2*y(t-1) + b*u(t)^2\n";

            const double a1 = readDouble("a1 = ");
            const double a2 = readDouble("a2 = ");
            const double b = readDouble("b = ");

            return std::make_unique<Model2_6>(a1, a2, b);
        }

        case 3:
        {
            std::cout << "\nModel 3.9:\n";
            std::cout << "dy/dt = b*tanh(u)\n";

            const double b = readDouble("b = ");

            return std::make_unique<Model3_9>(b);
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
    std::cout << "\n"
              << std::setw(8) << "tau"
              << std::setw(15) << "u"
              << std::setw(15) << "y"
              << '\n';

    std::cout << std::string(38, '-') << '\n';

    for (const Row& row : rows)
    {
        std::cout << std::setw(8) << row.tau
                  << std::setw(15) << std::fixed << std::setprecision(6) << row.u
                  << std::setw(15) << row.y
                  << '\n';
    }
}

static void runPlot()
{
    std::cout << "\nЗапуск plot.py...\n";
    const int result = std::system("python plot.py");

    if (result != 0)
        std::cout << "Не удалось автоматически запустить Python. "
                     "Запустите plot.py вручную.\n";
}

int main()
{
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif

    std::cout << "============================================\n";
    std::cout << " OTIS-2026 | Лабораторная работа №1\n";
    std::cout << " Вариант 30\n";
    std::cout << "============================================\n";

    std::unique_ptr<Model> model = createModel();

    const SignalType signal = readSignalType();
    const double amplitude = readDouble("Амплитуда A = ");
    const int n = readInt("Количество шагов n = ", 1);
    const double y0 = readDouble("Начальное значение y0 = ");

    double dt = 1.0;

    if (std::string(model->getName()).find("Model 3.9") != std::string::npos)
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

        const double nextY = model->calculateNext(
            y, yPrev, yPrev2, u, dt
        );

        yPrev2 = yPrev;
        yPrev = y;
        y = nextY;

        rows.push_back({
            tau + 1,
            getInput(signal, tau + 1, amplitude),
            y
        });
    }

    std::cout << "\nМодель: " << model->getName() << '\n';

    printTable(rows);
    saveCsv(rows);
    runPlot();

#ifdef _WIN32
    system("pause");
#endif

    return 0;
}

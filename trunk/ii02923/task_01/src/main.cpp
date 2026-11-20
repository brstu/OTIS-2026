/**
 * @file main.cpp
 * @brief Лабораторная работа №1, вариант 11 (модели 1.1, 2.5, 3.9).
 *
 * Программа моделирует дискретный объект управления при одном из трёх
 * входных воздействий, выводит таблицу переходного процесса в консоль
 * и (опционально) в текстовый файл для построения графиков.
 *
 * Примеры запуска:
 *   otis_lab1_ii02911.exe
 *   otis_lab1_ii02911.exe --model 2.5 --signal harmonic --steps 40
 *   otis_lab1_ii02911.exe --model 3.9 --signal step --steps 100 --dt 0.1 --out result.txt
 */

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "InputSignal.h"
#include "Model1_1.h"
#include "Model2_5.h"
#include "Model3_9.h"
#include "Simulator.h"
#include "StabilityAnalyzer.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace {

/// Параметры командной строки.
struct Options {
    std::string model = "1.1";
    SignalType signal = SignalType::Step;
    int steps = 30;
    double a = 0.8;
    double b = 1.0;
    double dt = 0.1;
    double amplitude = 1.0;
    double omega = 1.0;
    std::string outPath;
};

void printUsage() {
    std::cout
        << "Использование: otis_lab1_ii02911 [параметры]\n\n"
        << "  --model <1.1|2.5|3.9>   модель: 1.1 (по умолчанию), 2.5, 3.9\n"
        << "  --signal <step|impulse|harmonic>  входное воздействие\n"
        << "  --steps <N>             количество шагов симуляции (по умолчанию 30)\n"
        << "  --a <число>             коэффициент a (по умолчанию 0.8)\n"
        << "  --b <число>             коэффициент b (по умолчанию 1.0)\n"
        << "  --dt <число>            шаг интегрирования (по умолчанию 0.1)\n"
        << "  --amplitude <число>     амплитуда входного сигнала (по умолчанию 1.0)\n"
        << "  --omega <число>         частота гармонического сигнала (по умолчанию 1.0)\n"
        << "  --out <файл.txt>        сохранить таблицу в текстовый файл\n"
        << "  --help                  показать эту справку\n";
}

double toDouble(const std::string& text, const std::string& option) {
    try {
        std::size_t pos = 0;
        const double value = std::stod(text, &pos);
        if (pos != text.size()) {
            throw std::invalid_argument("tail");
        }
        return value;
    } catch (const std::exception&) {
        throw std::invalid_argument("Некорректное число для " + option + ": " + text);
    }
}

int toInt(const std::string& text, const std::string& option) {
    try {
        std::size_t pos = 0;
        const int value = std::stoi(text, &pos);
        if (pos != text.size()) {
            throw std::invalid_argument("tail");
        }
        return value;
    } catch (const std::exception&) {
        throw std::invalid_argument("Некорректное целое для " + option + ": " + text);
    }
}

std::string nextValue(int& index, int argc, char** argv,
                      const std::string& option) {
    if (index + 1 >= argc) {
        throw std::invalid_argument("Для " + option + " не указано значение");
    }
    ++index;
    return argv[index];
}

Options parseOptions(int argc, char** argv) {
    Options opt;
    int i = 1;
    while (i < argc) {
        if (const std::string arg = argv[i]; arg == "--help" || arg == "-h") {
            printUsage();
            std::exit(0);
        } else if (arg == "--model") {
            opt.model = nextValue(i, argc, argv, arg);
        } else if (arg == "--signal") {
            opt.signal = parseSignalType(nextValue(i, argc, argv, arg));
        } else if (arg == "--steps") {
            opt.steps = toInt(nextValue(i, argc, argv, arg), arg);
        } else if (arg == "--a") {
            opt.a = toDouble(nextValue(i, argc, argv, arg), arg);
        } else if (arg == "--b") {
            opt.b = toDouble(nextValue(i, argc, argv, arg), arg);
        } else if (arg == "--dt") {
            opt.dt = toDouble(nextValue(i, argc, argv, arg), arg);
        } else if (arg == "--amplitude") {
            opt.amplitude = toDouble(nextValue(i, argc, argv, arg), arg);
        } else if (arg == "--omega") {
            opt.omega = toDouble(nextValue(i, argc, argv, arg), arg);
        } else if (arg == "--out") {
            opt.outPath = nextValue(i, argc, argv, arg);
        } else {
            throw std::invalid_argument("Неизвестный параметр: " + arg);
        }

        ++i;
    }
    return opt;
}

std::unique_ptr<Model> makeModel(const std::string& id, double a, double b) {
    if (id == "1.1") {
        return std::make_unique<Model1_1>(a, b);
    }
    if (id == "2.5") {
        return std::make_unique<Model2_5>(a, b);
    }
    if (id == "3.9") {
        return std::make_unique<Model3_9>(b);
    }
    throw std::invalid_argument("Неизвестная модель: " + id +
                                " (доступны 1.1, 2.5, 3.9)");
}

void printHeader(const Options& opt, const Model& model,
                 const InputSignal& signal) {
    const std::string line(78, '=');
    std::cout << line << "\n";
    std::cout << " Лабораторная работа №1. Вариант 11\n";
    std::cout << " Студент: Савко Никита, группа ИИ-29\n";
    std::cout << " Модель:  " << model.name() << "\n";
    std::cout << " Уравнение: " << model.equation() << "\n";
    std::cout << " Вход:    " << signal.formula() << "\n";
    std::cout << " Параметры: a = " << opt.a << ", b = " << opt.b
              << ", dt = " << opt.dt << "\n";
    std::cout << " Шагов симуляции: " << opt.steps << "\n";
    std::cout << line << "\n";
}

void printStability(const Model& model) {
    if (model.name() == "Model 3.9") {
        std::cout << "Устойчивость: не применимо (dy/dt не зависит от y).\n\n";
        return;
    }

    std::cout << "Устойчивость: " << StabilityAnalyzer::verdict(model.characteristicRoot())
              << ".\n";
    if (!model.isStable()) {
        std::cout << "\n"
                  << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                  << "!!  ВНИМАНИЕ: ПРОЦЕСС РАСХОДИТСЯ!\n"
                  << "!!  Корень |z| = |a| >= 1 лежит вне единичной окружности.\n"
                  << "!!  Симуляция будет неустойчивой: уменьшите |a| < 1.\n"
                  << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n";
    } else {
        std::cout << "\n";
    }
}

void printSummary(const std::vector<Sample>& data) {
    double minY = data.front().y;
    double maxY = data.front().y;
    for (const Sample& s : data) {
        if (s.y < minY) minY = s.y;
        if (s.y > maxY) maxY = s.y;
    }
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Итог: y_min = " << minY << ", y_max = " << maxY
              << ", y(конечное) = " << data.back().y << "\n";
}

}  // namespace

int main(int argc, char** argv) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    try {
        const Options opt = parseOptions(argc, argv);
        std::unique_ptr<Model> model = makeModel(opt.model, opt.a, opt.b);
        const std::unique_ptr<InputSignal> signal =
            makeSignal(opt.signal, opt.amplitude, opt.omega);

        printHeader(opt, *model, *signal);
        printStability(*model);

        Simulator simulator(*model, *signal, opt.dt);
        const std::vector<Sample> data = simulator.run(opt.steps);

        Simulator::printTable(std::cout, data);
        printSummary(data);

        if (!opt.outPath.empty()) {
            Simulator::writeTable(opt.outPath, data);
            std::cout << "Таблица сохранена в файл: " << opt.outPath << "\n";
        }
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n\n";
        printUsage();
        return 1;
    }
}

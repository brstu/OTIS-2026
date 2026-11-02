#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <memory>
#include <stdexcept>  // для std::invalid_argument, std::out_of_range
#include <cstdlib>    // для std::system

#include "ProcessFactory.h"
#include "InputSignal.h"

// =====================================================================
// НАСТРОЙКИ ПО УМОЛЧАНИЮ
// =====================================================================
constexpr int    DEFAULT_N  = 20;     // количество шагов по умолчанию
constexpr double DEFAULT_DT = 0.01;   // шаг интегрирования
constexpr int    PRECISION  = 5;      // точность вывода

// =====================================================================
// readDouble — безопасный ввод числа с плавающей точкой.
// =====================================================================
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
    }
    catch (const std::invalid_argument&) {
        std::cout << "  Не удалось разобрать, беру значение по умолчанию.\n";
        return def;
    }
    catch (const std::out_of_range&) {
        std::cout << "  Число вне диапазона, беру значение по умолчанию.\n";
        return def;
    }
}

// =====================================================================
// readInt — безопасный ввод целого числа.
// =====================================================================
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
    }
    catch (const std::invalid_argument&) {
        std::cout << "  Не удалось разобрать, беру значение по умолчанию.\n";
        return def;
    }
    catch (const std::out_of_range&) {
        std::cout << "  Число вне диапазона, беру значение по умолчанию.\n";
        return def;
    }
}

// =====================================================================
// readChoice — ввод числа в диапазоне [lo; hi] с использованием
// init-statement (C++17).
// =====================================================================
static int readChoice(const std::string& prompt, int lo, int hi)
{
    while (true) {
        std::cout << prompt;
        std::string line;
        std::getline(std::cin, line);

        try {
            if (const int v = std::stoi(line); v >= lo && v <= hi) {
                return v;
            }
            std::cout << "  Число вне диапазона.\n";
        }
        catch (const std::invalid_argument&) {
            std::cout << "  Это не число.\n";
        }
        catch (const std::out_of_range&) {
            std::cout << "  Число слишком большое.\n";
        }

        std::cout << "  Введите число от " << lo << " до " << hi << ".\n";
    }
}

// =====================================================================
// MAIN — точка входа в программу.
// =====================================================================
int main()
{
    std::cout << "=================================================\n";
    std::cout << "  Лабораторная работа №1 (ОТИС) — ii02919\n";
    std::cout << "  Моделирование управляемого объекта\n";
    std::cout << "  Вариант 17\n";
    std::cout << "=================================================\n\n";

    // ------------------ Выбор модели ------------------
    std::cout << "Выберите модель:\n";
    std::cout << "  1) Model 1.7 — Multi-Step Control History\n";
    std::cout << "  2) Model 2.1 — Quadratic Feedback\n";
    std::cout << "  3) Model 3.5 — Harmonic Driving Force\n";

    const int modelChoice = readChoice("Ваш выбор (1-3): ", 1, 3);

    // ------------------ Ввод коэффициентов ------------------
    double p1 = 0.0;
    double p2 = 0.0;
    double p3 = 0.0;
    double p4 = 0.0;

    if (modelChoice == 1) {
        p1 = readDouble("a  (коэф. затухания)", 0.9);
        p2 = readDouble("b1 (коэф. текущего входа)", 1.0);
        p3 = readDouble("b2 (коэф. входа с задержкой 1)", 0.0);
        p4 = readDouble("b3 (коэф. входа с задержкой 2)", 0.0);
    }
    else if (modelChoice == 2) {
        p1 = readDouble("a (линейная обратная связь)", 0.5);
        p2 = readDouble("b (квадратичная обратная связь)", 0.1);
        p3 = readDouble("c (коэф. текущего входа)", 1.0);
        p4 = readDouble("d (амплитуда гармоники)", 0.5);
    }
    else {
        p1 = readDouble("b  (амплитуда)", 1.0);
        p2 = readDouble("dt (шаг интегрирования)", DEFAULT_DT);
    }

    // ------------------ Создание модели ------------------
    std::unique_ptr<DynamicProcess> model =
        ProcessFactory::create(modelChoice, p1, p2, p3, p4);

    if (!model) {
        std::cout << "Ошибка: не удалось создать модель.\n";
        return 1;
    }

    std::cout << "\nМодель: " << model->title() << "\n";

    // ------------------ Проверка устойчивости ------------------
    if (!model->isStableSystem()) {
        if (const std::string warn = model->alertText(); !warn.empty()) {
            std::cout << "\n!!! ВНИМАНИЕ !!!\n" << warn << "\n";
        }

        std::cout << "Продолжить симуляцию? (y/n): ";
        std::string ans;
        std::getline(std::cin, ans);

        if (!(ans == "y" || ans == "Y")) {
            std::cout << "Отменено пользователем.\n";
            return 0;
        }
    }

    // ------------------ Выбор входного сигнала ------------------
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
    default:
        signal = std::make_unique<Sinusoid>();
        break;
    }

    std::cout << "Сигнал: " << signal->label() << "\n";

    // ------------------ Ввод количества шагов ------------------
    const int n = readInt("\nВведите количество шагов n", DEFAULT_N);
    if (n <= 0) {
        std::cout << "n должно быть > 0\n";
        return 1;
    }

    // ------------------ Симуляция ------------------
    std::cout << "\n========================================\n";
    std::cout << std::setw(6)  << "tau"
              << std::setw(14) << "u_tau"
              << std::setw(14) << "y_tau" << "\n";
    std::cout << "----------------------------------------\n";

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

    std::cout << "----------------------------------------\n";
    std::cout << "Результаты сохранены в result.csv\n";

    // =====================================================================
    // АВТОМАТИЧЕСКАЯ ВИЗУАЛИЗАЦИЯ
    // После симуляции вызываем Python-скрипт, который строит график,
    // а затем открываем полученный PNG-файл.
    // =====================================================================

    // Формируем имя PNG-файла, чтобы каждый запуск не перезаписывал старый.
    // Например, для модели 1 и сигнала 2 получится: "graph_1_2.png".
    const std::string png_name = "graph_"
                               + std::to_string(modelChoice)
                               + "_"
                               + std::to_string(sigChoice)
                               + ".png";

    // Путь к Python-скрипту относительно текущей рабочей директории.
    // Программа запускается из build/src/, значит:
    //   .. → build/
    //   ../.. → task_01/
    // То есть скрипт лежит в "../../plot_results.py".
    const std::string script_path = "../../plot_results.py";

    // Собираем команду целиком (как если бы ты сама её писала в терминале).
    // Кавычки вокруг title() — потому что в названии модели есть пробелы.
    const std::string cmd = "python3 " + script_path
                          + " " + png_name
                          + " \"" + model->title() + "\"";

    std::cout << "\nЗапуск визуализации...\n";
    std::cout << "Команда: " << cmd << "\n";

    // Выполняем команду через системный вызов.
    // std::system возвращает 0, если команда завершилась успешно.
    const int ret = std::system(cmd.c_str());

    if (ret == 0) {
        std::cout << "График сохранён: " << png_name << "\n";

        // Открываем картинку стандартным просмотрщиком.
        // Для macOS — команда "open".
        // Если у тебя Linux — замени "open" на "xdg-open".
        // Если Windows — на "start".
        const std::string open_cmd = "open " + png_name;
        std::system(open_cmd.c_str());
    }
    else {
        std::cout << "Не удалось построить график (код возврата "
                  << ret << ").\n";
        std::cout << "Проверь, что файл " << script_path
                  << " существует и что Python установлен.\n";
    }

    std::cout << "\nГотово.\n";

    return 0;
}

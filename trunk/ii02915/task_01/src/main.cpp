#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>
#include <cmath>
#include <limits>

#include "Model.h"
#include "Signals.h"
#include "Model1_5.h"
#include "Model2_9.h"
#include "Model3_3.h"

constexpr int kMinModelChoice = 1;
constexpr int kMaxModelChoice = 3;
constexpr int kMaxModelK = 99;
constexpr int kMaxSimulationSteps = 100;

constexpr double kDefaultTimeStep = 0.1;

constexpr int kColWidthStep = 10;
constexpr int kColWidthValue = 15;
constexpr int kOutputPrecision = 4;

void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    std::setlocale(LC_ALL, "");

    int choice = 0;

    while (true) {
        std::cout << "Выбор модели\n";
        std::cout << "1. Model 1.5 (Линейная)\n";
        std::cout << "2. Model 2.9 (Нелинейная)\n";
        std::cout << "3. Model 3.3 (Дифференциальная)\n";
        std::cout << "Выбор (1-3): ";

        if (std::cin >> choice && choice >= kMinModelChoice && choice <= kMaxModelChoice) {
            break;
        }
        std::cout << "Неверный ввод, попробуйте еще раз\n\n";
        clearInputBuffer();
    }

    std::unique_ptr<Model> model;

    double a1 = 0.0;
    double a2 = 0.0;
    double b = 0.0;
    int k = 1;
    double initial_y = 0.0;
    double dt = kDefaultTimeStep;

    std::cout << "Введите параметры: \n";
    switch (choice) {
    case 1:
        while (true) {
            std::cout << "a1, a2, b, k (до 99), начальный y - ";
            if (std::cin >> a1 >> a2 >> b >> k >> initial_y && k >= 0 && k <= kMaxModelK) {
                model = std::make_unique<Model1_5>(a1, a2, b, k, initial_y);
                break;
            }
            std::cout << "Неверный ввод, попробуйте еще раз\n";
            clearInputBuffer();
        }
        break;

    case 2:
        while (true) {
            std::cout << "a, b, начальный y - ";
            if (std::cin >> a1 >> b >> initial_y) {
                model = std::make_unique<Model2_9>(a1, b, initial_y);
                break;
            }
            std::cout << "Неверный ввод, попробуйте еще раз\n";
            clearInputBuffer();
        }
        break;

    case 3:
        while (true) {
            std::cout << "a, b, dt, начальный y - ";
            if (std::cin >> a1 >> b >> dt >> initial_y && dt > 0.0) {
                model = std::make_unique<Model3_3>(a1, b, initial_y);
                break;
            }
            std::cout << "Неверный ввод, попробуйте еще раз\n";
            clearInputBuffer();
        }
        break;

    default:
        break;
    }

    int signalChoice = 0;
    double amplitude = 0.0;
    int steps = 0;

    while (true) {
        std::cout << "\nТип входного сигнала:\n";
        std::cout << "1. Шаговый (u = const)\n";
        std::cout << "2. Импульсный (u0 = amplitude, иначе 0)\n";
        std::cout << "3. Гармонический (u = amplitude * sin(tau))\n";
        std::cout << "Выбор (1-3): ";

        if (std::cin >> signalChoice && signalChoice >= kMinModelChoice && signalChoice <= kMaxModelChoice) {
            break;
        }
        std::cout << "Неверный ввод, попробуйте еще раз\n";
        clearInputBuffer();
    }

    auto signalType = static_cast<SignalType>(signalChoice);

    while (true) {
        std::cout << "Введите амплитуду сигнала: ";
        if (std::cin >> amplitude && std::isfinite(amplitude)) {
            break;
        }
        std::cout << "[Ошибка] Неверная амплитуда. Введите корректное число.\n";
        clearInputBuffer();
    }

    while (true) {
        std::cout << "Введите количество шагов симуляции (от 1 до 100): ";
        if (std::cin >> steps && steps > 0 && steps <= kMaxSimulationSteps) {
            break;
        }
        std::cout << "[Ошибка] Количество шагов должно быть от 1 до 100. Попробуйте еще раз.\n";
        clearInputBuffer();
    }

    std::ofstream file("result.csv");
    if (!file.is_open()) {
        std::cout << "Ошибка: не удалось создать файл result.csv.\n";
        return 1;
    }

    file << "Step,Input,Output\n";

    std::cout << "\nЗапуск модели: " << model->getName() << "\n";
    std::cout << std::fixed << std::setprecision(kOutputPrecision);
    std::cout << std::setw(kColWidthStep) << "Шаг"
        << std::setw(kColWidthValue) << "Вход"
        << std::setw(kColWidthValue) << "Выход" << "\n";

    for (int tau = 0; tau < steps; ++tau) {
        double u = getInput(signalType, tau, amplitude);
        double nextY = model->calculateNext(u, dt);

        if (!std::isfinite(nextY)) {
            std::cout << "Ошибка: модель вернула недопустимое значение (NaN/Inf).\n";
            file.close();
            return 1;
        }

        std::cout << std::setw(kColWidthStep) << tau
            << std::setw(kColWidthValue) << u
            << std::setw(kColWidthValue) << nextY << "\n";

        file << tau << "," << u << "," << nextY << "\n";
    }

    file.close();

    std::cout << "\nСимуляция успешно завершена.\n";
    std::cout << "Результаты сохранены в файл result.csv.\n";

    return 0;
}
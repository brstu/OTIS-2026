#include <iostream>
#include <memory>
#include "models.h"
#include "Utils.h"

int main() {
    int modelChoice, inputChoice, steps;
    double a, b, a2 = 0.0, delta = 0.0, customU = 0.0;

    std::cout << "=== ЛАБОРАТОРНАЯ РАБОТА: МОДЕЛИРОВАНИЕ СИСТЕМ ===\n";

    // 1. Выбор модели
    std::cout << "\nВыберите модель:\n";
    std::cout << "1. Линейная модель (1-го порядка)\n";
    std::cout << "2. Нелинейная модель (с зоной нечувствительности)\n";
    std::cout << "3. Модель на основе дифференциального уравнения (2-го порядка)\n";
    std::cout << "Ваш выбор (1, 2 или 3): ";
    std::cin >> modelChoice;

    std::unique_ptr<Model> model = nullptr;

    if (modelChoice == 1) {
        std::cout << "Введите коэффициент a: ";
        std::cin >> a;
        std::cout << "Введите коэффициент b: ";
        std::cin >> b;
        model = std::make_unique<LinearModel1_1>(a, b);
    }
    else if (modelChoice == 2) {
        std::cout << "Введите коэффициент a: ";
        std::cin >> a;
        std::cout << "Введите коэффициент b: ";
        std::cin >> b;
        std::cout << "Введите параметр зоны нечувствительности (delta): ";
        std::cin >> delta;
        model = std::make_unique<NonLinearModel2_3>(a, b, delta);
    }
    else if (modelChoice == 3) {
        std::cout << "Введите коэффициент a1 (при y_prev1): ";
        std::cin >> a;
        std::cout << "Введите коэффициент a2 (при y_prev2): ";
        std::cin >> a2;
        std::cout << "Введите коэффициент b0 (при входе u): ";
        std::cin >> b;
        model = std::make_unique<DifferentialModel>(a, a2, b);
    }
    else {
        std::cout << "Неверный выбор! По умолчанию используется линейная модель.\n";
        std::cout << "Введите коэффициент a: ";
        std::cin >> a;
        std::cout << "Введите коэффициент b: ";
        std::cin >> b;
        model = std::make_unique<LinearModel1_1>(a, b);
    }

    // 2. Выбор типа воздействия
    std::cout << "\nВыберите тип входного воздействия (u):\n";
    std::cout << "1. Ступенчатое\n";
    std::cout << "2. Импульсное\n";
    std::cout << "3. Гармоническое (синусоида)\n";
    std::cout << "Ваш выбор (1, 2 или 3): ";
    std::cin >> inputChoice;

    // 3. Ввод входного значения
    std::cout << "Введите значение входного воздействия (например, 1.0, 5.0): ";
    std::cin >> customU;

    // 4. Количество шагов
    std::cout << "Введите количество шагов симуляции (например, 30): ";
    std::cin >> steps;

    // 5. Запуск
    runSimulation(*model, "simulation_result", steps, inputChoice, customU);

    std::cout << "\nСимуляция успешно завершена!\n";
    return 0;
}
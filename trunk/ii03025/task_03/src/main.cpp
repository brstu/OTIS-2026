#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>
#include <string>
#include <cmath>
#include "../../task_01/src/model19.h"
#include "../../task_01/src/model23.h"
#include "../../task_01/src/model37.h"
#include "pid.h"

int main() {

    int modelChoice;

    std::cout << "===== СИСТЕМА АВТОМАТИЧЕСКОГО УПРАВЛЕНИЯ =====\n\n";

    std::cout << "Выберите объект управления:\n";
    std::cout << "1. Модель 1.9\n";
    std::cout << "2. Модель 2.3\n";
    std::cout << "3. Модель 3.7\n";
    std::cout << "Ваш выбор: ";
    std::cin >> modelChoice;

    std::unique_ptr<Model> model;

    double dt = 0.1;

    if (modelChoice == 1) {

        double a1;
        double a2;
        double a3;
        double b1;

        std::cout << "\nВведите коэффициенты a1, a2, a3, b1: ";
        std::cin >> a1 >> a2 >> a3 >> b1;

        model = std::make_unique<Model19>(a1, a2, a3, b1);

    }
    else if (modelChoice == 2) {

        double a;
        double b;
        double delta;

        std::cout << "\nВведите коэффициенты a, b и зону нечувствительности: ";
        std::cin >> a >> b >> delta;

        model = std::make_unique<Model23>(a, b, delta);

    }
    else if (modelChoice == 3) {

        double a;
        double b;

        std::cout << "\nВведите коэффициенты a и b: ";
        std::cin >> a >> b;

        std::cout << "Введите шаг времени: ";
        std::cin >> dt;

        if (dt <= 0) {
            std::cout << "Ошибка: шаг времени должен быть положительным!\n";
            return 1;
        }

        model = std::make_unique<Model37>(a, b, dt);

    }
    else {

        std::cout << "Ошибка: неверный выбор модели!\n";
        return 1;

    }

    double setpoint;

    std::cout << "\nВведите заданную температуру: ";
    std::cin >> setpoint;

    double kp;
    double ki;
    double kd;

    std::cout << "\nВведите коэффициенты ПИД-регулятора:\n";

    std::cout << "Kp: ";
    std::cin >> kp;

    std::cout << "Ki: ";
    std::cin >> ki;

    std::cout << "Kd: ";
    std::cin >> kd;

    double uMin;
    double uMax;

    std::cout << "\nВведите ограничения управляющего сигнала:\n";

    std::cout << "Минимальное значение: ";
    std::cin >> uMin;

    std::cout << "Максимальное значение: ";
    std::cin >> uMax;

    if (uMin >= uMax) {
        std::cout << "Ошибка: неверные ограничения управляющего сигнала!\n";
        return 1;
    }

    PIDController pid(kp, ki, kd, uMin, uMax);

    int mode;

    std::cout << "\nВыберите режим моделирования:\n";
    std::cout << "1. Фиксированное количество шагов\n";
    std::cout << "2. До достижения заданной точности\n";
    std::cout << "Ваш выбор: ";
    std::cin >> mode;

    int maxSteps = 0;
    double epsilon = 0.0;

    if (mode == 1) {

        std::cout << "\nВведите количество шагов моделирования: ";
        std::cin >> maxSteps;

    }
    else if (mode == 2) {

        std::cout << "\nВведите допустимую погрешность: ";
        std::cin >> epsilon;

        std::cout << "Введите максимальное количество шагов: ";
        std::cin >> maxSteps;

    }
    else {

        std::cout << "Ошибка: неверный режим моделирования!\n";
        return 1;

    }

    if (maxSteps <= 0 || (mode == 2 && epsilon <= 0)) {
        std::cout << "Ошибка: неверные параметры моделирования!\n";
        return 1;
    }

    double output = 0.0;
    double control = 0.0;

    int stableSteps = 0;

    std::ofstream file("results.csv");

    if (!file.is_open()) {
        std::cout << "Ошибка: не удалось создать файл results.csv!\n";
        return 1;
    }

    file << "Шаг;Время;Заданная температура;Выход;Ошибка;Управляющий сигнал\n";

    std::cout << "\nОбъект управления: " << model->getName() << "\n";
    std::cout << "Шаг времени: " << dt << "\n\n";

    std::cout << std::fixed << std::setprecision(4);

    std::cout << std::setw(11) << "Шаг"
          << std::setw(17) << "Время"
          << std::setw(30) << "Заданная темп."
          << std::setw(19) << "Выход"
          << std::setw(20) << "Ошибка"
          << std::setw(24) << "Управление"
          << "\n";

    for (int i = 0; i < maxSteps; i++) {

        // Вычисление управляющего сигнала
        control = pid.calculate(setpoint, output, dt);

        // Передача сигнала объекту управления
        output = model->nextStep(control);

        // Вычисление ошибки регулирования
        double error = setpoint - output;

        // Вычисление текущего времени
        double time = (i + 1) * dt;

        // Вывод результатов
        std::cout << std::setw(8) << i + 1
                  << std::setw(12) << time
                  << std::setw(18) << setpoint
                  << std::setw(14) << output
                  << std::setw(14) << error
                  << std::setw(14) << control
                  << "\n";

        // Запись результатов в CSV-файл
        file << i + 1 << ";"
             << time << ";"
             << setpoint << ";"
             << output << ";"
             << error << ";"
             << control << "\n";

        // Проверка достижения заданной точности
        if (mode == 2) {

            if (std::abs(error) <= epsilon) {
                stableSteps++;
            }
            else {
                stableSteps = 0;
            }

            if (stableSteps >= 10) {
                std::cout << "\nЗаданная точность достигнута!\n";
                break;
            }

        }

    }

    file.close();

    std::cout << "\nМоделирование завершено.\n";
    std::cout << "Результаты сохранены в файле results.csv\n";

    return 0;
}

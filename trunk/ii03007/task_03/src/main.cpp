/// @file main.cpp
/// @brief ПИД-регулятор и объекты варианта 23: модель 1.6, модель 2.1, уравнение 3.7.

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <cmath>
#include <memory>
#include <string>
#include <limits>

#include "../../task_01/src/model.h"
#include "../../task_01/src/model1.6.h"
#include "../../task_01/src/model2.1.h"
#include "../../task_01/src/model3.7.h"
#include "../../task_02/src/stability.h"
#include "pid_regulator.h"

double readDouble(const std::string& prompt);
int readInt(const std::string& prompt, int minValue, int maxValue);
std::unique_ptr<BaseModel> makePlant(int choice);
PidRegulator makeRegulator();
void chooseMode(int& mode, int& steps, double& eps);
void runLoop(BaseModel& plant, PidRegulator& pid, double w, int mode, int steps,
             double eps, std::ofstream& csv);

int main(){
    std::remove("pid_result.csv");

    std::cout << "ЛР3. ПИД-регулятор, вариант 23. Выберите объект управления:" << std::endl;
    std::cout << " 1 - модель 1.6 (линейная): y(t+1) = a1*y(t) + a2*y(t-1) + a3*y(t-2) + b*u(t)" << std::endl;
    std::cout << " 2 - модель 2.1 (нелинейная): y(t+1) = a*y(t) - b*y(t-1)^2 + c*u(t) + d*sin(u(t-1))" << std::endl;
    std::cout << " 3 - уравнение 3.7 (дифференциальное): dy/dt = -exp(a)*y + b*u, схема Эйлера" << std::endl;
    const int choice = readInt("Ваш выбор: ", 1, 3);

    std::unique_ptr<BaseModel> plant = makePlant(choice);
    if (!plant){
        std::cout << "Симуляция отменена." << std::endl;
        return 0;
    }

    PidRegulator pid = makeRegulator();

    const double w = readDouble("Задание w = ");

    int mode = 1;
    int steps = 0;
    double eps = 0.0;
    chooseMode(mode, steps, eps);

    std::cout << std::endl;
    std::cout << "  такт |  задание w |   ошибка e |  управл. u |   выход y" << std::endl;
    std::cout << std::string(62, '=') << std::endl;

    std::ofstream csv("pid_result.csv");
    if (!csv){
        std::cout << "Ошибка: не удалось создать pid_result.csv" << std::endl;
        return 1;
    }
    csv << "t;w;e;u;y" << std::endl;
    csv << std::fixed << std::setprecision(4);

    runLoop(*plant, pid, w, mode, steps, eps, csv);
    csv.close();

    std::cout << std::string(62, '=') << std::endl;
    std::cout << "Таблица записана в pid_result.csv" << std::endl;
    std::cout << "График: python plot.py" << std::endl;
    return 0;
}

std::unique_ptr<BaseModel> makePlant(int choice){
    if (choice == 1){
        std::cout << std::endl << "Модель 1.6. Коэффициенты:" << std::endl;
        const double a1 = readDouble("  a1 = ");
        const double a2 = readDouble("  a2 = ");
        const double a3 = readDouble("  a3 = ");
        const double b  = readDouble("  b  = ");

        std::cout << std::endl;
        printStabilityCheck(a1, a2, a3, std::cout);
        std::cout << std::endl;

        if (!stable16(a1, a2, a3)){
            std::cout << "Объект неустойчив. Продолжить симуляцию? (y/n): ";
            std::string answer;
            std::cin >> answer;
            if (!(answer == "y" || answer == "Y")){
                return nullptr;
            }
        }
        return std::make_unique<model1_6>(a1, a2, a3, b);
    }

    if (choice == 2){
        std::cout << std::endl << "Модель 2.1. Коэффициенты:" << std::endl;
        const double a = readDouble("  a = ");
        const double b = readDouble("  b = ");
        const double c = readDouble("  c = ");
        const double d = readDouble("  d = ");
        return std::make_unique<model2_1>(a, b, c, d);
    }

    std::cout << std::endl << "Уравнение 3.7. Параметры:" << std::endl;
    const double a = readDouble("  a = ");
    const double b = readDouble("  b = ");
    double h = 0.0;
    while (true){
        h = readDouble("  h (шаг Эйлера) = ");
        if (h > 0.0) break;
        std::cout << "Шаг h должен быть положительным!" << std::endl;
    }
    return std::make_unique<model3_7>(a, b, h);
}

PidRegulator makeRegulator(){
    std::cout << std::endl << "Настройка ПИД-регулятора:" << std::endl;
    const double kp = readDouble("  Kp = ");
    const double ki = readDouble("  Ki = ");
    const double kd = readDouble("  Kd = ");

    double dt = 0.0;
    while (true){
        dt = readDouble("  dt (шаг квантования) = ");
        if (dt > 0.0) break;
        std::cout << "Шаг dt должен быть положительным!" << std::endl;
    }

    double uMin = 0.0;
    double uMax = 0.0;
    while (true){
        uMin = readDouble("  uMin = ");
        uMax = readDouble("  uMax = ");
        if (uMin < uMax) break;
        std::cout << "uMin должен быть меньше uMax!" << std::endl;
    }
    return PidRegulator(kp, ki, kd, dt, uMin, uMax);
}

void chooseMode(int& mode, int& steps, double& eps){
    std::cout << std::endl << "Режим симуляции:" << std::endl;
    std::cout << " 1 - режим А: фиксированное число тактов n" << std::endl;
    std::cout << " 2 - режим Б: до установления |e| < eps" << std::endl;
    mode = readInt("Ваш выбор: ", 1, 2);

    if (mode == 1){
        steps = readInt("Число тактов n: ", 1, 100000);
    } else {
        while (true){
            eps = readDouble("Точность eps: ");
            if (eps > 0.0) break;
            std::cout << "eps должна быть положительной!" << std::endl;
        }
        steps = 100000;
    }
}

void runLoop(BaseModel& plant, PidRegulator& pid, double w, int mode, int steps,
             double eps, std::ofstream& csv){
    double y = 0.0;
    bool reached = false;

    for (int t = 0; t < steps; ++t){
        const double e = w - y;
        const double u = pid.compute(w, y);
        y = plant.stepForward(u);

        std::cout << std::setw(6)  << t << " |"
                  << std::setw(11) << std::fixed << std::setprecision(4) << w << "  |"
                  << std::setw(11) << e << "  |"
                  << std::setw(11) << u << "  |"
                  << std::setw(11) << y << std::endl;
        csv << t << ";" << w << ";" << e << ";" << u << ";" << y << std::endl;

        if (mode == 2 && std::fabs(e) < eps){
            reached = true;
            break;
        }
    }

    std::cout << std::endl;
    if (mode == 2){
        if (reached)
            std::cout << "Установившийся режим достигнут: |e| < eps." << std::endl;
        else
            std::cout << "ВНИМАНИЕ: |e| >= eps за отведённые такты." << std::endl;
    }
}

double readDouble(const std::string& prompt){
    for (;;){
        std::cout << prompt;
        if (double value{}; std::cin >> value) return value;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Это не число, попробуйте ещё раз." << std::endl;
    }
}

int readInt(const std::string& prompt, int minValue, int maxValue){
    for (;;){
        std::cout << prompt;
        if (int value{}; (std::cin >> value) && value >= minValue && value <= maxValue) return value;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Нужно целое число от " << minValue << " до " << maxValue << "!" << std::endl;
    }
}

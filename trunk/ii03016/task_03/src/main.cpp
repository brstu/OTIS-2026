#include <iostream>
#include <iomanip>
#include <limits>
#include "Simulation.h"

/**
 * @file main.cpp
 * @brief Главный файл программы для симуляции САУ с ПИД-регулятором.
 * @details Реализует пользовательское меню для выбора модели, режима симуляции и настройки коэффициентов.
 */

/**
 * @brief Очистка потока ввода (защита от некорректного ввода).
 */
void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    system("chcp 65001 > nul");
    std::cout << "==================================================\n";
    std::cout << "   Система автоматического управления (ПИД-регулятор)\n";
    std::cout << "==================================================\n\n";

    // 1. Выбор объекта управления
    std::cout << "Выберите объект управления (Plant):\n";
    std::cout << "1 - Линейная модель 1.10\n";
    std::cout << "2 - Нелинейная модель 2.5\n";
    std::cout << "3 - Дифференциальное уравнение 3.10\n";
    std::cout << "Ваш выбор: ";
    
    int plant_choice;
    std::cin >> plant_choice;
    
    PlantType plant;
    if (plant_choice == 1) plant = PlantType::Linear_1_10;
    else if (plant_choice == 2) plant = PlantType::Nonlinear_2_5;
    else if (plant_choice == 3) plant = PlantType::Diff_3_10;
    else {
        std::cerr << "Ошибка: неверный выбор модели.\n";
        return 1;
    }

    // 2. Ввод параметров ПИД-регулятора
    std::cout << "\n--- Настройка ПИД-регулятора ---\n";
   double Kp = 0.0;
double Ki = 0.0;
double Kd = 0.0;

    std::cout << "Введите Kp (пропорциональный): ";
    std::cin >> Kp;
    std::cout << "Введите Ki (интегральный): ";
    std::cin >> Ki;
    std::cout << "Введите Kd (дифференциальный): ";
    std::cin >> Kd;

    // 3. Ввод уставки (заданной температуры)
    double setpoint;
    std::cout << "\nВведите заданное значение (уставку): ";
    std::cin >> setpoint;

    // 4. Выбор режима симуляции
    std::cout << "\nВыберите режим симуляции:\n";
    std::cout << "А - Фиксированное количество шагов (n)\n";
    std::cout << "Б - До достижения установившегося режима (|e| < epsilon)\n";
    std::cout << "Ваш выбор (A/B): ";
    
    char mode;
    std::cin >> mode;

    // Создаем объект симуляции
    Simulation sim(plant, 0.1);

    if (mode == 'A' || mode == 'a') {
        int n_steps;
        std::cout << "Введите количество шагов n: ";
        std::cin >> n_steps;
        sim.runFixedSteps(setpoint, n_steps, Kp, Ki, Kd);
    } 
    else if (mode == 'B' || mode == 'b') {
        double epsilon;
        int max_steps;
        std::cout << "Введите допустимую ошибку epsilon (например, 0.01): ";
        std::cin >> epsilon;
        std::cout << "Введите максимальное число шагов (защита от зацикливания): ";
        std::cin >> max_steps;
        sim.runUntilSteadyState(setpoint, epsilon, max_steps, Kp, Ki, Kd);
    } 
    else {
        std::cerr << "Ошибка: неверный режим симуляции.\n";
        return 1;
    }
 // Автоматическое построение и сохранение графика
    std::cout << "\nСтроим график переходного процесса...\n";
    system("python plot.py");

    return 0;
}
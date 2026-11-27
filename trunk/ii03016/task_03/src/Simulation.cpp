#include "Simulation.h"
#include "PID.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <memory>

// Подключаем модели из ЛР1 (обрати внимание на путь, как мы делали во второй лабе)
#include "../../task_01/src/Model 1.10.h"
#include "../../task_01/src/Model 2.5.h"
#include "../../task_01/src/Model 3.10.h"

Simulation::Simulation(PlantType plant_type, double dt_) 
    : current_plant(plant_type), dt(dt_) {}

void Simulation::runFixedSteps(double setpoint, int n_steps, double Kp, double Ki, double Kd) {
    // Создаем регулятор
    PID pid(Kp, Ki, Kd, dt, -100.0, 100.0); // Ограничение u от -100 до 100

    // Создаем объект управления (выбираем нужную модель)
    std::unique_ptr<Model> plant;
    if (current_plant == PlantType::Linear_1_10) {
        plant = std::make_unique<LineModel_1_10>(0.8, 1.0, 0.5);
    } else if (current_plant == PlantType::Nonlinear_2_5) {
        plant = std::make_unique<NonlinearModel_2_5>(0.7, 0.5);
    } else {
        plant = std::make_unique<Diff_3_10>(0.5, 0.2, dt);
    }

    // Открываем файл для записи (CSV)
    std::ofstream file("simulation_results.csv");
    file << "Step,Setpoint,Error,U,Y\n";

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Step\tSetpoint\tError\tU(k)\tY(k)\n";
    std::cout << "----------------------------------------------------\n";

    double y = 0.0; // Начальное состояние объекта
    for (int k = 0; k < n_steps; ++k) {
        // 1. Вычисляем ошибку
        double error = setpoint - y;
        
        // 2. ПИД-регулятор вычисляет управляющее воздействие u(k)
        double u = pid.compute(setpoint, y);
        
        // 3. Подаем u(k) на объект управления и получаем y(k+1)
        y = plant->step(u);

        // 4. Вывод в консоль (табличный вид)
        std::cout << k << "\t" << setpoint << "\t\t" << error << "\t" << u << "\t" << y << "\n";
        
        // 5. Запись в CSV
        file << k << "," << setpoint << "," << error << "," << u << "," << y << "\n";
    }

    file.close();
    std::cout << "----------------------------------------------------\n";
    std::cout << "Simulation finished. Data saved to simulation_results.csv\n";
}

void Simulation::runUntilSteadyState(double setpoint, double epsilon, int max_steps, double Kp, double Ki, double Kd) {
    // Создаем регулятор
    PID pid(Kp, Ki, Kd, dt, -100.0, 100.0);

    // Создаем объект управления
    std::unique_ptr<Model> plant;
    if (current_plant == PlantType::Linear_1_10) {
        plant = std::make_unique<LineModel_1_10>(0.8, 1.0, 0.5);
    } else if (current_plant == PlantType::Nonlinear_2_5) {
        plant = std::make_unique<NonlinearModel_2_5>(0.7, 0.5);
    } else {
        plant = std::make_unique<Diff_3_10>(0.5, 0.2, dt);
    }

    std::ofstream file("simulation_results.csv");
    file << "Step,Setpoint,Error,U,Y\n";

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Step\tSetpoint\tError\tU(k)\tY(k)\n";
    std::cout << "----------------------------------------------------\n";

    double y = 0.0;
    int k = 0;
    double error = setpoint - y;

    // Цикл работает, пока ошибка больше epsilon и не превышен лимит шагов
    while (std::fabs(error) > epsilon && k < max_steps) {
        error = setpoint - y;
        double u = pid.compute(setpoint, y);
        y = plant->step(u);

        std::cout << k << "\t" << setpoint << "\t\t" << error << "\t" << u << "\t" << y << "\n";
        file << k << "," << setpoint << "," << error << "," << u << "," << y << "\n";
        
        k++;
    }

    file.close();
    std::cout << "----------------------------------------------------\n";
    if (std::fabs(error) <= epsilon) {
        std::cout << "Steady state reached at step " << k << " (Error = " << error << ")\n";
    } else {
        std::cout << "Max steps reached (" << max_steps << "). Error = " << error << "\n";
    }
}
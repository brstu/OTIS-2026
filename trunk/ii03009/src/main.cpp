#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <windows.h>

#include "linear_model_1_9.h"
#include "nonlinear_model_2_9.h"
#include "diff_eq_3_9.h"

void removeOldFiles() {
    std::remove("out_linear_step.csv");
    std::remove("out_linear_impulse.csv");
    std::remove("out_linear_sin.csv");
    std::remove("out_nonlinear_step.csv");
    std::remove("out_nonlinear_impulse.csv");
    std::remove("out_nonlinear_sin.csv");
    std::remove("out_diffeq_step.csv");
    std::remove("out_diffeq_impulse.csv");
    std::remove("out_diffeq_sin.csv");
}

void simulate(Model* model, const std::string& filename, int n, int signal_type) {
    std::ofstream csv(filename);
    if (!csv.is_open()) {
        std::cerr << "Ошибка: не удалось создать файл " << filename << "\n";
        return;
    }
    csv << "step,u,y\n";
    model->reset();
    
    for (int t = 0; t < n; ++t) {
        double u = 0.0;
        if (signal_type == 0) u = 1.0;                      
        else if (signal_type == 1) u = (t == 0) ? 1.0 : 0.0; 
        else if (signal_type == 2) u = std::sin(0.5 * t);    
        
        double y = model->step(u);
        csv << t << "," << u << "," << y << "\n";
    }
    csv.close();
}

void runAllSignals(Model* model, const std::string& prefix) {
    std::cout << "  1. Ступенчатый сигнал..." << std::endl;
    simulate(model, prefix + "_step.csv", 50, 0); 
    
    std::cout << "  2. Импульсный сигнал..." << std::endl;
    simulate(model, prefix + "_impulse.csv", 50, 1);
    
    std::cout << "  3. Гармонический сигнал..." << std::endl;
    simulate(model, prefix + "_sin.csv", 50, 2);
    
    std::cout << "  Данные сохранены." << std::endl;
}

int main() {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    int choice;
    std::string prefix; 

    std::cout << " ВЫБЕРИТЕ МОДЕЛЬ \n";
    std::cout << "1 - Линейная модель 1.9\n";
    std::cout << "2 - Нелинейная модель 2.9\n";
    std::cout << "3 - Дифференциальное уравнение 3.9\n";
    std::cout << "Ваш выбор (1/2/3): ";
    std::cin >> choice;

    removeOldFiles();

    if (choice == 1) {
        double a1, a2, a3, b1;
        std::cout << "Введите коэффициенты a1, a2, a3, b1: ";
        std::cin >> a1 >> a2 >> a3 >> b1;
        LinearModel1_9 m(a1, a2, a3, b1);
        prefix = "out_linear";
        runAllSignals(&m, prefix);
    } 
    else if (choice == 2) {
        double a, b;
        std::cout << "Введите коэффициенты a, b: ";
        std::cin >> a >> b;
        NonlinearModel2_9 m(a, b);
        prefix = "out_nonlinear";
        runAllSignals(&m, prefix);
    } 
    else if (choice == 3) {
        double b, dt;
        std::cout << "Введите коэффициент b и dt: ";
        std::cin >> b >> dt;
        DiffEq3_9 m(b, dt);
        prefix = "out_diffeq";
        runAllSignals(&m, prefix);
    } 
    else {
        std::cout << "Неверный выбор!\n";
        return 1;
    }

    std::cout << "\nПостроение графиков...\n";
    
    std::string cmd = "python plot.py " + prefix;
    system(cmd.c_str());

    return 0;
}
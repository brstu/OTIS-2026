/**
 * @file main.cpp
 * @brief Main file for Lab 3: PID Controller simulation.
 */

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <fstream>

#include "Plant.h"
#include "PIDController.h"

int main() {

    // --- 1. Plant parameters ---
    double a, b;
    std::cout << "Enter plant parameters (Model 1.4):\n";
    std::cout << "  Coefficient a [default 0.9]: ";
    if (!(std::cin >> a)) a = 0.9;
    std::cout << "  Coefficient b [default 1.0]: ";
    if (!(std::cin >> b)) b = 1.0;

    // --- 2. PID parameters ---
    double Kp, Ki, Kd, T0;
    std::cout << "\nEnter PID coefficients:\n";
    std::cout << "  Kp [default 1.0]: ";  if (!(std::cin >> Kp)) Kp = 1.0;
    std::cout << "  Ki [default 0.1]: ";  if (!(std::cin >> Ki)) Ki = 0.1;
    std::cout << "  Kd [default 0.05]: "; if (!(std::cin >> Kd)) Kd = 0.05;
    std::cout << "  Sampling step T0 [default 0.01]: "; if (!(std::cin >> T0)) T0 = 0.01;

    // --- 3. Simulation mode ---
    int mode;
    std::cout << "\nSelect simulation mode:\n";
    std::cout << "  1) Mode A: fixed number of steps (n)\n";
    std::cout << "  2) Mode B: until |e| < epsilon\n";
    std::cout << "Your choice (1-2): ";
    std::cin >> mode;

    int n = 100;
    double epsilon = 0.01;

    if (mode == 1) {
        std::cout << "  Enter number of steps n [default 100]: ";
        if (!(std::cin >> n)) n = 100;
    } else {
        std::cout << "  Enter error threshold epsilon [default 0.01]: ";
        if (!(std::cin >> epsilon)) epsilon = 0.01;
    }

    // --- 4. Init system ---
    Plant plant(a, b);
    PIDController pid(Kp, Ki, Kd, T0, -10.0, 10.0);

    double target = 1.0;

    std::cout << "\n===========================================\n";
    std::cout << std::setw(6) << "k" 
              << std::setw(12) << "w (target)" 
              << std::setw(12) << "y (plant)" 
              << std::setw(12) << "e (error)" 
              << std::setw(12) << "u (control)" << "\n";
    std::cout << "-------------------------------------------\n";

    std::ofstream csv("result_task03.csv");
    csv << "k,w,y,e,u\n";

    // --- 5. Simulation loop ---
    int step = 0;
    while (true) {
        step++;
        
        double y = plant.getOutput();
        double e = target - y;
        
        if (mode == 2 && std::abs(e) < epsilon) {
            std::cout << "Target accuracy achieved (|e| < " << epsilon << ") at step " << step << "\n";
            break;
        }

        if (mode == 1 && step > n) {
            break;
        }

        if (step > 10000) {
            std::cout << "Iteration limit (10000) reached. Force stop.\n";
            break;
        }

        double u = pid.compute(e);

        std::cout << std::setw(6) << step 
                  << std::setw(12) << std::fixed << std::setprecision(4) << target
                  << std::setw(12) << y
                  << std::setw(12) << e
                  << std::setw(12) << u << "\n";

        csv << step << "," << target << "," << y << "," << e << "," << u << "\n";

        plant.step(u);
    }

    csv.close();
    std::cout << "-------------------------------------------\n";
    std::cout << "Simulation finished. Results saved to result_task03.csv\n";

#ifdef _WIN32
    std::cout << "\nGenerating plot...\n";
    // 1. Запускаем Python для создания картинки
    std::system("python plot_pid.py");
    // 2. Принудительно открываем картинку через Windows
    std::system("start plot_task03.png");
#endif
    return 0;
}
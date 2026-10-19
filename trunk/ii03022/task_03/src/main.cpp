#include <iostream>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <memory>

#include "Model.h"
#include "Model 1.8.h"
#include "Model 2.2.h"
#include "Model 3.6.h"
#include "StabilityAnalyzer.h"
#include "PIDController.h"

double checkInputDouble(const std::string& text);
int checkInputInt(const std::string& text, int minVal, int maxVal);

int main(){
    std::remove("pid_result.csv");
    std::cout << "Choose controlled plant:" << std::endl;
    std::cout << " 1 - Model 1.8 (Generalized Autoregressive Linear Model)" << std::endl;
    std::cout << " 2 - Model 2.2 (Actuator Saturation Non-linearity)" << std::endl;
    std::cout << " 3 - Model 3.6 (Cubic Growth and Control)" << std::endl;
    int user_choice = checkInputInt("Your choice: ", 1, 3);

    std::unique_ptr<Model> plant;
    switch(user_choice){
        case 1:{
            std::cout << "Model 1.8: y(t+1) = a1*y(t) + a2*y(t-1) + b1*u(t) + b2*u(t-1)" << std::endl;
            double a1 = checkInputDouble("a1 = ");
            double a2 = checkInputDouble("a2 = ");
            double b1 = checkInputDouble("b1 = ");
            double b2 = checkInputDouble("b2 = ");
            plant = std::make_unique<GeneralizedAutoregressiveLinearModel>(a1, a2, b1, b2);
            std::cout << std::endl;
            printStabilityReport(a1, a2, std::cout);   // модуль сам печатает корни и вердикт
            std::cout << std::endl;
            if (!isStable(a1, a2)){                    // main решает только: продолжать или нет
                std::cout << "Simulate unstable plant? (Y/N): ";
                std::string answer;
                std::cin >> answer;
                if (!(answer == "y" || answer == "Y")){
                    std::cout << "Simulation cancelled." << std::endl;
                    return 1;
                }
            }
            break;
        }
        case 2:{
            std::cout << "Model 2.2: y(t+1) = a*y(t) + b*sat(u(t))" << std::endl;
            double a = checkInputDouble("a = ");
            double b = checkInputDouble("b = ");
            double uMin;
            double uMax;
            while(true){
                uMin = checkInputDouble("plant uMin = ");
                uMax = checkInputDouble("plant uMax = ");
                if (uMin < uMax) break;
                std::cout << "uMin must be less than uMax! Enter another numbers." << std::endl;
            }
            plant = std::make_unique<ActuatorSaturationNonLinearityModel>(a, b, uMin, uMax);
            break;
        }
        case 3:{
            std::cout << "Model 3.6: dy/dt = a*y^3 + b*u (y = y + h*(a*y^3 + b*u))" << std::endl;
            double a = checkInputDouble("a = ");
            double b = checkInputDouble("b = ");
            double h;
            while (true){
                h = checkInputDouble("h (Euler step) = ");
                if (h > 0) break;
                std::cout << "h must be positive!" << std::endl;
            }
            plant = std::make_unique<CubicGrowthAndControlModel>(a, b, h);
            break;
        }
        default: return 0;
    }

    std::cout << std::endl << "PID controller settings:" << std::endl;
    double Kp = checkInputDouble("Kp = ");
    double Ki = checkInputDouble("Ki = ");
    double Kd = checkInputDouble("Kd = ");
    double dt;
    while (true){
        dt = checkInputDouble("dt (sampling step) = ");
        if (dt > 0) break;
        std::cout << "dt must be positive!" << std::endl;
    }
    double uMin;
    double uMax;
    while (true){
        uMin = checkInputDouble("PID uMin = ");
        uMax = checkInputDouble("PID uMax = ");
        if (uMin < uMax) break;
        std::cout << "uMin must be less than uMax!" << std::endl;
    }

    double w = checkInputDouble("Target value w (setpoint) = ");

    std::cout << "Choose simulation mode:" << std::endl;
    std::cout << " 1 - fixed number of steps (n)" << std::endl;
    std::cout << " 2 - until |e| < eps (steady state)" << std::endl;
    int mode = checkInputInt("Your choice: ", 1, 2);

    int n;
    double eps = 0;
    if (mode == 1){
        n = checkInputInt("Input number of steps (n): ", 1, 100000);
    } else {
        while (true){
            eps = checkInputDouble("eps (target accuracy) = ");
            if (eps > 0) break;
            std::cout << "eps must be positive!" << std::endl;
        }
        n = 100000;
    }

    PIDController pid(Kp, Ki, Kd, dt, uMin, uMax);

    std::cout << std::setw(5)  << "t"
              << std::setw(12) << "w"
              << std::setw(12) << "e(t)"
              << std::setw(12) << "u(t)"
              << std::setw(12) << "y(t)" << std::endl;
    std::cout << std::string(53, '-') << std::endl;

    std::ofstream file("pid_result.csv");
    if (!file){
        std::cout << "Error: cannot create pid_result.csv" << std::endl;
        return 1;
    }
    file << "t;w;e;u;y" << std::endl;
    file << std::fixed << std::setprecision(4);

    double y = 0;
    int steps = n;
    bool reached = false;
    for (int t = 0; t < n; t++){
        double e = w - y;
        double u = pid.step(w, y);
        y = plant->nextStep(u);
        std::cout << std::setw(5)  << t
                  << std::setw(12) << std::fixed << std::setprecision(4) << w
                  << std::setw(12) << e
                  << std::setw(12) << u
                  << std::setw(12) << y << std::endl;
        file << t << ";" << w << ";" << e << ";" << u << ";" << y << std::endl;
        if (mode == 2 && std::fabs(e) < eps){
            reached = true;
            steps = t + 1;
            break;
        }
    }
    file.close();

    std::cout << std::endl;
    if (mode == 2){
        if (reached)
            std::cout << "Steady state reached: |e| < " << eps << " in " << steps << " steps." << std::endl;
        else
            std::cout << "WARNING: |e| >= eps after " << n << " steps." << std::endl;
    }
    std::cout << "Data saved to pid_result.csv" << std::endl;
    std::cout << "Run 'python plot.py' to draw graphs." << std::endl;
    return 0;
}

double checkInputDouble(const std::string& text){
    double x;
    while(true){
        std::cout << text;
        if (std::cin >> x) return x;
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Incorrect input! Try another number." << std::endl;
    }
}

int checkInputInt(const std::string& text, int minVal, int maxVal){
    int x;
    while (true){
        std::cout << text;
        if ((std::cin >> x) && x >= minVal && x <= maxVal) return x;
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Enter integer from " << minVal << " to " << maxVal << "!" << std::endl;
    }
}

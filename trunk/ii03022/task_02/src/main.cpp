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
#include "InputSignal.h"
#include "StabilityAnalyzer.h"

double checkInputDouble(const std::string& text);
int checkInputInt(const std::string& text, int minVal, int maxVal);

int main(){
    std::remove("result.csv");
    std::cout << "Choose model:" << std::endl;
    std::cout << " 1 - Model 1.8 (Generalized Autoregressive Linear Model)" << std::endl;
    std::cout << " 2 - Model 2.2 (Actuator Saturation Non-linearity)" << std::endl;
    std::cout << " 3 - Model 3.6 (Cubic Growth and Control)" << std::endl;
    int user_choice = checkInputInt("Your choice: ", 1, 3);
    
    int n = checkInputInt("Input number of steps (n): ", 1, 100000);
    
    std::unique_ptr<Model> model;
    switch(user_choice){
        case 1:{
            std::cout << "Model 1.8: y(t+1) = a1*y(t) + a2*y(t-1) + b1*u(t) + b2*u(t-1)" << std::endl;
            double a1 = checkInputDouble("a1 = ");
            double a2 = checkInputDouble("a2 = ");
            double b1 = checkInputDouble("b1 = ");
            double b2 = checkInputDouble("b2 = ");
            model = std::make_unique<GeneralizedAutoregressiveLinearModel>(a1, a2, b1, b2);
            std::cout << std::endl;
            printStabilityReport(a1, a2, std::cout);
            std::cout << std::endl;
            if (!isStable(a1, a2)){
                std::cout << "Simulate unstable system? (Y/N): ";
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
                uMin = checkInputDouble("uMin = ");
                uMax = checkInputDouble("uMax = ");
                if (uMin < uMax) break;
                std::cout << "uMin must be less than uMax! Enter another numbers." << std::endl;
            }
            model = std::make_unique<ActuatorSaturationNonLinearityModel>(a, b, uMin, uMax);
            break;
        }
        case 3:{
            std::cout << "Model 3.6: dy/dt = a*y^3 + b*u (y = y + h*(a*y^3 + b*u))" << std::endl;
            double a = checkInputDouble("a = ");
            double b = checkInputDouble("b = ");
            double h;
            while (true){
                h = checkInputDouble("h (time step) = ");
                if (h > 0) break;
                std::cout << "h must be positive!" << std::endl;
            }
            model = std::make_unique<CubicGrowthAndControlModel>(a, b, h);
            break;
        }
        default: return 0;
    }
    
    std::cout << "Choose input signal u(t):" << std::endl;
    std::cout << " 1 - step     (u = const)" << std::endl;
    std::cout << " 2 - impulse  (u(0) = A, u(t > 0) = 0)" << std::endl;
    std::cout << " 3 - harmonic (u = A * sin(t))" << std::endl;
    int signal_type = checkInputInt(" Your choice: ", 1, 3);
    
    double amplitude = checkInputDouble("Input amplitude A: ");
    std::cout << std::endl;
    
    std::cout << std::setw(5)  << "t"
              << std::setw(15) << "u(t)"
              << std::setw(15) << "y(t)" << std::endl;
    std::cout << std::string(35, '-') << std::endl;
    
    std::ofstream file("result.csv");   
        if (!file){                        
            std::cout << "Error: cannot create result.csv" << std::endl;
            return 1;
        }
    
    file << "t;u;y" << std::endl;
    file << std::fixed << std::setprecision(4);
    
    for(int t = 0; t < n; t++){
        double u = inputSignal(t, signal_type, amplitude);
        double y = model -> nextStep(u);
        std::cout << std::setw(5)  << t
                  << std::setw(15) << std::fixed << std::setprecision(4) << u
                  << std::setw(15) << y << std::endl;
             file << t << ";" << u << ";" << y << std::endl;
    }
    file.close();
    std::cout << std::endl << "Data saved to result.csv" << std::endl;
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
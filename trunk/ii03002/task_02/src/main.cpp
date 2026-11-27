#include <iostream>
#include <memory>
#include <string>
#include "InputSignal.h"
#include "Model.h"
#include "Model1_9.h"
#include "Model2_1.h"
#include "Model3_3.h"
#include "StabilityAnalyzer.h"

int main() {
    int steps = 30;
    int model_choice = 1;
    int signal_choice = 1;
    std::cout << "Simulation steps (default 30): ";
    if (!(std::cin >> steps)) { std::cin.clear(); steps = 30; }
    std::cout << "Model: 1=Model1_9, 2=Model2_1, 3=Model3_3: ";
    if (!(std::cin >> model_choice)) return 1;
    std::cout << "Signal: 1=step, 2=impulse, 3=harmonic: ";
    if (!(std::cin >> signal_choice)) return 1;

    std::unique_ptr<Model> model;
    std::string label;
    if (model_choice == 1) {
        double a1;
        double a2;
        double a3;
        double b1;
        double y0;
        std::cout << "Enter a1 a2 a3 b1 y0: ";
        if (!(std::cin >> a1 >> a2 >> a3 >> b1 >> y0)) return 1;
        printStabilityReport(a1, a2, a3, std::cout);
        model = std::make_unique<Model1_9>(a1, a2, a3, b1, y0);
        label = "Model 1.9";
    } else if (model_choice == 2) {
        double a;
        double b;
        double c;
        double d;
        double y0;
        std::cout << "Enter a b c d y0: ";
        if (!(std::cin >> a >> b >> c >> d >> y0)) return 1;
        model = std::make_unique<Model2_1>(a, b, c, d, y0);
        label = "Model 2.1";
    } else if (model_choice == 3) {
        double a;
        double b;
        double dt;
        double y0;
        std::cout << "Enter a b dt y0: ";
        if (!(std::cin >> a >> b >> dt >> y0)) return 1;
        model = std::make_unique<Model3_3>(a, b, dt, y0);
        label = "Model 3.3";
    } else return 1;

    SignalType type = SignalType::STEP;
    switch (signal_choice) {
        case 2:
            type = SignalType::IMPULSE;
            break;
        case 3:
            type = SignalType::HARMONIC;
            break;
        default:
            type = SignalType::STEP;
            break;
    }
    std::cout << "\nstep,u,y\n";
    for (int k = 0; k < steps; ++k) {
        const double u = InputSignal::getSignal(type, k);
        std::cout << k << ',' << u << ',' << model->step(u) << '\n';
    }
    return 0;
}

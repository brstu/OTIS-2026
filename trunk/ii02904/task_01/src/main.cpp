#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>
#include <vector>
#include <filesystem>
#include "Models.hpp"
#include "Input.hpp"

using namespace std;

double ask(const string& text, double def) {
    cout << text << " [" << def << "]: ";
    string line;
    getline(cin, line);
    if (line.empty()) return def;
    return stod(line);
}

int main(int argc, char** argv) {
    filesystem::path outDir = (argc > 1) ? argv[1] : ".";
    filesystem::create_directories(outDir);

    cout << "=== Lab 1, variant 4: Model 1.4, Model 2.6, Model 3.8 ===\n";
    cout << "(Enter = значение по умолчанию)\n\n";

    int    n  = (int)ask("Число шагов симуляции n", 20);
    double u0 = ask("Значение ступенчатого воздействия", 1.0);

    cout << "\n-- Model 1.4 --\n";
    double a  = ask("a", 0.8);
    double b  = ask("b", 1.0);
    cout << "-- Model 2.6 --\n";
    double a1 = ask("a1", 0.1);
    double a2 = ask("a2", 0.5);
    double b2 = ask("b", 1.0);
    cout << "-- Model 3.8 --\n";
    double c1 = ask("a1", 1.0);
    double c2 = ask("a2", 0.5);
    double b3 = ask("b", 1.0);
    double dt = ask("dt", 0.1);

    vector<InputType> inputs = {InputType::Step, InputType::Impulse, InputType::Harmonic};

    for (InputType type : inputs) {
        // Создаём модели (полиморфизм: храним их как Model)
        vector<unique_ptr<Model>> models;
        models.push_back(make_unique<LinearModel14>(a, b));
        models.push_back(make_unique<NonlinearModel26>(a1, a2, b2));
        models.push_back(make_unique<DiffModel38>(c1, c2, b3, dt));

        cout << "\n########## Входное воздействие: " << inputName(type) << " ##########\n";

        ofstream csv(outDir / ("results_" + inputName(type) + ".csv"));
        csv << "tau,u,y_model_1_4,y_model_2_6,y_model_3_8\n";

        cout << fixed << setprecision(5);
        cout << setw(5) << "tau" << setw(12) << "u";
        for (auto& m : models) cout << setw(14) << m->name();
        cout << "\n";

        for (int tau = 0; tau <= n; tau++) {
            double u = inputValue(type, tau, u0);

            cout << setw(5) << tau << setw(12) << u;
            csv << tau << "," << u;
            for (auto& m : models) {
                cout << setw(14) << m->current();
                csv << "," << m->current();
            }
            cout << "\n";
            csv << "\n";

            // Считаем y_{tau+1}
            for (auto& m : models) m->nextStep(u);
        }
    }
    cout << "\nРезультаты сохранены в: " << outDir.string() << "\n";
    return 0;
}

#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>
#include <vector>
#include <filesystem>
#include "Models.hpp"
#include "Input.hpp"

using namespace std;

// Спросить число; если нажать Enter - берётся значение по умолчанию
double ask(const string& text, double def) {
    cout << text << " [" << def << "]: ";
    string line;
    getline(cin, line);
    if (line.empty()) return def;
    return stod(line);
}

int main(int argc, char** argv) {
    // Папка для результатов: первый аргумент (по умолчанию - текущая)
    filesystem::path outDir = (argc > 1) ? argv[1] : ".";
    filesystem::create_directories(outDir);

    cout << "=== Lab 1, variant 4: Model 1.4, Model 2.6, Model 3.8 ===\n";
    cout << "(Enter = значение по умолчанию)\n\n";

    // Выбор модели
    cout << "Какую модель показать?\n";
    cout << "  1 - Model 1.4 (линейная)\n";
    cout << "  2 - Model 2.6 (нелинейная)\n";
    cout << "  3 - Model 3.8 (дифференциальная)\n";
    cout << "  0 - все сразу\n";
    auto choice = static_cast<int>(ask("Ваш выбор", 0));
    if (choice < 0 || choice > 3) {
        cout << "Неверный выбор, показываю все модели.\n";
        choice = 0;
    }
    const bool use14 = (choice == 0 || choice == 1);
    const bool use26 = (choice == 0 || choice == 2);
    const bool use38 = (choice == 0 || choice == 3);

    const auto n = static_cast<int>(ask("Число шагов симуляции n", 20));
    const double u0 = ask("Значение ступенчатого воздействия", 1.0);

    // Параметры (значения по умолчанию, если модель не выбрана - не используются)
    double a = 0.8;
    double b = 1.0;
    double a1 = 0.1;
    double a2 = 0.5;
    double b2 = 1.0;
    double c1 = 1.0;
    double c2 = 0.5;
    double b3 = 1.0;
    double dt = 0.1;

    if (use14) {
        cout << "\n-- Model 1.4 --\n";
        a = ask("a", a);
        b = ask("b", b);
    }
    if (use26) {
        cout << "\n-- Model 2.6 --\n";
        a1 = ask("a1", a1);
        a2 = ask("a2", a2);
        b2 = ask("b", b2);
    }
    if (use38) {
        cout << "\n-- Model 3.8 --\n";
        c1 = ask("a1", c1);
        c2 = ask("a2", c2);
        b3 = ask("b", b3);
        dt = ask("dt", dt);
    }

    const vector inputs = {InputType::Step, InputType::Impulse, InputType::Harmonic};

    for (InputType type : inputs) {
        // Создаём только выбранные модели (полиморфизм: храним их как Model)
        vector<unique_ptr<Model>> models;
        if (use14) models.push_back(make_unique<LinearModel14>(a, b));
        if (use26) models.push_back(make_unique<NonlinearModel26>(a1, a2, b2));
        if (use38) models.push_back(make_unique<DiffModel38>(c1, c2, b3, dt));

        cout << "\n########## Входное воздействие: " << inputName(type) << " ##########\n";

        ofstream csv(outDir / ("results_" + inputName(type) + ".csv"));
        csv << "tau,u";
        for (const auto& m : models) csv << "," << m->name();
        csv << "\n";

        cout << fixed << setprecision(5);
        cout << setw(5) << "tau" << setw(12) << "u";
        for (const auto& m : models) cout << setw(14) << m->name();
        cout << "\n";

        for (int tau = 0; tau <= n; tau++) {
            double u = inputValue(type, tau, u0);

            // Выводим y_tau
            cout << setw(5) << tau << setw(12) << u;
            csv << tau << "," << u;
            for (const auto& m : models) {
                cout << setw(14) << m->current();
                csv << "," << m->current();
            }
            cout << "\n";
            csv << "\n";

            // Считаем y_{tau+1}
            for (const auto& m : models) m->nextStep(u);
        }
    }
    cout << "\nРезультаты сохранены в: " << outDir.string() << "\n";
    return 0;
}

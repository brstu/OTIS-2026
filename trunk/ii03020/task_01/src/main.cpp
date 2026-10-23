#include <iostream>
#include <string>
#include <vector>

#include "Linear_Model.h"
#include "Non_linear_Model.h"
#include "Simple_Differential_Equation.h"
#include "TableOutput.h"
#include "Plotter.h"

#ifdef _WIN32
  #include <windows.h>
#endif

using namespace std;

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    int    input_type, steps, plot_choice;
    double u_const = 0.0;

    cout << "Выберите входное воздействие:\n"
         << "  0 - ступенчатое (const)\n"
         << "  1 - импульсное (u0=1, uτ=0 при τ>0)\n"
         << "  2 - гармоническое sin(τ)\n"
         << "Ваш выбор: ";
    cin >> input_type;

    if (input_type == 0) {
        cout << "Введите постоянное значение u: ";
        cin >> u_const;
    }

    cout << "Введите количество шагов симуляции: ";
    cin >> steps;
    if (steps <= 0) {
        cerr << "Ошибка: количество шагов должно быть > 0\n";
        return 1;
    }

    cout << "Запустить gnuplot для построения графиков?\n"
         << "  1 - да (нужен gnuplot в PATH)\n"
         << "  0 - только сохранить .plt-скрипты\n"
         << "Ваш выбор: ";
    cin >> plot_choice;
    bool invoke = (plot_choice != 0);

    if (invoke && !Plotter::is_gnuplot_available()) {
        cerr << "Внимание: gnuplot не найден в PATH. "
                "Будут сохранены только .plt-скрипты.\n";
        invoke = false;
    }

    double a = 0.9, b = 0.5, c = 1.0, d = 2.0;
    double b1 = 1.0, b2 = 0.5, b3 = 0.25;

    bool ok = true;

    {
        Linear_Model m(a, b1, b2, b3, input_type, u_const);
        ok = run_and_report(m, steps, "Линейная модель", "linear_model.csv") && ok;
        Plotter::plot_csv("linear_model.csv",
                          "linear_model.plt",
                          "linear_model.png",
                          "Linear model", invoke);
    }
    {
        Non_linear_Model m(a, b, c, d, input_type, u_const);
        ok = run_and_report(m, steps, "Нелинейная модель", "nonlinear_model.csv") && ok;
        Plotter::plot_csv("nonlinear_model.csv",
                          "nonlinear_model.plt",
                          "nonlinear_model.png",
                          "Nonlinear model", invoke);
    }
    {
        Simple_Differential_Equation m(b, 0.1, input_type, u_const);
        ok = run_and_report(m, steps, "Дифференциальное уравнение", "sde_model.csv") && ok;
        Plotter::plot_csv("sde_model.csv",
                          "sde_model.plt",
                          "sde_model.png",
                          "Differential equation", invoke);
    }

    // Сводный график: y(step) всех трёх моделей на одной картинке
    Plotter::plot_comparison(
        {"linear_model.csv", "nonlinear_model.csv", "sde_model.csv"},
        {"Linear", "Nonlinear", "SDE"},
        "comparison.plt",
        "comparison.png",
        "Model comparison", invoke);

    cout << "\nГотово. Файлы результатов:\n"
         << "  linear_model.csv / .plt / .png\n"
         << "  nonlinear_model.csv / .plt / .png\n"
         << "  sde_model.csv / .plt / .png\n"
         << "  comparison.plt / .png\n";

    return ok ? 0 : 1;
}
#include <iostream>
#include <string>
#include <vector>
#include "Linear_Model.h"
#include "Non_Linear_Model.h"
#include "Simple_Differential_Equation.h"
#include "TableOutput.h"
#include "Plotter.h"
#include "StabilityAnalyzer.h"

#ifdef _WIN32
    #include <windows.h>
#endif

using namespace std;

int main() {

    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif

    int input_type;
    int steps;
    int model_type;
    double u_const = 0.0;

    cout << "Выберите входное воздействие (0-const, 1-импульс, 2-sin): ";
    cin >> input_type;
    if (input_type == 0) {
        cout << "Введите u: "; cin >> u_const;
    }
    cout << "Введите количество шагов: "; cin >> steps;

    cout << "Выберите тип модели:\n 0 - Линейная\n 1 - Нелинейная\n 2 - Дифференциальное\nВаш выбор: ";
    cin >> model_type;

    if (model_type == 0) {
        double a;
        double b1;
        double b2;
        double b3;
        cout << "Введите a, b1, b2, b3: ";
        cin >> a >> b1 >> b2 >> b3;

        // Дополнительное условие из ТЗ (п.2): Логическая проверка устойчивости
        if (!StabilityAnalyzer::isStable(a)) {
            cout << "\n[ВНИМАНИЕ] Коэффициент a = " << a 
                 << " приводит к расходящемуся (неустойчивому) процессу!\n" << endl;
        }

        Linear_Model m(a, b1, b2, b3, input_type, u_const);
        run_and_report(m, steps, "Линейная модель", "linear_model.csv");
        Plotter::plot_csv("linear_model.csv", "linear_model.plt", "linear_model.png", "Linear model", true);
    }
    else if (model_type == 1) {
        double a;
        double b;
        double c;
        double d;
        cout << "Введите a, b, c, d: ";
        cin >> a >> b >> c >> d;
        Non_linear_Model m(a, b, c, d, input_type, u_const);
        run_and_report(m, steps, "Нелинейная модель", "nonlinear_model.csv");
        Plotter::plot_csv("nonlinear_model.csv", "nonlinear_model.plt", "nonlinear_model.png", "Nonlinear model", true);
    }
    else if (model_type == 2) {
        double b;
        cout << "Введите b: ";
        cin >> b;
        Simple_Differential_Equation m(b, 0.1, input_type, u_const);
        run_and_report(m, steps, "Дифференциальное уравнение", "sde_model.csv");
        Plotter::plot_csv("sde_model.csv", "sde_model.plt", "sde_model.png", "Differential equation", true);
    }

    return 0;
}
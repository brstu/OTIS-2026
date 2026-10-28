#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <string>

using namespace std;

// Абстрактный базовый класс
class Model {
public:
    virtual double step(double u) = 0;
    virtual void reset() = 0;
    virtual ~Model() = default;
};

// ==================== Линейная модель 1.3 ====================
// y[τ+1] = a1*y[τ] + a2*y[τ-1] + b*u[τ]
class LinearModel : public Model {
private:
    double a1, a2, b;
    double y_prev = 0.0;  // y[τ-1]
    double y_curr = 0.0;  // y[τ]

public:
    LinearModel(double a1_, double a2_, double b_) : a1(a1_), a2(a2_), b(b_) {}

    double step(double u) override {
        double y_next = a1 * y_curr + a2 * y_prev + b * u;
        y_prev = y_curr;
        y_curr = y_next;
        return y_curr;
    }

    void reset() override {
        y_prev = 0.0;
        y_curr = 0.0;
    }

    // Проверка устойчивости (корни характеристического уравнения |z| < 1)
    // z² - a1*z - a2 = 0
    bool isStable() const {
        double disc = a1 * a1 + 4.0 * a2;
        if (disc >= 0.0) {
            double z1 = (a1 + sqrt(disc)) / 2.0;
            double z2 = (a1 - sqrt(disc)) / 2.0;
            return (fabs(z1) < 1.0) && (fabs(z2) < 1.0);
        } else {
            // Комплексные корни: |z| = sqrt(-a2)
            return sqrt(-a2) < 1.0;
        }
    }
};

// ==================== Нелинейная модель 2.5 ====================
// y[τ+1] = a*y[τ] + b * sign(u[τ]) * (1 - exp(-|u[τ]|))
class NonlinearModel : public Model {
private:
    double a, b;
    double y_curr = 0.0;

public:
    NonlinearModel(double a_, double b_) : a(a_), b(b_) {}

    double step(double u) override {
        double sign_u = (u > 0.0) ? 1.0 : ((u < 0.0) ? -1.0 : 0.0);
        y_curr = a * y_curr + b * sign_u * (1.0 - exp(-fabs(u)));
        return y_curr;
    }

    void reset() override {
        y_curr = 0.0;
    }
};

// ==================== Дифференциальная модель 3.7 ====================
// dy/dt = -exp(a*y) + b*u
// Метод Эйлера: y = y + (-exp(a*y) + b*u) * dt
class DifferentialModel : public Model {
private:
    double a, b, dt;
    double y_curr = 0.0;

public:
    DifferentialModel(double a_, double b_, double dt_) : a(a_), b(b_), dt(dt_) {}

    double step(double u) override {
        y_curr = y_curr + (-exp(a * y_curr) + b * u) * dt;
        return y_curr;
    }

    void reset() override {
        y_curr = 0.0;
    }
};

// Генерация входных сигналов
double generateSignal(int type, int tau) {
    switch (type) {
        case 0: return 1.0;                          // ступенчатое
        case 1: return (tau == 0) ? 1.0 : 0.0;        // импульсное
        case 2: return sin(static_cast<double>(tau)); // гармоническое
        default: return 0.0;
    }
}

// Запуск симуляции и запись в CSV
void runSimulation(Model* model, int n, int signalType, const string& filename) {
    ofstream fout(filename);
    fout << fixed << setprecision(6);

    model->reset();

    for (int tau = 0; tau < n; ++tau) {
        double u = generateSignal(signalType, tau);
        double y = model->step(u);
        fout << tau << "," << u << "," << y << "\n";
    }

    fout.close();
}

int main() {
    int n;
    cout << "Enter number of iterations: ";
    cin >> n;
    if (n <= 0) {
        cout << "Wrong value\n";
        return 1;
    }

    // ----- Линейная модель -----
    double a1, a2, b_lin;
    cout << "\nEnter linear coefficients (Model 1.3):\n";
    cout << "a1: "; cin >> a1;
    cout << "a2: "; cin >> a2;
    cout << "b: ";  cin >> b_lin;

    LinearModel lin(a1, a2, b_lin);
    if (!lin.isStable()) {
        cout << "\nWARNING: Divergent (unstable) process!\n";
    }

    runSimulation(&lin, n, 0, "linear_step.csv");
    runSimulation(&lin, n, 1, "linear_impulse.csv");
    runSimulation(&lin, n, 2, "linear_harmonic.csv");
    cout << "All linear processes have been completed\n";

    // ----- Нелинейная модель -----
    double a_non, b_non;
    cout << "\nEnter nonlinear coefficients (Model 2.5):\n";
    cout << "a: "; cin >> a_non;
    cout << "b: "; cin >> b_non;

    NonlinearModel nonlin(a_non, b_non);
    runSimulation(&nonlin, n, 0, "nonlinear_step.csv");
    runSimulation(&nonlin, n, 1, "nonlinear_impulse.csv");
    runSimulation(&nonlin, n, 2, "nonlinear_harmonic.csv");
    cout << "All nonlinear processes have been completed\n";

    // ----- Дифференциальная модель -----
    double a_dif, b_dif, dt;
    cout << "\nEnter differential coefficients (Model 3.7):\n";
    cout << "a: ";  cin >> a_dif;
    cout << "b: ";  cin >> b_dif;
    cout << "dt: "; cin >> dt;

    DifferentialModel dif(a_dif, b_dif, dt);
    runSimulation(&dif, n, 0, "dif_step.csv");
    runSimulation(&dif, n, 1, "dif_impulse.csv");
    runSimulation(&dif, n, 2, "dif_harmonic.csv");
    cout << "All differential processes have been completed\n";

    // Запуск gnuplot (путь под Windows, как в примере)
    system(R"("C:\Program Files\gnuplot\bin\gnuplot.exe" linear_step.gp)");
    system(R"("C:\Program Files\gnuplot\bin\gnuplot.exe" linear_impulse.gp)");
    system(R"("C:\Program Files\gnuplot\bin\gnuplot.exe" linear_harmonic.gp)");
    system(R"("C:\Program Files\gnuplot\bin\gnuplot.exe" nonlinear_step.gp)");
    system(R"("C:\Program Files\gnuplot\bin\gnuplot.exe" nonlinear_impulse.gp)");
    system(R"("C:\Program Files\gnuplot\bin\gnuplot.exe" nonlinear_harmonic.gp)");
    system(R"("C:\Program Files\gnuplot\bin\gnuplot.exe" dif_step.gp)");
    system(R"("C:\Program Files\gnuplot\bin\gnuplot.exe" dif_impulse.gp)");
    system(R"("C:\Program Files\gnuplot\bin\gnuplot.exe" dif_harmonic.gp)");

    // Открытие графиков
    system("plot_linear_step.png");
    system("plot_linear_impulse.png");
    system("plot_linear_harmonic.png");
    system("plot_nonlinear_step.png");
    system("plot_nonlinear_impulse.png");
    system("plot_nonlinear_harmonic.png");
    system("plot_dif_step.png");
    system("plot_dif_impulse.png");
    system("plot_dif_harmonic.png");

    return 0;
}

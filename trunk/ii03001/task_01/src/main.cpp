#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

enum class InputType { Step = 1, Impulse = 2, Harmonic = 3 };

double inputValue(InputType type, int t) {
    switch (type) {
    case InputType::Step: return 1.0;
    case InputType::Impulse: return t == 0 ? 1.0 : 0.0;
    case InputType::Harmonic: return sin(static_cast<double>(t));
    }
    return 0.0;
}

string inputName(InputType type) {
    if (type == InputType::Step) return "step";
    if (type == InputType::Impulse) return "impulse";
    return "harmonic";
}

class Model {
public:
    virtual ~Model() = default;
    virtual double next(double y, double u, int t) const = 0;
    virtual string name() const = 0;
    virtual bool hasLinearStabilityCheck() const { return false; }
    virtual bool isStable() const { return true; }
};

class FirstOrderModel : public Model {
    double a;
    double b;
public:
    FirstOrderModel(double aValue, double bValue) : a(aValue), b(bValue) {}
    double next(double y, double u, int) const override { return a * y + b * u; }
    string name() const override { return "Model 1.1: first-order inertial"; }
    bool hasLinearStabilityCheck() const override { return true; }
    bool isStable() const override { return fabs(a) < 1.0; }
};

double deadZone(double y, double delta) {
    if (y > delta) return y - delta;
    if (y < -delta) return y + delta;
    return 0.0;
}

class DeadZoneModel : public Model {
    double a;
    double b;
    double delta;
public:
    DeadZoneModel(double aValue, double bValue, double deltaValue)
        : a(aValue), b(bValue), delta(deltaValue) {
    }
    double next(double y, double, int) const override {
        return a * y + b * deadZone(y, delta);
    }
    string name() const override { return "Model 2.3: dead zone"; }
};

class HarmonicForceModel : public Model {
    double b;
    double dt;
public:
    HarmonicForceModel(double bValue, double dtValue) : b(bValue), dt(dtValue) {}
    // Euler discretization of dy/dt = b * sin(t).
    double next(double y, double, int t) const override {
        return y + dt * b * sin(static_cast<double>(t) * dt);
    }
    string name() const override { return "Model 3.5: harmonic driving force"; }
};

void simulate(const Model& model, InputType inputType, int n, double y0,
    const string& filename) {
    ofstream out(filename);
    if (!out) {
        cerr << "Cannot open output file: " << filename << '\n';
        return;
    }

    cout << "\n" << model.name() << " | input: " << inputName(inputType) << '\n';
    cout << left << setw(8) << "t" << setw(16) << "u(t)" << setw(16) << "y(t)" << '\n';
    cout << string(40, '-') << '\n';
    out << "t,u,y\n";
    double y = y0;
    for (int t = 0; t < n; ++t) {
        double u = inputValue(inputType, t);
        cout << left << setw(8) << t << setw(16) << fixed << setprecision(6)
            << u << setw(16) << y << '\n';
        out << t << ',' << setprecision(10) << u << ',' << y << '\n';
        y = model.next(y, u, t);
    }
    cout << "CSV saved to " << filename << "\n";
}

int main() {
    cout << "Discrete models laboratory\n";
    int n;
    cout << "Number of simulation steps n (1..100000): ";
    if (!(cin >> n) || n < 1 || n > 100000) {
        cerr << "Invalid n. Use an integer from 1 to 100000.\n";
        return 1;
    }

    double a;
    double b;
    double y0;
    cout << "Enter coefficients a and b for Model 1.1: ";
    cin >> a >> b;
    cout << "Initial value y0: ";
    cin >> y0;

    FirstOrderModel first(a, b);
    if (first.hasLinearStabilityCheck() && !first.isStable()) {
        cout << "WARNING: Model 1.1 is unstable for this coefficient: |a| >= 1.\n";
    }
    else {
        cout << "Model 1.1 stability condition |a| < 1 is satisfied.\n";
    }

    double a2;
    double b2;
    double delta;
    cout << "Enter coefficients a and b and dead-zone width delta for Model 2.3: ";
    cin >> a2 >> b2 >> delta;
    if (delta < 0) {
        cerr << "delta must be non-negative.\n";
        return 1;
    }

    double b3;
    double dt;
    cout << "Enter coefficient b and time step dt for Model 3.5 (dt > 0): ";
    cin >> b3 >> dt;
    if (dt <= 0) {
        cerr << "dt must be positive.\n";
        return 1;
    }

    DeadZoneModel deadZoneModel(a2, b2, delta);
    HarmonicForceModel harmonicModel(b3, dt);

    vector inputs = { InputType::Step, InputType::Impulse, InputType::Harmonic };
    for (InputType type : inputs) {
        simulate(first, type, n, y0, "model11_" + inputName(type) + ".csv");
        simulate(deadZoneModel, type, n, y0, "model23_" + inputName(type) + ".csv");
        simulate(harmonicModel, type, n, y0, "model35_" + inputName(type) + ".csv");
    }
    cout << "\nDone. Nine CSV files were created in the current directory.\n";
    return 0;
}
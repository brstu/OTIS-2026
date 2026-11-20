#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

constexpr int kTimeWidth = 6;
constexpr int kValueWidth = 12;
constexpr int kPreviewRows = 8;

#ifdef _WIN32
constexpr const char* kPythonCommands[] = { "python", "py -3" };
constexpr const char* kQuietErrors = " 2>nul";
#else
constexpr const char* kPythonCommands[] = { "python3", "python" };
constexpr const char* kQuietErrors = " 2>/dev/null";
#endif

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

// ---------- console helpers ----------

void stopOnEndOfInput() {
    cerr << "\nInput ended unexpectedly.\n";
    exit(1);
}

// Reads one number per prompt: a single Enter is enough.
double readDouble(const string& prompt) {
    double value = 0.0;
    cout << prompt;
    while (!(cin >> value)) {
        if (cin.eof()) stopOnEndOfInput();
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid number, try again: ";
    }
    return value;
}

int readInt(const string& prompt, int minValue, int maxValue) {
    int value = 0;
    cout << prompt;
    while (!(cin >> value) || value < minValue || value > maxValue) {
        if (cin.eof()) stopOnEndOfInput();
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter an integer from " << minValue << " to " << maxValue << ": ";
    }
    return value;
}

// Short number for the table: 4 decimals, scientific form for huge/tiny values.
string formatValue(double value) {
    ostringstream text;
    const double magnitude = fabs(value);
    if (magnitude >= 1e6 || (magnitude != 0.0 && magnitude < 1e-4)) {
        text << scientific << setprecision(3) << value;
    }
    else {
        text << fixed << setprecision(4) << value;
    }
    return text.str();
}

string tableBorder(size_t valueColumns) {
    string border = "+" + string(kTimeWidth + 2, '-');
    for (size_t i = 0; i < valueColumns; ++i) {
        border += "+" + string(kValueWidth + 2, '-');
    }
    return border + "+";
}

void printTableRow(const string& first, const vector<string>& cells) {
    cout << "| " << right << setw(kTimeWidth) << first << ' ';
    for (const string& cell : cells) {
        cout << "| " << right << setw(kValueWidth) << cell << ' ';
    }
    cout << "|\n";
}

void printBanner(const string& text) {
    const string frame(44, '=');
    cout << frame << '\n' << "  " << text << '\n' << frame << '\n';
}

void printMenuLine(const string& text) {
    cout << "| " << left << setw(40) << text << " |\n";
}

void printMenu() {
    const string border = "+" + string(42, '-') + "+";
    cout << '\n' << border << '\n';
    printMenuLine("Select a model");
    cout << border << '\n';
    printMenuLine("1  Model 1.1 - first-order inertial");
    printMenuLine("2  Model 2.3 - dead zone");
    printMenuLine("3  Model 3.5 - harmonic driving force");
    printMenuLine("0  Exit");
    cout << border << '\n';
}

// Prints only a short preview of the results: the first rows and the last one.
void printPreview(const string& title, const vector<string>& headers,
    const vector<vector<double>>& results, int n, const string& filename) {
    const string border = tableBorder(headers.size());
    const int innerWidth = static_cast<int>(border.size()) - 4;

    cout << "\n+" << string(border.size() - 2, '-') << "+\n";
    cout << "| " << left << setw(innerWidth) << title << " |\n";
    cout << border << '\n';
    printTableRow("t", headers);
    cout << border << '\n';

    auto rowCells = [&results](int t) {
        vector<string> cells;
        for (const auto& series : results) {
            cells.push_back(formatValue(series[static_cast<size_t>(t)]));
        }
        return cells;
    };

    const bool truncated = n > kPreviewRows + 1;
    const int shown = truncated ? kPreviewRows : n;
    for (int t = 0; t < shown; ++t) {
        printTableRow(to_string(t), rowCells(t));
    }
    if (truncated) {
        printTableRow("...", vector<string>(headers.size(), "..."));
        printTableRow(to_string(n - 1), rowCells(n - 1));
    }
    cout << border << '\n';
    if (truncated) {
        cout << "Preview: " << kPreviewRows << " of " << n << " steps. ";
    }
    cout << "Full data: " << filename << '\n';
}

// Simulates one model for all three inputs, saves ALL values to ONE csv file
// (t, y_step, y_impulse, y_harmonic) and prints only a short preview.
bool simulate(const Model& model, int n, double y0, const string& filename) {
    ofstream out(filename);
    if (!out) {
        cerr << "Cannot open output file: " << filename << '\n';
        return false;
    }

    const vector inputs = { InputType::Step, InputType::Impulse, InputType::Harmonic };

    vector<vector<double>> results;
    vector<string> headers;
    out << "t";
    for (InputType type : inputs) {
        vector<double> series;
        double y = y0;
        for (int t = 0; t < n; ++t) {
            series.push_back(y);
            y = model.next(y, inputValue(type, t), t);
        }
        results.push_back(series);
        headers.push_back("y(" + inputName(type) + ")");
        out << ",y_" << inputName(type);
    }
    out << '\n';

    out << setprecision(10);
    for (int t = 0; t < n; ++t) {
        out << t;
        for (const auto& series : results) {
            out << ',' << series[static_cast<size_t>(t)];
        }
        out << '\n';
    }

    printPreview(model.name(), headers, results, n, filename);
    return true;
}

// Builds the graph of the chosen model with plot_results.py (Python + matplotlib).
void showPlot(const string& csvFile) {
    cout << "\nOpening graph... (close the graph window to return to the menu)\n" << flush;
    for (const char* python : kPythonCommands) {
        const string command = string(python) + " plot_results.py " + csvFile + kQuietErrors;
        if (system(command.c_str()) == 0) {
            return;
        }
    }
    cerr << "Could not build the graph. Install Python 3 and matplotlib "
         << "(pip install matplotlib).\n";
}

// ---------- one function per model: ask parameters, simulate, plot ----------

void runFirstOrder(int n, double y0) {
    cout << "\n[ Model 1.1 ]  y(t+1) = a*y(t) + b*u(t)\n";
    const double a = readDouble("  a = ");
    const double b = readDouble("  b = ");

    const FirstOrderModel model(a, b);
    if (model.hasLinearStabilityCheck() && !model.isStable()) {
        cout << "  WARNING: Model 1.1 is unstable: |a| >= 1.\n";
    }
    else {
        cout << "  Stable: |a| < 1.\n";
    }

    if (simulate(model, n, y0, "model11.csv")) {
        showPlot("model11.csv");
    }
}

void runDeadZone(int n, double y0) {
    cout << "\n[ Model 2.3 ]  y(t+1) = a*y(t) + b*DZ(y(t), delta)\n";
    const double a = readDouble("  a = ");
    const double b = readDouble("  b = ");
    double delta = readDouble("  delta (>= 0) = ");
    while (delta < 0) {
        cout << "  delta must be non-negative.\n";
        delta = readDouble("  delta (>= 0) = ");
    }

    const DeadZoneModel model(a, b, delta);
    if (simulate(model, n, y0, "model23.csv")) {
        showPlot("model23.csv");
    }
}

void runHarmonicForce(int n, double y0) {
    cout << "\n[ Model 3.5 ]  dy/dt = b*sin(t), Euler method\n";
    const double b = readDouble("  b = ");
    double dt = readDouble("  dt (> 0) = ");
    while (dt <= 0) {
        cout << "  dt must be positive.\n";
        dt = readDouble("  dt (> 0) = ");
    }

    const HarmonicForceModel model(b, dt);
    if (simulate(model, n, y0, "model35.csv")) {
        showPlot("model35.csv");
    }
}

int main() {
    printBanner("Discrete models laboratory");
    const int n = readInt("Number of simulation steps n (1..100000): ", 1, 100000);
    const double y0 = readDouble("Initial value y0: ");

    int choice = -1;
    while (choice != 0) {
        printMenu();
        choice = readInt("Your choice (0-3): ", 0, 3);
        if (choice == 1) {
            runFirstOrder(n, y0);
        }
        else if (choice == 2) {
            runDeadZone(n, y0);
        }
        else if (choice == 3) {
            runHarmonicForce(n, y0);
        }
    }

    cout << "\nGoodbye.\n";
    return 0;
}

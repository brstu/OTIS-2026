#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "Models.hpp"
#include "Input.hpp"

using namespace std;

namespace {

double ask(const string& text, double def)
{
    cout << text << " [" << def << "]: ";
    string line;
    if (!getline(cin, line) || line.empty()) return def;   // Enter / EOF -> default
    try {
        return stod(line);
    } catch (const exception&) {
        cout << "  ! Некорректное число, беру значение по умолчанию.\n";
        return def;
    }
}

int askInt(const string& text, int def)
{
    return static_cast<int>(ask(text, static_cast<double>(def)));
}

struct Params
{
    // какие модели считаем
    bool use14 = true;
    bool use26 = true;
    bool use38 = true;

    // какие воздействия считаем (в выбранном порядке)
    vector<InputType> inputs = { InputType::Step, InputType::Impulse, InputType::Harmonic };

    int n = 20;
    InputParams in;   // параметры воздействий

    // Model 1.4
    double a = 0.8;
    double b = 1.0;
    // Model 2.6
    double a1 = 0.1;
    double a2 = 0.5;
    double b2 = 1.0;
    // Model 3.8
    double c1 = 1.0;
    double c2 = 0.5;
    double b3 = 1.0;
    double dt = 0.1;
};

bool hasInput(const Params& p, InputType t)
{
    return find(p.inputs.begin(), p.inputs.end(), t) != p.inputs.end();
}

int chooseModel()
{
    cout << "\nКакую модель показать?\n";
    cout << "  1 - Model 1.4 (линейная)\n";
    cout << "  2 - Model 2.6 (нелинейная)\n";
    cout << "  3 - Model 3.8 (дифференциальная)\n";
    cout << "  0 - все сразу\n";
    const int choice = askInt("Ваш выбор", 0);
    if (choice < 0 || choice > 3) {
        cout << "Неверный выбор, показываю все модели.\n";
        return 0;
    }
    return choice;
}

vector<InputType> chooseInputs()
{
    cout << "\nКакое воздействие показать?\n";
    for (InputType t : allInputTypes())
        cout << "  " << inputCode(t) << " - " << inputTitle(t) << "\n";
    cout << "  0 - все сразу\n";

    const int choice = askInt("Ваш выбор", 0);
    if (choice == 0)
        return { allInputTypes().begin(), allInputTypes().end() };

    InputType t;
    if (inputTypeFromCode(choice, t))
        return { t };

    cout << "Неверный выбор, показываю все воздействия.\n";
    return { allInputTypes().begin(), allInputTypes().end() };
}

Params readParams()
{
    Params p;

    const int choice = chooseModel();
    p.use14 = (choice == 0 || choice == 1);
    p.use26 = (choice == 0 || choice == 2);
    p.use38 = (choice == 0 || choice == 3);

    p.inputs = chooseInputs();

    p.n = askInt("\nЧисло шагов симуляции n", p.n);
    if (p.n < 1) {
        cout << "n должно быть >= 1, беру 1.\n";
        p.n = 1;
    }

    // параметры только тех воздействий, которые реально выбраны
    if (hasInput(p, InputType::Step)) {
        cout << "\n-- Воздействие: step --\n";
        p.in.stepValue = ask("Значение ступеньки u0", p.in.stepValue);
    }
    if (hasInput(p, InputType::Impulse)) {
        cout << "\n-- Воздействие: impulse --\n";
        p.in.impulseValue = ask("Амплитуда импульса u(0)", p.in.impulseValue);
    }
    if (hasInput(p, InputType::Harmonic)) {
        cout << "\n-- Воздействие: harmonic --\n";
        p.in.harmonicAmp  = ask("Амплитуда синусоиды", p.in.harmonicAmp);
        p.in.harmonicFreq = ask("Частота синусоиды (рад/шаг)", p.in.harmonicFreq);
    }

    if (p.use14) {
        cout << "\n-- Model 1.4 --\n";
        p.a = ask("a", p.a);
        p.b = ask("b", p.b);
    }
    if (p.use26) {
        cout << "\n-- Model 2.6 --\n";
        p.a1 = ask("a1", p.a1);
        p.a2 = ask("a2", p.a2);
        p.b2 = ask("b",  p.b2);
    }
    if (p.use38) {
        cout << "\n-- Model 3.8 --\n";
        p.c1 = ask("a1", p.c1);
        p.c2 = ask("a2", p.c2);
        p.b3 = ask("b",  p.b3);
        p.dt = ask("dt", p.dt);
    }
    return p;
}

vector<unique_ptr<Model>> createModels(const Params& p)
{
    vector<unique_ptr<Model>> models;
    if (p.use14) models.push_back(make_unique<LinearModel14>(p.a, p.b));
    if (p.use26) models.push_back(make_unique<NonlinearModel26>(p.a1, p.a2, p.b2));
    if (p.use38) models.push_back(make_unique<DiffModel38>(p.c1, p.c2, p.b3, p.dt));
    return models;
}

void writeHeader(const vector<unique_ptr<Model>>& models, ostream& csv)
{
    csv << "tau,u";
    cout << setw(5) << "= tau =" << setw(10) << "= u =";
    for (const auto& m : models) {
        csv << "," << m->name();
        cout << setw(14) << m->name();
    }
    csv << "\n";
    cout << "\n";
}

bool writeRow(const vector<unique_ptr<Model>>& models, ostream& csv, int tau, double u)
{
    cout << setw(5) << tau << setw(12) << u;
    csv << tau << "," << u;
    bool finite = true;
    for (const auto& m : models) {
        const double y = m->current();
        if (!isfinite(y)) finite = false;
        cout << setw(14) << y;
        csv << "," << y;
    }
    cout << "\n";
    csv << "\n";
    return finite;
}

filesystem::path simulate(InputType type, const Params& p, const filesystem::path& outDir)
{
    auto models = createModels(p);
    const filesystem::path csvPath = outDir / ("results_" + inputName(type) + ".csv");

    cout << "\n########## Входное воздействие: " << inputTitle(type) << " ##########\n";
    ofstream csv(csvPath);
    cout << fixed << setprecision(5);

    writeHeader(models, csv);

    bool allFinite = true;
    for (int tau = 0; tau <= p.n; ++tau) {
        const double u = inputValue(type, tau, p.in);
        allFinite &= writeRow(models, csv, tau, u);
        for (const auto& m : models) m->nextStep(u);
    }

    if (!allFinite)
        cout << "! Внимание: модель разошлась (inf/nan) — уменьшите параметры или dt.\n";

    return csvPath;
}

} // namespace

int main(int argc, char** argv)
{
    const filesystem::path outDir = (argc > 1) ? argv[1] : ".";
    filesystem::create_directories(outDir);

    cout << "=== Lab 1, variant 4: Model 1.4, Model 2.6, Model 3.8 ===\n";
    cout << "(Enter = значение по умолчанию)\n";

    const Params params = readParams();

    if (params.inputs.empty()) {
        cout << "Не выбрано ни одного воздействия.\n";
        return 1;
    }

    vector<filesystem::path> files;
    files.reserve(params.inputs.size());
    for (InputType type : params.inputs)
        files.push_back(simulate(type, params, outDir));

    // Манифест для plot.py: какие именно воздействия были посчитаны
    {
        ofstream manifest(outDir / "plot_inputs.txt", ios::trunc);
        for (InputType type : params.inputs)
            manifest << inputName(type) << "\n";
    }

    cout << "\nРезультаты сохранены в: " << outDir.string() << "\n";
    for (const auto& f : files)
        cout << "  - " << f.filename().string() << "\n";
    cout << "\nПостроить график:  python plot.py \"" << outDir.string() << "\"\n";
    return 0;
}
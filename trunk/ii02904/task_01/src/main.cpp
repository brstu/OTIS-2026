#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>
#include <vector>
#include <filesystem>
#include "Models.hpp"
#include "Input.hpp"
#include <span>

using namespace std;

double ask(const string& text, double def) {
    cout << text << " [" << def << "]: ";
    string line;
    getline(cin, line);
    if (line.empty()) return def;
    return stod(line);
}

namespace {

struct Params {
    bool use14 = true;
    bool use26 = true;
    bool use38 = true;
    int n = 20;
    double u0 = 1.0;
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

int chooseModel() {
    cout << "Какую модель показать?\n";
    cout << "  1 - Model 1.4 (линейная)\n";
    cout << "  2 - Model 2.6 (нелинейная)\n";
    cout << "  3 - Model 3.8 (дифференциальная)\n";
    cout << "  0 - все сразу\n";
    const auto choice = static_cast<int>(ask("Ваш выбор", 0));
    if (choice < 0 || choice > 3) {
        cout << "Неверный выбор, показываю все модели.\n";
        return 0;
    }
    return choice;
}

int choiseSimulation() {
    cout << "Какую симуляцию показать показать?\n";
    cout << "  1 - step\n";
    cout << "  2 - impulse\n";
    cout << "  3 - harmonic\n";
    cout << "  0 - все сразу\n";
    const auto choice = static_cast<int>(ask("Ваш выбор", 0));
    if (choice < 0 || choice > 3) {
        cout << "Неверный выбор, показываю все модели.\n";
        return 0;
    }
    return choice;
}

Params readParams() {
    Params p;
    const int choice = chooseModel();
    p.use14 = (choice == 0 || choice == 1);
    p.use26 = (choice == 0 || choice == 2);
    p.use38 = (choice == 0 || choice == 3);

    p.n = static_cast<int>(ask("Число шагов симуляции n", p.n));
    p.u0 = ask("Значение ступенчатого воздействия", p.u0);

    if (p.use14) {
        cout << "\n-- Model 1.4 --\n";
        p.a = ask("a", p.a);
        p.b = ask("b", p.b);
    }
    if (p.use26) {
        cout << "\n-- Model 2.6 --\n";
        p.a1 = ask("a1", p.a1);
        p.a2 = ask("a2", p.a2);
        p.b2 = ask("b", p.b2);
    }
    if (p.use38) {
        cout << "\n-- Model 3.8 --\n";
        p.c1 = ask("a1", p.c1);
        p.c2 = ask("a2", p.c2);
        p.b3 = ask("b", p.b3);
        p.dt = ask("dt", p.dt);
    }
    return p;
}

vector<unique_ptr<Model>> createModels(const Params& p) {
    vector<unique_ptr<Model>> models;
    if (p.use14) {
        models.push_back(make_unique<LinearModel14>(p.a, p.b));
    }
    if (p.use26) {
        models.push_back(make_unique<NonlinearModel26>(p.a1, p.a2, p.b2));
    }
    if (p.use38) {
        models.push_back(make_unique<DiffModel38>(p.c1, p.c2, p.b3, p.dt));
    }
    return models;
}

void writeHeader(const vector<unique_ptr<Model>>& models, ostream& csv) {
    csv << "tau,u";
    cout << setw(5) << "tau" << setw(12) << "u";
    for (const auto& m : models) {
        csv << "," << m->name();
        cout << setw(14) << m->name();
    }
    csv << "\n";
    cout << "\n";
}

void writeRow(const vector<unique_ptr<Model>>& models, ostream& csv, int tau, double u) {
    cout << setw(5) << tau << setw(12) << u;
    csv << tau << "," << u;
    for (const auto& m : models) {
        cout << setw(14) << m->current();
        csv << "," << m->current();
    }
    cout << "\n";
    csv << "\n";
}

void simulate(InputType type, const Params& p, const filesystem::path& outDir) {
    auto models = createModels(p);

    cout << "\n########## Входное воздействие: " << inputName(type) << " ##########\n";

    ofstream csv(outDir / ("results_" + inputName(type) + ".csv"));
    cout << fixed << setprecision(5);
    writeHeader(models, csv);

    for (int tau = 0; tau <= p.n; tau++) {
        const double u = inputValue(type, tau, p.u0);
        writeRow(models, csv, tau, u);

        for (const auto& m : models) {
            m->nextStep(u);
        }
    }
}

}  

int main(int argc, char** argv) {

    const filesystem::path outDir = (argc > 1) ? argv[1] : ".";
    filesystem::create_directories(outDir);

    cout << "=== Lab 1, variant 4: Model 1.4, Model 2.6, Model 3.8 ===\n";
    cout << "(Enter = значение по умолчанию)\n\n";

    const Params params = readParams();

    const vector inputs = {InputType::Step, InputType::Impulse, InputType::Harmonic};
    const int ch = choiseSimulation();
    if(ch != 0){
        for (InputType type : std::vector<InputType>(inputs.begin() + ch, inputs.begin() + ch + 1)){
            simulate(type, params, outDir);
        }
    }
    else{
    for (InputType type : inputs){
        simulate(type, params, outDir);
    }
    }
    cout << "\nРезультаты сохранены в: " << outDir.string() << "\n";
    return 0;
}

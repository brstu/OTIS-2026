#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "Model.h"
#include "Model1_5.h"
#include "Model2_9.h"
#include "Model3_3.h"
#include "Signals.h"

namespace {

constexpr int kMaxSteps = 100000;
const char* const kCsvName = "result.csv";

// ---------- Ввод данных с проверкой ----------

bool parseDouble(const std::string& s, double& out) {
    try {
        size_t pos = 0;
        out = std::stod(s, &pos);
        return pos == s.size();
    } catch (const std::invalid_argument&) {
        return false;
    } catch (const std::out_of_range&) {
        return false;
    }
}

bool parseInt(const std::string& s, int lo, int hi, int& out) {
    try {
        size_t pos = 0;
        const int v = std::stoi(s, &pos);
        if (pos != s.size() || v < lo || v > hi) {
            return false;
        }
        out = v;
        return true;
    } catch (const std::invalid_argument&) {
        return false;
    } catch (const std::out_of_range&) {
        return false;
    }
}

// Пустая строка = значение по умолчанию.
double readDouble(const std::string& prompt, double def) {
    while (true) {
        std::cout << prompt << " [" << def << "]: ";
        std::string s;
        std::getline(std::cin, s);
        if (s.empty()) {
            return def;
        }
        if (double v = 0.0; parseDouble(s, v)) {
            return v;
        }
        std::cout << "  Нужно число, попробуйте ещё раз.\n";
    }
}

int readInt(const std::string& prompt, int lo, int hi, int def) {
    while (true) {
        std::cout << prompt << " [" << def << "]: ";
        std::string s;
        std::getline(std::cin, s);
        if (s.empty()) {
            return def;
        }
        if (int v = 0; parseInt(s, lo, hi, v)) {
            return v;
        }
        std::cout << "  Нужно целое число от " << lo << " до " << hi << ".\n";
    }
}

// ---------- Создание моделей ----------

std::unique_ptr<Model> createModel1_5() {
    std::cout << "y(t+1) = a1*y(t) + a2*y(t-k) + b*u(t)\n";
    const double a1 = readDouble("a1", 0.5);
    const double a2 = readDouble("a2", 0.3);
    const double b = readDouble("b", 1.0);
    const int k = readInt("k (задержка, шагов)", 1, 1000, 2);
    const double y0 = readDouble("y0", 0.0);
    return std::make_unique<Model1_5>(a1, a2, b, k, y0);
}

std::unique_ptr<Model> createModel2_9() {
    std::cout << "y(t+1) = a*y(t) + b*sqrt(|u|)*sign(u)\n";
    const double a = readDouble("a", 0.7);
    const double b = readDouble("b", 1.0);
    const double y0 = readDouble("y0", 0.0);
    return std::make_unique<Model2_9>(a, b, y0);
}

std::unique_ptr<Model> createModel3_3() {
    std::cout << "dy/dt = -a*y + b*u  (метод Эйлера)\n";
    const double a = readDouble("a", 0.5);
    const double b = readDouble("b", 1.0);
    const double dt = readDouble("dt (шаг)", 0.1);
    const double y0 = readDouble("y0", 0.0);
    return std::make_unique<Model3_3>(a, b, dt, y0);
}

std::unique_ptr<Model> createModel(int choice) {
    switch (choice) {
        case 1:
            return createModel1_5();
        case 2:
            return createModel2_9();
        default:
            return createModel3_3();
    }
}

// ---------- Симуляция ----------

void printHeader() {
    std::cout << std::setw(6) << "tau" << std::setw(14) << "u" << std::setw(14) << "y" << "\n";
}

void printRow(int tau, double u, double y) {
    std::cout << std::setw(6) << tau << std::setw(14) << std::fixed << std::setprecision(5) << u
              << std::setw(14) << y << "\n";
}

// Считает n шагов, печатает таблицу в консоль и пишет result.csv (Step,Input,Output).
void simulate(Model& model, SignalType type, double amplitude, int n) {
    std::ofstream csv(kCsvName);
    if (!csv.is_open()) {
        std::cout << "[Ошибка] Не удалось создать файл " << kCsvName << "\n";
        return;
    }
    csv << "Step,Input,Output\n";
    printHeader();
    for (int tau = 0; tau < n; ++tau) {
        const double u = getInput(type, tau, amplitude);
        const double y = model.current();
        printRow(tau, u, y);
        csv << tau << "," << u << "," << y << "\n";
        model.calculateNext(u);
    }
    std::cout << "\nТаблица сохранена в " << kCsvName << "\n";
}

SignalType readSignal() {
    std::cout << "Входной сигнал: 1 - ступенчатый, 2 - импульсный, 3 - гармонический\n";
    return static_cast<SignalType>(readInt("Выбор", 1, 3, 1));
}

void runOnce(int choice) {
    auto model = createModel(choice);
    const SignalType type = readSignal();
    const double amplitude = readDouble("Амплитуда A", 1.0);
    const int n = readInt("Количество шагов n", 1, kMaxSteps, 20);
    if (!model->isStable()) {
        std::cout << "\n[ВНИМАНИЕ] Для этих коэффициентов устойчивость не гарантируется: "
                     "процесс может расходиться.\n";
    }
    std::cout << "\n" << model->getName() << ", сигнал: " << signalName(type) << "\n";
    simulate(*model, type, amplitude, n);
}

void printMenu() {
    std::cout << "\n=== ЛР1, вариант 15 ===\n"
              << "1 - Model 1.5 (линейная)\n"
              << "2 - Model 2.9 (нелинейная)\n"
              << "3 - Model 3.3 (дифф. уравнение)\n"
              << "0 - выход\n";
}

}  // namespace

int main() {
    while (true) {
        printMenu();
        const int choice = readInt("Выбор", 0, 3, 0);
        if (choice == 0) {
            break;
        }
        runOnce(choice);
        std::cout << "\nГрафик можно построить: python plot.py " << kCsvName << "\n";
    }
    return 0;
}

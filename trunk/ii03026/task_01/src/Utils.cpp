#include "Utils.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>

void saveCSV(const std::string& filename, const std::vector<double>& steps, const std::vector<double>& outputs) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл для записи: " << filename << std::endl;
        return;
    }

    file << "Step,Y\n";
    for (size_t i = 0; i < steps.size(); ++i) {
        file << steps[i] << "," << outputs[i] << "\n";
    }

    file.close();
}

void runSimulation(Model& model, const std::string& prefix, int n, int inputType, double customU) {
    std::cout << std::fixed << std::setprecision(4);

    std::vector<double> steps(n);
    std::vector<double> outputs(n);
    double y = 0.0;
    std::string filenameSuffix = "";

    std::cout << "\n========================================\n";
    std::cout << " Шаг (i) \t Вход (u) \t Выход (Y) \n";
    std::cout << "========================================\n";

    for (int i = 0; i < n; ++i) {
        double u = 0.0;

        switch (inputType) {
            case 1: // Ступенчатое
                u = (customU != 0.0) ? customU : 1.0;
                filenameSuffix = "_step.csv";
                break;
            case 2: // Импульсное
                u = (i == 0) ? ((customU != 0.0) ? customU : 1.0) : 0.0;
                filenameSuffix = "_pulse.csv";
                break;
            case 3: // Гармоническое
                u = ((customU != 0.0) ? customU : 1.0) * std::sin(static_cast<double>(i));
                filenameSuffix = "_harmonic.csv";
                break;
            default:
                u = 1.0;
                filenameSuffix = "_custom.csv";
                break;
        }

        y = model.calculateNextStep(u, y);
        steps[i] = i;
        outputs[i] = y;

        std::cout << "   " << i << " \t\t " << u << " \t\t " << y << "\n";
    }

    std::cout << "========================================\n";

    saveCSV(prefix + filenameSuffix, steps, outputs);
    std::cout << "Данные сохранены в файл: " << prefix + filenameSuffix << "\n";
}
#include <iostream>
#include <fstream>
#include <vector>
#include "Model1_5.h"
#include "Model2_9.h"
#include "Model3_3.h"
#include "Signals.h"

void saveResults(const std::string& filename, const std::vector<double>& time, const std::vector<double>& output) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[Ошибка] Не удалось открыть файл для записи: " << filename << std::endl;
        return;
    }

    file << "Time,Output\n";
    for (size_t i = 0; i < time.size() && i < output.size(); ++i) {
        file << time[i] << "," << output[i] << "\n";
    }
    file.close();
}

void runModel1_5() {
    std::cout << "Запуск Модели 1.5...\n";
    Model1_5 model;
    std::vector<double> time;
    std::vector<double> output;

    for (int i = 0; i < 100; ++i) {
        double t = i * 0.1;
        double u = stepSignal(t); 
        double y = model.update(u);

        time.push_back(t);
        output.push_back(y);
    }

    saveResults("result.csv", time, output);
}

// Запуск симуляции Модели 2.9
void runModel2_9() {
    std::cout << "Запуск Модели 2.9...\n";
    Model2_9 model;
    std::vector<double> time;
    std::vector<double> output;

    for (int i = 0; i < 100; ++i) {
        double t = i * 0.1;
        double u = stepSignal(t);
        double y = model.update(u);

        time.push_back(t);
        output.push_back(y);
    }

    saveResults("result.csv", time, output);
}

void runModel3_3() {
    std::cout << "Запуск Модели 3.3...\n";
    Model3_3 model;
    std::vector<double> time;
    std::vector<double> output;

    for (int i = 0; i < 100; ++i) {
        double t = i * 0.1;
        double u = stepSignal(t);
        double y = model.update(u);

        time.push_back(t);
        output.push_back(y);
    }

    saveResults("result.csv", time, output);
}

int main() {
    std::cout << "=== Лабораторная работа 1 (Вариант 15) ===\n";
    runModel1_5();
    std::cout << "Симуляция завершена. Данные сохранены в result.csv\n";
    return 0;
}
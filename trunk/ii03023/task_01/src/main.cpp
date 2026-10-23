#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>
#include <vector>
#include "Model1_6.h"
#include "Model2_10.h"
#include "Model3_4.h"

// Сохранение результатов в CSV-файл в корень папки task_01 (на уровень выше из src)
void saveToCsv(const std::string& filename, const std::vector<double>& stepRes, 
               const std::vector<double>& impulseRes, const std::vector<double>& harmonicRes) {
    // Сохраняем в папку task_01 (на уровень выше из src или из папки сборки)
    // Если exe запускается из task_01/build/Release, то до task_01 нужно подняться на 2 уровня: "../../"
    // Сделаем универсальный путь для сохранения в корень task_01:
    std::string fullPath = "../../" + filename; 
    
    std::ofstream file(fullPath);
    if (!file.is_open()) {
        // Попробуем на один уровень выше (если запуск из build)
        fullPath = "../" + filename;
        file.open(fullPath);
    }
    
    if (!file.is_open()) {
        fullPath = filename;
        file.open(fullPath);
    }

    file << "Step,StepInput,ImpulseInput,HarmonicInput\n";
    for (size_t i = 0; i < stepRes.size(); ++i) {
        file << i << "," << stepRes[i] << "," << impulseRes[i] << "," << harmonicRes[i] << "\n";
    }
    file.close();
    std::cout << "[INFO] CSV файл успешно сохранен в: " << fullPath << "\n";
}

// Вывод таблицы результатов в консоль
void printTable(const std::string& modelName, const std::vector<double>& stepRes, 
                const std::vector<double>& impulseRes, const std::vector<double>& harmonicRes) {
    std::cout << "\n=== Табличные результаты для " << modelName << " ===\n";
    std::cout << std::setw(6) << "Шаг" << " | " 
              << std::setw(12) << "Ступень" << " | " 
              << std::setw(12) << "Импульс" << " | " 
              << std::setw(12) << "Гармоника" << "\n";
    std::cout << "----------------------------------------------------\n";
    
    size_t limit = std::min(size_t(10), stepRes.size());
    for (size_t i = 0; i < limit; ++i) {
        std::cout << std::setw(6) << i << " | " 
                  << std::setw(12) << std::fixed << std::setprecision(4) << stepRes[i] << " | " 
                  << std::setw(12) << impulseRes[i] << " | " 
                  << std::setw(12) << harmonicRes[i] << "\n";
    }
    std::cout << "... (полные данные записаны в CSV-файл)\n";
}

int main() {
    // Настройка кодировки консоли для корректного отображения кириллицы в Windows
    system("chcp 65001 > nul");
    
    int steps = 50; // Настройка количества шагов симуляции (пункт 6 ТЗ)
    std::cout << "Введите количество шагов симуляции (по умолчанию 50): ";
    if (std::cin >> steps && steps <= 0) {
        steps = 50;
    }

    // Создаем экземпляры моделей через полиморфизм (абстрактный класс Model)
    std::unique_ptr<Model> model1 = std::make_unique<Model1_6>(0.5, 0.3, 0.1, 1.0);
    std::unique_ptr<Model> model2 = std::make_unique<Model2_10>(0.7, 0.2);
    std::unique_ptr<Model> model3 = std::make_unique<Model3_4>(0.1, 0.1);

    // Модель 1.6 (Динамическая система 3-го порядка)
    auto m1_step = model1->simulate(steps, InputType::STEP);
    auto m1_imp = model1->simulate(steps, InputType::IMPULSE);
    auto m1_harm = model1->simulate(steps, InputType::HARMONIC);
    printTable("Модель 1.6 (3-й порядок)", m1_step, m1_imp, m1_harm);
    saveToCsv("model_1_6_results.csv", m1_step, m1_imp, m1_harm);

    // Модель 2.10 (Нелинейное сглаживание гиперболическим тангенсом)
    auto m2_step = model2->simulate(steps, InputType::STEP);
    auto m2_imp = model2->simulate(steps, InputType::IMPULSE);
    auto m2_harm = model2->simulate(steps, InputType::HARMONIC);
    printTable("Модель 2.10 (Гиперб. тангенс)", m2_step, m2_imp, m2_harm);
    saveToCsv("model_2_10_results.csv", m2_step, m2_imp, m2_harm);

    // Модель 3.4 (Квадратичный самораспад)
    auto m3_step = model3->simulate(steps, InputType::STEP);
    auto m3_imp = model3->simulate(steps, InputType::IMPULSE);
    auto m3_harm = model3->simulate(steps, InputType::HARMONIC);
    printTable("Модель 3.4 (Квадратичный самораспад)", m3_step, m3_imp, m3_harm);
    saveToCsv("model_3_4_results.csv", m3_step, m3_imp, m3_harm);

    std::cout << "\n[SUCCESS] Все симуляции успешно завершены, файлы данных выгружены в task_01!\n";
    return 0;
}
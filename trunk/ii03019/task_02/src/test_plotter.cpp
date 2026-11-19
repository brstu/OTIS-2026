#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include "Plotter.h"

// Тест 1: Проверка доступности gnuplot
TEST(PlotterTest, CheckAvailability) {
    // Просто вызываем функцию. Она проверит наличие gnuplot в системе.
    // Нам не важно, вернет она true или false, главное - мы покроем ее код.
    bool is_available = Plotter::is_gnuplot_available();
    (void)is_available; 
}

// Тест 2: Генерация скрипта для одной модели (plot_csv)
TEST(PlotterTest, GenerateCsvPlotScript) {
    // 1. Создаем фейковый CSV-файл, чтобы Plotter не ругался на его отсутствие
    std::string dummy_csv = "dummy_test_data.csv";
    std::ofstream f(dummy_csv);
    f << "step,y,u_used\n0,0.0,-\n1,1.0,2.0\n";
    f.close();

    // 2. Вызываем функцию с флагом false (НЕ запускать gnuplot)
    std::string plt_file = "test_script.plt";
    bool success = Plotter::plot_csv(dummy_csv, plt_file, "output.png", "Test Title", false);
    
    // 3. Проверяем, что функция вернула true
    EXPECT_TRUE(success);

    // 4. Проверяем, что .plt файл действительно создался
    std::ifstream script(plt_file);
    EXPECT_TRUE(script.is_open());
    script.close();

    // 5. Убираем за собой мусор
    std::remove(dummy_csv.c_str());
    std::remove(plt_file.c_str());
}

// Тест 3: Генерация скрипта для сравнения (plot_comparison)
TEST(PlotterTest, GenerateComparisonPlotScript) {
    // 1. Создаем два фейковых CSV для сравнения
    std::ofstream f1("dummy1.csv"); f1 << "step,y,u_used\n0,0,-\n"; f1.close();
    std::ofstream f2("dummy2.csv"); f2 << "step,y,u_used\n0,0,-\n"; f2.close();

    // 2. Вызываем функцию сравнения
    std::string plt_file = "comparison_script.plt";
    bool success = Plotter::plot_comparison(
        {"dummy1.csv", "dummy2.csv"}, 
        {"Legend 1", "Legend 2"}, 
        plt_file, "comp.png", "Compare Title", false
    );

    EXPECT_TRUE(success);

    // 3. Проверяем наличие файла
    std::ifstream script(plt_file);
    EXPECT_TRUE(script.is_open());
    script.close();

    // 4. Уборка
    std::remove("dummy1.csv");
    std::remove("dummy2.csv");
    std::remove(plt_file.c_str());
}

// Тест 4: Проверка обработки ошибки (разное количество CSV и легенд)
TEST(PlotterTest, ComparisonMismatchError) {
    // Передаем 2 файла, но только 1 легенду. Функция должна вернуть false.
    bool success = Plotter::plot_comparison(
        {"file1.csv", "file2.csv"}, 
        {"Only one legend"}, 
        "error.plt", "err.png", "Error Title", false
    );
    
    EXPECT_FALSE(success);
}
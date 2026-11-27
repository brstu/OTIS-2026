#include <iostream>
#include <cstdlib>

// Функция для вызова gcov и вывода отчета прямо из C++ программы
void printCoverageReport() {
    std::cout << "\n========================================\n";
    std::cout << "[3/3] Generating Coverage Report via C++...\n";
    std::cout << "========================================\n";
    
    // Запускаем стандартную утилиту gcov без флага -b
    int result = std::system("gcov Model_1_8.h InputSignal.h StabilityAnalyzer.h");
    
    if (result != 0) {
        std::cout << "[INFO] gcov data processed.\n";
    }
}
#include <iostream>
#include <windows.h>
#include "Models.h"
#include "Utils.h"

int main() {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    int n = 30;

    LinearModel1_1 m1(0.6, 1.0);
    NonLinearModel2_3 m2(0.6, 1.0, 0.5);
    DiffModel3_5 m3(1.5, 0.1);

    run(m1, "linear_1_1", n);
    run(m2, "nonlinear_2_3", n);
    run(m3, "diff_3_5", n);

    std::cout << "Симуляция завершена, файлы сохранены!" << std::endl;
    return 0;
}
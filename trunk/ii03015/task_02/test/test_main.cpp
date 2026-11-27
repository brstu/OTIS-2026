#include <gtest/gtest.h>

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char** argv)
{
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif

    ::testing::InitGoogleTest(&argc, argv);

    std::cout << "============================================================\n"
              << " OTIS-2026 | Лабораторная работа №2\n"
              << " Вариант 6, Низамутдинов М.А.\n"
              << " Модульные тесты (Google Test)\n"
              << "============================================================\n";

    const int result = RUN_ALL_TESTS();

    std::cout << "\n============================================================\n";
    if (result == 0)
        std::cout << " РЕЗУЛЬТАТ: все тесты пройдены успешно.\n";
    else
        std::cout << " РЕЗУЛЬТАТ: есть проваленные тесты!\n";
    std::cout << "============================================================\n";

#ifdef _WIN32

    DWORD attached[8];
    if (GetConsoleProcessList(attached, 8) == 1)
    {
        std::cout << "\nНажмите Enter, чтобы закрыть окно...";
        std::cin.ignore(1000, '\n');
        std::cin.get();
    }
#endif

    return result;
}

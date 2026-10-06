#pragma once

#include <memory>
#include "DynamicProcess.h"

// =====================================================================
// ProcessFactory — фабрика для создания моделей по номеру варианта.
// Возвращает unique_ptr на базовый класс DynamicProcess.
// =====================================================================
class ProcessFactory
{
public:
    // choice — номер модели (1, 2 или 3).
    // p1..p4 — коэффициенты, зависящие от выбранной модели.
    static std::unique_ptr<DynamicProcess> create(
        int choice, double p1, double p2, double p3, double p4);
};

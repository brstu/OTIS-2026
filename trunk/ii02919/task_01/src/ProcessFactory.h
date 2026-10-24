#pragma once

#include "DynamicProcess.h"
#include <memory>

// ============================================================================
//  ProcessFactory — фабрика динамических процессов.
//  Создаёт нужную модель по её типу (1, 2 или 3).
// ============================================================================
class ProcessFactory
{
public:
    // Создать процесс указанного типа.
    // type = 1 → MultiStepModel (Model 1.7)
    // type = 2 → QuadraticFeedback (Model 2.1)
    // type = 3 → HarmonicODE (Model 3.5)
    // Возвращает nullptr, если тип неизвестен.
    static std::unique_ptr<DynamicProcess> create(int type,
                                                   double p1,
                                                   double p2,
                                                   double p3 = 0.0,
                                                   double p4 = 0.0);
};

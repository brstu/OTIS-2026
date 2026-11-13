#pragma once

#include <iosfwd>
#include <vector>

#include "InputSignals.h"
#include "Model.h"

// Одна строка результата: такт tau, вход u_tau, выход y_tau (после такта).
struct Step {
    int tau;
    double u;
    double y;
};

// Чистая симуляция без ввода-вывода: reset модели, затем n тактов tau = 1..n.
// Бросает std::invalid_argument при n < 1.
std::vector<Step> simulate(Model& model, const InputSignal& signal, int n);

// Печатает предупреждение об устойчивости ДО симуляции.
// Возвращает true, если модель неустойчива и предупреждение выведено.
bool printStabilityWarning(const Model& model, std::ostream& out);

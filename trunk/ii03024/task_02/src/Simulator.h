#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>

#include "InputSignal.h"
#include "model.h"  // базовый класс Model из ЛР №1

struct Sample {
    int tau;
    double u;
    double y;
};

// Прогоняет модель по шагам tau = 0..n включительно (как в ЛР №1) и возвращает таблицу (tau, u, y).
std::vector<Sample> simulate(Model& model, const ISignal& signal, int n, double dt);

#endif

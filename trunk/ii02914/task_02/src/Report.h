#pragma once

#include <iosfwd>
#include <string>
#include <vector>

#include "Simulation.h"

// Табличный вывод результатов симуляции.
void printTable(std::ostream& out, const std::vector<Step>& steps);

// Сохранение в CSV (tau,u_tau,y_tau). false, если файл не удалось открыть/записать.
bool saveCsv(const std::string& path, const std::vector<Step>& steps);

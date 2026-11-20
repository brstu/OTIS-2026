#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include "Model.h"

void saveCSV(const std::string& filename, const std::vector<double>& steps, const std::vector<double>& outputs);
void runSimulation(Model& model, const std::string& prefix, int n, int inputType, double customU);

#endif
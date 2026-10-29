#ifndef UTILS_H
#define UTILS_H

#include "Model.h"
#include <string>
#include <vector>

void saveCSV(const std::string& filename, const std::vector<double>& y);
void run(Model& m, const std::string& name, int n);

#endif // UTILS_H
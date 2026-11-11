#include "Utils.h"
#include <fstream>
#include <cmath>

void saveCSV(const std::string& filename, const std::vector<double>& y) {
    std::ofstream file(filename);
    file << "Step,Y\n";
    for (size_t i = 0; i < y.size(); ++i) {
        file << i << "," << y[i] << "\n";
    }
}

void run(Model& m, const std::string& name, int n) {
    std::vector<double> y_step(n, 0.0), y_pulse(n, 0.0), y_harm(n, 0.0);
    double cur1 = 0, cur2 = 0, cur3 = 0;

    for (int i = 0; i < n; ++i) {
        cur1 = m.calculateNextStep(2.0, cur1);
        y_step[i] = cur1;

        double u_p = (i == 0) ? 1.0 : 0.0;
        cur2 = m.calculateNextStep(u_p, cur2);
        y_pulse[i] = cur2;

        cur3 = m.calculateNextStep(std::sin(i), cur3);
        y_harm[i] = cur3;
    }

    saveCSV(name + "_step.csv", y_step);
    saveCSV(name + "_pulse.csv", y_pulse);
    saveCSV(name + "_harmonic.csv", y_harm);
}
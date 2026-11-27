#ifndef STABILITY_ANALYZER_H
#define STABILITY_ANALYZER_H

#include <string>


class StabilityAnalyzer {
public:
    static bool model11Stable(double a, double epsilon = 1e-12);
    static bool model28LocallyStable(double a, double epsilon = 1e-12);
    static bool model36LocallyAsymptoticallyStable(double dt, double a, double epsilon = 1e-12);
    static std::string model11Message(double a);
};

#endif

#ifndef STABILITY_ANALYZER_H
#define STABILITY_ANALYZER_H

#include <complex>
#include <iosfwd>
#include <string>
#include <vector>

enum class StabilityStatus {
    STABLE,
    MARGINALLY_STABLE,
    UNSTABLE
};

struct StabilityReport {
    StabilityStatus status = StabilityStatus::STABLE;
    double max_root_modulus = 0.0;
    std::vector<std::complex<double>> roots;
    std::string warning_message;

    bool isStable() const {
        return status == StabilityStatus::STABLE;
    }
};

class StabilityAnalyzer {
public:
    static constexpr double kUnitCircleEps = 1e-9;

    static std::vector<std::complex<double>> findCubicRoots(double a1, double a2, double a3);
    static StabilityReport analyzeModel1_6(double a1, double a2, double a3);
    static StabilityReport analyzeModel3_7(double a, double dt);
    static bool checkAndWarnIfUnstable(const StabilityReport& report, std::ostream& out);
};

#endif // STABILITY_ANALYZER_H
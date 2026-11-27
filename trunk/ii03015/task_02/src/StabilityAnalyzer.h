#pragma once

#include <complex>
#include <iosfwd>
#include <string>
#include <vector>

enum class StabilityVerdict
{
    Stable,
    Marginal,
    Unstable
};

struct StabilityReport
{
    std::string modelName;
    std::vector<double> coefficients;
    std::vector<std::complex<double>> roots;
    double spectralRadius = 0.0;
    StabilityVerdict verdict = StabilityVerdict::Stable;
};

class StabilityAnalyzer
{
public:
    static std::vector<std::complex<double>> findRoots(const std::vector<double>& coefficients);
    static StabilityVerdict schurVerdict(const std::vector<double>& coefficients);
    static bool isStable(const std::vector<double>& coefficients);
    static bool needsWarning(const StabilityReport& report);

    static double spectralRadius(const std::vector<std::complex<double>>& roots);

    static StabilityReport analyzeModel1_6(double a1, double a2, double a3);
    static std::vector<double> characteristicPolynomial1_6(double a1, double a2, double a3);

    static bool isMarginallyStable(const std::vector<double>& coefficients);

    static std::string describe(const StabilityReport& report);

    static std::string warningText(const StabilityReport& report);

    static bool printWarningIfNeeded(const StabilityReport& report, std::ostream& os);
};

#include "models.h"

#include "StabilityAnalyzer.h"

bool Model::isStable() const {
    const std::vector<double> polynomial = characteristicPolynomial();

    if (polynomial.empty()) {
        return true;
    }

    return stability::isStable(polynomial);
}

std::string Model::stabilityWarning() const {
    const std::vector<double> polynomial = characteristicPolynomial();

    if (polynomial.empty()) {
        return "";
    }

    return stability::makeWarning(name(), stability::analyze(polynomial));
}

#include "StabilityAnalyzer.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace stability {

namespace {

using Complex = std::complex<double>;

std::vector<Complex> solveLinear(const std::vector<double>& c) {
    return { Complex(-c[1] / c[0], 0.0) };
}

std::vector<Complex> solveQuadratic(const std::vector<double>& c) {
    const double a = c[0];
    const double b = c[1];
    const double d = c[2];

    const Complex disc = std::sqrt(Complex(b * b - 4.0 * a * d, 0.0));

    return {
        (-b + disc) / (2.0 * a),
        (-b - disc) / (2.0 * a)
    };
}

// Метод Дюрана-Кернера (Вейерштрасса) для степени >= 3
std::vector<Complex> solveDurandKerner(const std::vector<double>& c) {
    const std::size_t n = c.size() - 1;

    // Приводим к монической форме
    std::vector<Complex> p(c.size());
    for (std::size_t i = 0; i < c.size(); ++i) {
        p[i] = c[i] / c[0];
    }

    auto eval = [&p](const Complex& z) {
        Complex result = p[0];
        for (std::size_t i = 1; i < p.size(); ++i) {
            result = result * z + p[i];
        }
        return result;
    };

    // Начальные приближения: степени числа (0.4 + 0.9i)
    std::vector<Complex> roots(n);
    const Complex seed(0.4, 0.9);
    Complex current(1.0, 0.0);
    for (std::size_t i = 0; i < n; ++i) {
        roots[i] = current;
        current *= seed;
    }

    const int maxIterations = 2000;
    for (int iter = 0; iter < maxIterations; ++iter) {
        double maxDelta = 0.0;

        for (std::size_t i = 0; i < n; ++i) {
            Complex denominator(1.0, 0.0);
            for (std::size_t j = 0; j < n; ++j) {
                if (j != i) {
                    denominator *= (roots[i] - roots[j]);
                }
            }

            const Complex delta = eval(roots[i]) / denominator;
            roots[i] -= delta;
            maxDelta = std::max(maxDelta, std::abs(delta));
        }

        if (maxDelta < 1e-14) {
            break;
        }
    }

    return roots;
}

} // namespace

std::vector<std::complex<double>> findRoots(const std::vector<double>& coeffs) {
    if (coeffs.size() < 2) {
        throw std::invalid_argument(
            "Polynomial must have degree >= 1 (at least two coefficients).");
    }

    for (double value : coeffs) {
        if (!std::isfinite(value)) {
            throw std::invalid_argument("Polynomial coefficients must be finite.");
        }
    }

    if (coeffs.front() == 0.0) {
        throw std::invalid_argument("Leading coefficient must not be zero.");
    }

    if (coeffs.size() == 2) {
        return solveLinear(coeffs);
    }

    if (coeffs.size() == 3) {
        return solveQuadratic(coeffs);
    }

    return solveDurandKerner(coeffs);
}

double spectralRadius(const std::vector<std::complex<double>>& roots) {
    double radius = 0.0;
    for (const auto& root : roots) {
        radius = std::max(radius, std::abs(root));
    }
    return radius;
}

Report analyze(const std::vector<double>& coeffs, double tolerance) {
    Report report;
    report.roots = findRoots(coeffs);
    report.maxModulus = spectralRadius(report.roots);
    report.stable = report.maxModulus < 1.0 - tolerance;
    return report;
}

bool isStable(const std::vector<double>& coeffs, double tolerance) {
    return analyze(coeffs, tolerance).stable;
}

std::string makeWarning(const std::string& modelName, const Report& report) {
    if (report.stable) {
        return "";
    }

    std::ostringstream out;
    out << std::fixed << std::setprecision(4)
        << "WARNING: " << modelName
        << " is unstable (or on the stability boundary): max |z| = "
        << report.maxModulus << " >= 1. The simulation may diverge.";
    return out.str();
}

} // namespace stability

#include "StabilityAnalyzer.h"

#include <algorithm>
#include <cmath>
#include <ostream>

namespace {
constexpr double kPi = 3.14159265358979323846;
constexpr double kEps = 1e-12;

StabilityStatus classifyModulus(
    double modulus,
    double eps,
    std::string& warning_out,
    const char* unstable_warning,
    const char* boundary_warning) {
    if (modulus > 1.0 + eps) {
        warning_out = unstable_warning;
        return StabilityStatus::UNSTABLE;
    }
    if (std::abs(modulus - 1.0) <= eps) {
        warning_out = boundary_warning;
        return StabilityStatus::MARGINALLY_STABLE;
    }
    warning_out.clear();
    return StabilityStatus::STABLE;
}
} // namespace

std::vector<std::complex<double>> StabilityAnalyzer::findCubicRoots(double a1, double a2, double a3) {
    const double shift = a1 / 3.0;
    const double a1_sq = a1 * a1;
    const double p = -a2 - a1_sq / 3.0;
    const double q = -a3 - (a1 * a2) / 3.0 - (2.0 * a1_sq * a1) / 27.0;
    const double discriminant = (q * q) / 4.0 + (p * p * p) / 27.0;

    std::vector<std::complex<double>> roots;
    roots.reserve(3);

    if (discriminant > kEps) {
        const double sqrt_d = std::sqrt(discriminant);
        const double u = std::cbrt(-0.5 * q + sqrt_d);
        const double v = std::cbrt(-0.5 * q - sqrt_d);
        const double real_sum = u + v;
        const double imag_part = (std::sqrt(3.0) * 0.5) * (u - v);

        roots.emplace_back(real_sum + shift, 0.0);
        roots.emplace_back(-0.5 * real_sum + shift, imag_part);
        roots.emplace_back(-0.5 * real_sum + shift, -imag_part);
    } else if (std::abs(discriminant) <= kEps) {
        if (std::abs(p) <= kEps && std::abs(q) <= kEps) {
            roots.emplace_back(shift, 0.0);
            roots.emplace_back(shift, 0.0);
            roots.emplace_back(shift, 0.0);
        } else {
            const double u = std::cbrt(-0.5 * q);
            roots.emplace_back(2.0 * u + shift, 0.0);
            roots.emplace_back(-u + shift, 0.0);
            roots.emplace_back(-u + shift, 0.0);
        }
    } else {
        const double radius = 2.0 * std::sqrt(-p / 3.0);
        const double denom = std::sqrt((-p * p * p) / 27.0);
        const double cos_arg = std::clamp((-0.5 * q) / denom, -1.0, 1.0);
        const double phi = std::acos(cos_arg);

        for (int k = 0; k < 3; ++k) {
            const double angle = (phi + 2.0 * kPi * static_cast<double>(k)) / 3.0;
            roots.emplace_back(radius * std::cos(angle) + shift, 0.0);
        }
    }

    return roots;
} // LCOV_EXCL_LINE

StabilityReport StabilityAnalyzer::analyzeModel1_6(double a1, double a2, double a3) {
    StabilityReport report;
    report.roots = findCubicRoots(a1, a2, a3);

    double max_mod = 0.0;
    for (const auto& r : report.roots) {
        const double mod = std::abs(r);
        if (mod > max_mod) {
            max_mod = mod;
        }
    }
    report.max_root_modulus = max_mod;
    report.status = classifyModulus(
        max_mod,
        kUnitCircleEps,
        report.warning_message,
        "WARNING [StabilityAnalyzer]: Model 1.6 is UNSTABLE! (> 1.0). Process will diverge.",
        "WARNING [StabilityAnalyzer]: Model 1.6 is on the stability boundary (|z| = 1.0). "
        "Oscillations may persist or grow if roots have multiplicity > 1.");

    return report;
} // LCOV_EXCL_LINE

StabilityReport StabilityAnalyzer::analyzeModel3_7(double a, double dt) {
    StabilityReport report;
    const double z = 1.0 - dt * std::exp(a);
    report.roots.emplace_back(z, 0.0);
    report.max_root_modulus = std::abs(z);
    report.status = classifyModulus(
        report.max_root_modulus,
        kUnitCircleEps,
        report.warning_message,
        "WARNING [StabilityAnalyzer]: Model 3.7 Euler step is UNSTABLE! (> 1.0).",
        "WARNING [StabilityAnalyzer]: Model 3.7 Euler step is on stability boundary |z| = 1.0.");

    return report;
} // LCOV_EXCL_LINE

bool StabilityAnalyzer::checkAndWarnIfUnstable(const StabilityReport& report, std::ostream& out) {
    if (!report.isStable()) {
        out << "\n" << report.warning_message << "\n";
        return false;
    }
    return true;
}
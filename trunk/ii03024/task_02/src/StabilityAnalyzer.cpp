#include "StabilityAnalyzer.h"

#include <cmath>
#include <ostream>
#include <sstream>
#include <stdexcept>

double StabilityAnalyzer::characteristicRoot(double a) {
    return a;  // z - a = 0
}

Stability StabilityAnalyzer::classify(double a, double eps) {
    if (!std::isfinite(a)) return Stability::Unstable;

    const double r = std::fabs(characteristicRoot(a));
    if (std::fabs(r - 1.0) <= eps) return Stability::Marginal;
    return (r < 1.0) ? Stability::Stable : Stability::Unstable;
}

bool StabilityAnalyzer::isStable(double a, double eps) {
    return classify(a, eps) == Stability::Stable;
}

std::string StabilityAnalyzer::warningMessage(double a, double eps) {
    std::ostringstream s;
    switch (classify(a, eps)) {
    case Stability::Stable:
        return "";
    case Stability::Marginal:
        s << "ПРЕДУПРЕЖДЕНИЕ: корень z = " << a
          << " лежит на единичной окружности (|z| = 1), система на границе устойчивости, "
             "процесс не затухает.";
        break;
    case Stability::Unstable:
        s << "ПРЕДУПРЕЖДЕНИЕ: корень z = " << a
          << " лежит вне единичной окружности (|z| > 1), система неустойчива, "
             "процесс будет расходиться.";
        break;
    }
    return s.str();
}

bool StabilityAnalyzer::warnIfUnstable(double a, std::ostream& out, double eps) {
    const std::string msg = warningMessage(a, eps);
    if (msg.empty()) return true;
    out << msg << "\n";
    return false;
}

void StabilityAnalyzer::requireStable(double a, double eps) {
    const std::string msg = warningMessage(a, eps);
    if (!msg.empty()) throw std::domain_error(msg);
}

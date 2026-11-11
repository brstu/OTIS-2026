#include "StabilityAnalyzer.h"

#include <cmath>
#include <sstream>
#include <iomanip>

bool StabilityAnalyzer::isStableFirstOrder(double a) {
    return std::fabs(a) < 1.0;
}

std::string StabilityAnalyzer::verdict(double a) {
    std::ostringstream os;
    os << std::fixed << std::setprecision(3);
    os << "|a| = " << std::fabs(a);
    if (isStableFirstOrder(a)) {
        os << " < 1 — процесс сходится (модель устойчива)";
    } else {
        os << " >= 1 — процесс расходится (модель НЕУСТОЙЧИВА)";
    }
    return os.str();
}

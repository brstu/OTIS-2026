#include "StabilityAnalyzer.h"
#include <cmath>

bool StabilityAnalyzer::isStable(
    const std::vector<double>& outputHistory,
    double /*tolerance*/
) const
{
    const double MAX_VALUE = 1e6;

    for (double value : outputHistory)
    {
        if (!std::isfinite(value))
        {
            return false;
        }

        if (std::abs(value) > MAX_VALUE)
        {
            return false;
        }
    }

    return true;
}
#pragma once

#include <vector>

class StabilityAnalyzer
{
public:
    bool isStable(
        const std::vector<double>& outputHistory,
        double tolerance = 1e-3
    ) const;
};
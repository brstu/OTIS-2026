#include "StabilityAnalyzer.h"

#include <cmath>
#include <complex>
#include <vector>

using namespace std;

bool StabilityAnalyzer::isStable(double a1, double a2, int k)
{
    if (k < 1)
        return false;

    // Characteristic equation:
    // z^(k+1) - a1 * z^k - a2 = 0

    const int n = k + 1;
    const double PI = acos(-1.0);

    vector<complex<double>> roots(n);

    // Initial approximation of roots
    double radius = 1.0 + abs(a1) + abs(a2);

    for (int i = 0; i < n; i++)
    {
        double angle = 2.0 * PI * i / n + 0.25;

        roots[i] = polar(radius, angle);
    }

    // Polynomial calculation using Horner's method
    auto polynomial = [&](complex<double> z)
    {
        return pow(z, k + 1) - a1 * pow(z, k) - a2;
    };

    // Durand-Kerner method
    for (int iteration = 0; iteration < 1000; iteration++)
    {
        double maxChange = 0.0;

        vector<complex<double>> newRoots = roots;

        for (int i = 0; i < n; i++)
        {
            complex<double> denominator(1.0, 0.0);

            for (int j = 0; j < n; j++)
            {
                if (i != j)
                    denominator *= roots[i] - roots[j];
            }

            if (abs(denominator) < 1e-14)
                return false;

            complex<double> correction =
                polynomial(roots[i]) / denominator;

            newRoots[i] = roots[i] - correction;

            maxChange = max(maxChange, abs(correction));
        }

        roots = newRoots;

        if (maxChange < 1e-10)
            break;
    }

    // All roots must be inside the unit circle
    for (const auto& root : roots)
    {
        if (abs(root) >= 1.0 - 1e-9)
            return false;
    }

    return true;
}
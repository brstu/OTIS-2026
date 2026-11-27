#include "StabilityAnalyzer.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <ostream>
#include <sstream>

namespace
{

std::complex<double> evaluate(const std::vector<double>& coefficients,
                              std::complex<double> z)
{
    std::complex<double> result(0.0, 0.0);
    for (std::size_t i = coefficients.size(); i-- > 0;)
        result = result * z + coefficients[i];
    return result;
}

std::complex<double> computeDenominator(const std::vector<std::complex<double>>& roots, int i)
{
    std::complex<double> denominator(1.0, 0.0);
    for (int j = 0; j < 3; ++j)
    {
        if (j != i)
            denominator *= (roots[i] - roots[j]);
    }
    return denominator;
}

std::vector<std::complex<double>> durandKerner3(const std::vector<double>& coefficients)
{
    std::vector<std::complex<double>> roots(3);

    const std::complex<double> seed(0.4, 0.9);
    std::complex<double> current(1.0, 0.0);
    for (int i = 0; i < 3; ++i)
    {
        roots[i] = current;
        current *= seed;
    }

    const double leading = coefficients.back();
    for (int iteration = 0; iteration < 1000; ++iteration)
    {
        double maxDelta = 0.0;
        for (int i = 0; i < 3; ++i)
        {
            const std::complex<double> numerator = evaluate(coefficients, roots[i]) / leading;
            const std::complex<double> denominator = computeDenominator(roots, i);

            const std::complex<double> delta = numerator / denominator;
            roots[i] -= delta;
            maxDelta = std::max(maxDelta, std::abs(delta));
        }

        if (maxDelta < 1e-12)
            break;
    }

    return roots;
}

std::string verdictName(StabilityVerdict verdict)
{
    switch (verdict)
    {
    case StabilityVerdict::Stable:   return "STABLE";
    case StabilityVerdict::Marginal: return "MARGINAL";
    case StabilityVerdict::Unstable: return "UNSTABLE";
    }
    return "UNKNOWN";
}

std::string formatComplex(std::complex<double> value)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(6);
    out << value.real();
    if (std::abs(value.imag()) >= 1e-12)
        out << (value.imag() >= 0.0 ? " + " : " - ") << std::abs(value.imag()) << "i";
    return out.str();
}
}

std::vector<std::complex<double>> StabilityAnalyzer::findRoots(
    const std::vector<double>& coefficients)
{

    std::vector<double> poly = coefficients;
    while (poly.size() > 1 && poly.back() == 0.0)
        poly.pop_back();

    const std::size_t degree = poly.size() - 1;

    if (degree == 0)
        return {};

    if (degree == 1)
    {

        return { std::complex<double>(-poly[0] / poly[1], 0.0) };
    }

    if (degree == 2)
    {

        const double c0 = poly[0];
        const double c1 = poly[1];
        const double c2 = poly[2];
        const double discriminant = c1 * c1 - 4.0 * c2 * c0;
        const std::complex<double> sqrtD =
            discriminant >= 0.0
                ? std::complex<double>(std::sqrt(discriminant), 0.0)
                : std::complex<double>(0.0, std::sqrt(-discriminant));
        return {
            (-c1 + sqrtD) / (2.0 * c2),
            (-c1 - sqrtD) / (2.0 * c2)
        };
    }

    if (degree == 3)
        return durandKerner3(poly);

    return {};
}

StabilityVerdict StabilityAnalyzer::schurVerdict(const std::vector<double>& coefficients)
{
    std::vector<double> p = coefficients;

    while (p.size() > 1 && p.back() == 0.0)
        p.pop_back();

    if (p.size() <= 1)
        return StabilityVerdict::Stable;

    const double leading = p.back();
    for (double& value : p)
        value /= leading;

    std::size_t n = p.size() - 1;
    bool onBoundary = false;

    while (n >= 1)
    {
        const double a0 = p.front();
        const double an = p[n];
        const double absA0 = std::abs(a0);
        const double absAn = std::abs(an);
        const double eps = 1e-12 * std::max(1.0, absAn);

        if (absA0 > absAn + eps)
            return StabilityVerdict::Unstable;

        if (std::abs(absA0 - absAn) <= eps)
            onBoundary = true;

        const double denominator = an * an - a0 * a0;
        if (!(denominator > 0.0))
            return onBoundary ? StabilityVerdict::Marginal : StabilityVerdict::Unstable;

        std::vector<double> q(n);
        for (std::size_t k = 1; k <= n; ++k)
            q[k - 1] = (an * p[k] - a0 * p[n - k]) / denominator;

        p = std::move(q);
        n = p.size() - 1;
    }

    return onBoundary ? StabilityVerdict::Marginal : StabilityVerdict::Stable;
}

bool StabilityAnalyzer::isStable(const std::vector<double>& coefficients)
{
    return schurVerdict(coefficients) == StabilityVerdict::Stable;
}

bool StabilityAnalyzer::needsWarning(const StabilityReport& report)
{
    return report.verdict != StabilityVerdict::Stable;
}

double StabilityAnalyzer::spectralRadius(const std::vector<std::complex<double>>& roots)
{
    double radius = 0.0;
    for (const std::complex<double>& root : roots)
        radius = std::max(radius, std::abs(root));
    return radius;
}

bool StabilityAnalyzer::isMarginallyStable(const std::vector<double>& coefficients)
{
    return schurVerdict(coefficients) == StabilityVerdict::Marginal;
}

std::vector<double> StabilityAnalyzer::characteristicPolynomial1_6(double a1, double a2, double a3)
{

    return { -a3, -a2, -a1, 1.0 };
}

StabilityReport StabilityAnalyzer::analyzeModel1_6(double a1, double a2, double a3)
{

    const auto poly = std::vector{ -a3, -a2, -a1, 1.0 };

    StabilityReport report;
    report.modelName = "Model 1.6 (linear, order 3)";
    report.coefficients = poly;
    report.roots = findRoots(poly);
    report.verdict = schurVerdict(poly);

    double radius = 0.0;
    for (const std::complex<double>& root : report.roots)
        radius = std::max(radius, std::abs(root));
    report.spectralRadius = radius;

    return report;
}

std::string StabilityAnalyzer::describe(const StabilityReport& report)
{
    std::ostringstream out;
    out << "Модель: " << report.modelName << '\n';

    out << "Характеристическое уравнение: ";
    for (std::size_t i = 0; i < report.coefficients.size(); ++i)
    {
        if (i != 0)
            out << " + ";
        out << std::showpos << report.coefficients[i] << std::noshowpos
            << "*z^" << i;
    }
    out << " = 0\n";

    out << "Корни:";
    for (const std::complex<double>& root : report.roots)
        out << "\n  z = " << formatComplex(root)
            << "   (|z| = " << std::fixed << std::setprecision(6) << std::abs(root) << ')';
    out << "\nВердикт (критерий Шура): " << verdictName(report.verdict) << '\n';
    return out.str();
}

std::string StabilityAnalyzer::warningText(const StabilityReport& report)
{
    if (report.verdict == StabilityVerdict::Stable)
        return {};

    std::ostringstream out;
    out << "WARNING: ";
    if (report.verdict == StabilityVerdict::Marginal)
    {
        out << "система на границе устойчивости (|z|max = 1):\n"
               "         колебания не затухают, численное решение может \"уплывать\".\n";
    }
    else
    {
        out << "система НЕУСТОЙЧИВА (|z|max > 1):\n"
               "         решение будет расходящимся, результаты симуляции не имеют смысла.\n";
    }
    out << "         Модель: " << report.modelName << '\n';
    out << "         |z|max = " << std::fixed << std::setprecision(6)
        << report.spectralRadius << '\n';

    out << "         Корни характеристического уравнения:";
    for (const std::complex<double>& root : report.roots)
        out << "\n           z = " << formatComplex(root) << "   (|z| = "
            << std::fixed << std::setprecision(6) << std::abs(root) << ')';
    return out.str();
}

bool StabilityAnalyzer::printWarningIfNeeded(const StabilityReport& report, std::ostream& os)
{
    if (report.verdict == StabilityVerdict::Stable)
    {
        os << "Проверка устойчивости: система устойчива.\n";
        return false;
    }

    os << "\n============================================================\n";
    os << "WARNING: система НЕУСТОЙЧИВА";
    if (report.verdict == StabilityVerdict::Marginal)
        os << " (на границе |z| = 1)";
    os << ":\n";
    os << "         решение будет расходящимся, результаты симуляции не имеют смысла.\n";
    os << "         Модель: " << report.modelName << '\n';
    os << "         Корни характеристического уравнения:";
    for (const std::complex<double>& root : report.roots)
        os << "\n           z = " << formatComplex(root) << "   (|z| = "
           << std::fixed << std::setprecision(6) << std::abs(root) << ')';
    os << "\n============================================================\n\n";
    return true;
}

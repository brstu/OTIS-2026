#include "LinearARXModel.h"

LinearARXModel::LinearARXModel(double a1_, double a2_, double b1_, double b2_)
    : a1(a1_), a2(a2_), b1(b1_), b2(b2_)
{
}

double LinearARXModel::step(double u)
{
    double nextY = a1 * currentY + a2 * previousY + b1 * u + b2 * previousU;
    previousY = currentY;
    currentY = nextY;
    previousU = u;
    return currentY;
}

void LinearARXModel::reset()
{
    currentY = 0.0;
    previousY = 0.0;
    previousU = 0.0;
}

std::string LinearARXModel::getDescription() const
{
    return "Model 1.8 (Generalized Autoregressive Linear Model)";
}

bool LinearARXModel::checkStability() const
{
    const double discriminant = a1 * a1 + 4.0 * a2;// NOSONAR
    if (discriminant < 0.0)
    {
        return (a2 > -1.0) && (a2 < 1.0);
    }
    const double sqrtD = std::sqrt(discriminant);
    const double z1 = (a1 + sqrtD) / 2.0;
    const double z2 = (a1 - sqrtD) / 2.0;
    return (std::abs(z1) < 1.0) && (std::abs(z2) < 1.0);
}

std::string LinearARXModel::getWarningMessage() const
{
    if (checkStability()) return "";

    std::ostringstream oss;
    if (const double discriminant = a1 * a1 + 4.0 * a2; discriminant < 0.0)
    {
        oss << "[WARNING] Model 1.8: complex roots, |a2| >= 1. System may be unstable.";
    }
    else
    {
        const double sqrtD = std::sqrt(discriminant);
        oss << "[WARNING] Model 1.8: roots z1=" << (a1 + sqrtD) / 2.0
            << ", z2=" << (a1 - sqrtD) / 2.0 << " - UNSTABLE.";
    }
    return oss.str();
}
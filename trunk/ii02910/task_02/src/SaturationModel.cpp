#include "SaturationModel.h"

SaturationModel::SaturationModel(double a_, double b_, double uMin_, double uMax_)
    : a(a_), b(b_), uMin(uMin_), uMax(uMax_)
{
}

double SaturationModel::applySaturation(double u) const
{
    if (u > uMax) return uMax;
    if (u < uMin) return uMin;
    return u;
}

double SaturationModel::step(double u)
{
    currentY = a * currentY + b * applySaturation(u);
    return currentY;
}

void SaturationModel::reset()
{
    currentY = 0.0;
}

std::string SaturationModel::getDescription() const
{
    return "Model 2.2 (Actuator Saturation Non-linearity)";
}
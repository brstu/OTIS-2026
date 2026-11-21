#include "SystemFactory.h"
#include "LinearARXModel.h"
#include "SaturationModel.h"
#include "CubicODE.h"
#include <iostream>
#include <limits>
#include <stdexcept>

namespace
{
    double readDoubleParam(const std::string& prompt)
    {
        while (true)
        {
            std::cout << prompt;
            if (double value; std::cin >> value && std::isfinite(value))
                return value;
            std::cout << "Invalid input. Try again.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

std::unique_ptr<AbstractSystem> SystemFactory::create(SystemType type)
{
    switch (type)
    {
    case SystemType::ARX:
    {
        std::cout << "\n--- Model 1.8 parameters ---\n";
        double a1 = readDoubleParam("a1 = ");
        double a2 = readDoubleParam("a2 = ");
        double b1 = readDoubleParam("b1 = ");
        double b2 = readDoubleParam("b2 = ");
        return std::make_unique<LinearARXModel>(a1, a2, b1, b2);
    }
    case SystemType::Sat:
    {
        std::cout << "\n--- Model 2.2 parameters ---\n";
        double a = readDoubleParam("a  = ");
        double b = readDoubleParam("b  = ");
        double uMin = readDoubleParam("uMin = ");
        double uMax = readDoubleParam("uMax = ");
        return std::make_unique<SaturationModel>(a, b, uMin, uMax);
    }
    case SystemType::Cubic:
    {
        std::cout << "\n--- Model 3.6 parameters ---\n";
        double a = readDoubleParam("a  = ");
        double b = readDoubleParam("b  = ");
        double dt = readDoubleParam("dt = ");
        return std::make_unique<CubicODE>(a, b, dt);
    }
    default:
        throw std::invalid_argument("Unknown system type");
    }
}
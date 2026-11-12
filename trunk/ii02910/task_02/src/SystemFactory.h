#pragma once

#include <memory>
#include "AbstractSystem.h"

class SystemFactory
{
public:
    enum class SystemType
    {
        ARX = 1,
        Sat = 2,
        Cubic = 3
    };

    static std::unique_ptr<AbstractSystem> create(SystemType type);
};
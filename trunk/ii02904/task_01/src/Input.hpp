#pragma once
#include <cmath>
#include <string>

enum class InputType
{
    Step,
    Impulse,
    Harmonic
};

inline double inputValue(InputType type, int tau, double stepValue = 1.0)
{
    switch (type)
    {
    case InputType::Step:
        return stepValue; // u = const
    case InputType::Impulse:
        return tau == 0 ? 1.0 : 0.0; // u0 = 1, дальше 0
    case InputType::Harmonic:
        return std::sin((double)tau); // u = sin(tau)
    }
    return 0.0;
}

inline std::string inputName(InputType type)
{
    switch (type)
    {
    case InputType::Step:
        return "step";
    case InputType::Impulse:
        return "impulse";
    case InputType::Harmonic:
        return "harmonic";
    }
    return "";
}

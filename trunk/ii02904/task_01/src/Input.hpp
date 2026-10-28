#pragma once
#include <array>
#include <cmath>
#include <string>

enum class InputType
{
    Step,
    Impulse,
    Harmonic
};

// Параметры всех возможных воздействий
struct InputParams
{
    double stepValue    = 1.0; // step:     u = const
    double impulseValue = 1.0; // impulse:  u(0) = impulseValue, дальше 0
    double harmonicAmp  = 1.0; // harmonic: амплитуда
    double harmonicFreq = 1.0; // harmonic: частота (рад/шаг)
};

inline double inputValue(InputType type, int tau, const InputParams& p = InputParams{})
{
    switch (type)
    {
    case InputType::Step:
        return p.stepValue;
    case InputType::Impulse:
        return tau == 0 ? p.impulseValue : 0.0;
    case InputType::Harmonic:
        return p.harmonicAmp * std::sin(p.harmonicFreq * static_cast<double>(tau));
    }
    return 0.0;
}

// имя для файла: results_<name>.csv
inline std::string inputName(InputType type)
{
    switch (type)
    {
    case InputType::Step:     return "step";
    case InputType::Impulse:  return "impulse";
    case InputType::Harmonic: return "harmonic";
    }
    return "";
}

// человекочитаемое имя для меню и подписей
inline std::string inputTitle(InputType type)
{
    switch (type)
    {
    case InputType::Step:     return "step (ступенька)";
    case InputType::Impulse:  return "impulse (импульс)";
    case InputType::Harmonic: return "harmonic (синусоида)";
    }
    return "";
}

// номер пункта меню: 1..3
inline int inputCode(InputType type)
{
    switch (type)
    {
    case InputType::Step:     return 1;
    case InputType::Impulse:  return 2;
    case InputType::Harmonic: return 3;
    }
    return 0;
}

// обратное преобразование номера в тип
inline bool inputTypeFromCode(int code, InputType& out)
{
    switch (code)
    {
    case 1: out = InputType::Step;     return true;
    case 2: out = InputType::Impulse;  return true;
    case 3: out = InputType::Harmonic; return true;
    default: return false;
    }
}

inline const std::array<InputType, 3>& allInputTypes()
{
    static const std::array<InputType, 3> all{
        InputType::Step, InputType::Impulse, InputType::Harmonic };
    return all;
}
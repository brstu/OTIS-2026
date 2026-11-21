#pragma once

#include "stability.hpp"

// Абстрактный базовый класс математической модели.
class Model {
public:
    virtual ~Model() = default;

    virtual double next(double u) = 0;
    virtual void reset() = 0;
    virtual double value() const = 0;
    virtual std::string name() const = 0;
    virtual double pole() const = 0;

    bool isStable() const { return classifyPole(pole()) == Stability::Stable; }
    std::string stabilityWarning() const { return poleWarning(pole()); }
};

#pragma once
#include <string>

class DynamicModel {
public:
    virtual ~DynamicModel() = default;

    virtual double advance(double u) = 0;
    virtual void reset() = 0;
    virtual std::string describe() const = 0;
};
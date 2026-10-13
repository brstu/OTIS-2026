#pragma once
#include <string>

class Model {
protected:
    double y = 0.0;  // текущее значение y_tau
public:
    virtual ~Model() = default;

    // принимает u_tau, возвращает y_{tau+1}
    virtual double nextStep(double u) = 0;

    virtual std::string name() const = 0;

    double current() const { return y; }
};

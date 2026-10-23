#pragma once
#include <string>


class Model {
public:
    virtual ~Model() = default;

    // Главный метод: принимает u_tau, возвращает y_{tau+1}
    virtual double nextStep(double u) = 0;

    virtual std::string name() const = 0;

    // Текущее значение y_tau
    virtual double current() const = 0;
};

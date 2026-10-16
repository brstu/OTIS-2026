#pragma once

#include <string>

// Базовый абстрактный класс для всех моделей объекта управления.
//
// Модель хранит своё состояние (текущее и предыдущее значения y и u),
// поэтому наследнику нужно реализовать только формулу следующего шага
// в методе calculateNext(). Сдвиг истории делает базовый класс в step().
class Model
{
public:
    virtual ~Model() = default;

    // Установить начальное состояние: y(0) = y0.
    // Значения "до начала моделирования" принимаются равными:
    // y(-1) = y0, u(-1) = 0.
    void reset(double y0);

    // Выполнить один шаг моделирования с входом u(tau)
    // и вернуть новое значение y(tau+1).
    double step(double u);

    double currentY() const { return y_; }

    virtual std::string name() const = 0;
    virtual std::string formula() const = 0;

protected:
    // Расчёт следующего значения y(tau+1) по формуле конкретной модели.
    // Доступны: y_ = y(tau), yPrev_ = y(tau-1), uPrev_ = u(tau-1).
    virtual double calculateNext(double u) const = 0;

    double y_ = 0.0;
    double yPrev_ = 0.0;
    double uPrev_ = 0.0;
};

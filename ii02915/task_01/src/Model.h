#pragma once
#include <string>

// Базовый абстрактный класс: любая модель умеет считать следующий шаг.
class Model {
public:
    virtual ~Model() = default;

    // Принимает входной сигнал u(tau) и возвращает y(tau+1).
    virtual double calculateNext(double u) = 0;

    // Текущее значение выхода y(tau).
    virtual double current() const = 0;

    virtual std::string getName() const = 0;

    // Проверка устойчивости (используется для предупреждения перед симуляцией).
    virtual bool isStable() const { return true; }
};

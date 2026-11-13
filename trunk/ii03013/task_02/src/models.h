#pragma once

#include <string>
#include <vector>

class Model {
public:
    virtual ~Model() = default;

    virtual double next(
        double y,
        double u,
        double delayedU,
        double dt
    ) const = 0;

    virtual void reset() {}

    virtual std::string name() const = 0;

    // Характеристический многочлен линейной части модели:
    // c[0]*z^n + c[1]*z^(n-1) + ... + c[n] (от старшей степени к младшей).
    // Для моделей без линейной дискретной части возвращает пустой вектор
    // (анализ устойчивости по корням для них не применяется).
    virtual std::vector<double> characteristicPolynomial() const {
        return {};
    }

    // Устойчивость по расположению корней на Z-плоскости (|z| < 1).
    // Если многочлена нет, модель считается "не подлежащей анализу" -> true.
    virtual bool isStable() const;

    // Текст предупреждения (пустая строка, если модель устойчива).
    virtual std::string stabilityWarning() const;
};

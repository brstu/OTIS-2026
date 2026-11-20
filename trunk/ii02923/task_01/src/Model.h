#pragma once

#include <string>

/**
 * @brief Абстрактный базовый класс модели объекта управления.
 *
 * Каждая конкретная модель реализует один шаг дискретной симуляции.
 * Состояние хранится внутри модели и сбрасывается методом reset().
 */
class Model {
public:
    virtual ~Model() = default;

    /// Сбросить внутреннее состояние модели в начальное (y = 0).
    virtual void reset() = 0;

    /// Выполнить один шаг симуляции и вернуть новое значение выхода y.
    virtual double step(double u, double dt) = 0;

    /// Человекочитаемое имя модели, например "Model 1.1".
    virtual std::string name() const = 0;

    /// Строка с уравнением модели для вывода в консоль и отчёт.
    virtual std::string equation() const = 0;

    /// Устойчива ли модель при текущих коэффициентах.
    virtual bool isStable() const { return true; }

    /// Корень характеристического уравнения (для моделей первого порядка).
    virtual double characteristicRoot() const { return 0.0; }
};

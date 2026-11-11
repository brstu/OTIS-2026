#pragma once

#include "Model.h"

/**
 * @brief Model 3.9 — дифференциальное уравнение с ограниченной скоростью.
 *
 * dy/dt = b * tanh(u)
 * Решается явным методом Эйлера с шагом dt:
 *     y_{t+1} = y_t + dt * b * tanh(u_t)
 *
 * Уравнение не содержит y в правой части, поэтому собственных корней
 * характеристического уравнения нет: система нейтральна по выходу.
 */
class Model3_9 : public Model {
public:
    explicit Model3_9(double b = 1.0);

    void reset() override;
    double step(double u, double dt) override;
    std::string name() const override;
    std::string equation() const override;
    bool isStable() const override;

    double b() const { return b_; }

private:
    double b_;
    double y_ = 0.0;
};

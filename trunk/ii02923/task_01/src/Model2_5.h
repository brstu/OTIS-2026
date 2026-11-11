#pragma once

#include "Model.h"

/**
 * @brief Model 2.5 — нелинейность типа "сухое трение" (signum-функция).
 *
 * y_{t+1} = a*y_t + b * sign(u_t) * (1 - exp(-|u_t|))
 *
 * При u = 0 второе слагаемое равно нулю, поэтому положение равновесия
 * совпадает с линейной моделью y_{t+1} = a*y_t + b*u_t (в малом).
 */
class Model2_5 : public Model {
public:
    explicit Model2_5(double a = 0.85, double b = 1.2);

    void reset() override;
    double step(double u, double dt) override;
    std::string name() const override;
    std::string equation() const override;
    bool isStable() const override;
    double characteristicRoot() const override;

    double a() const { return a_; }
    double b() const { return b_; }

private:
    double a_;
    double b_;
    double y_ = 0.0;
};

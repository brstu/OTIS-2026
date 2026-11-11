#pragma once

#include "Model.h"

/**
 * @brief Model 1.1 — линейная инерционная система первого порядка.
 *
 * y_{t+1} = a * y_t + b * u_t
 */
class Model1_1 : public Model {
public:
    explicit Model1_1(double a = 0.8, double b = 1.0);

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

#pragma once

#include <string>

class Model {
public:
    virtual ~Model() = default;
    virtual double next(double u, double previousU) = 0;
    virtual void reset() = 0;
    virtual std::string name() const = 0;
};

class LinearModel19 final : public Model {
public:
    LinearModel19(double a1, double a2, double a3, double b1);
    double next(double u, double previousU) override;
    void reset() override;
    std::string name() const override;

private:
    double a1_;
    double a2_;
    double a3_;
    double b1_;
    double y0_ = 0.0;
    double y1_ = 0.0;
    double y2_ = 0.0;
};

class NonlinearModel21 final : public Model {
public:
    NonlinearModel21(double a, double b, double c, double d);
    double next(double u, double previousU) override;
    void reset() override;
    std::string name() const override;

private:
    double a_;
    double b_;
    double c_;
    double d_;
    double y0_ = 0.0;
    double y1_ = 0.0;
};

class DifferentialModel33 final : public Model {
public:
    DifferentialModel33(double a, double b, double dt);
    double next(double u, double previousU) override;
    void reset() override;
    std::string name() const override;

private:
    double a_;
    double b_;
    double dt_;
    double y_ = 0.0;
};

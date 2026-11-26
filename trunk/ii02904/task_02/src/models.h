#pragma once

#include <string>

class Model {
public:
    virtual ~Model() = default;
    virtual double nextStep(double input) = 0;
    virtual std::string name() const = 0;
};

class Model14 final : public Model {
public:
    Model14(double a, double b, double initialOutput);

    double nextStep(double input) override;
    std::string name() const override;

private:
    double a_;
    double b_;
    double output_;
    double previousInput_ = 0.0;
};

class Model26 final : public Model {
public:
    Model26(double a1, double a2, double b, double initialOutput);

    double nextStep(double input) override;
    std::string name() const override;

private:
    double a1_;
    double a2_;
    double b_;
    double output_;
    double previousOutput_ = 0.0;
};

class Model38 final : public Model {
public:
    Model38(double a1, double a2, double b, double timeStep, double initialOutput);

    double nextStep(double input) override;
    std::string name() const override;

private:
    double a1_;
    double a2_;
    double b_;
    double timeStep_;
    double output_;
};

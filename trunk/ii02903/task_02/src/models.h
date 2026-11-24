#pragma once
#include <string>

class Model {
public:
    virtual ~Model() = default;

    virtual double nextStep(double u) = 0;

    virtual double current() const = 0;

    virtual std::string name() const = 0;
    virtual std::string tag() const = 0;   
    virtual std::string equation() const = 0;
    virtual std::string params() const = 0;

    virtual double timeStep() const { return 1.0; }

    virtual bool isStable(std::string& info) const;
};

class LinearModel13 : public Model {
public:
    LinearModel13(double a1, double a2, double b, double y0);

    double nextStep(double u) override;
    double current() const override { return yCur_; }

    std::string name() const override;
    std::string tag() const override { return "model13"; }
    std::string equation() const override;
    std::string params() const override;

    bool isStable(std::string& info) const override;

private:
    double a1_;
    double a2_;
    double b_;
    double yCur_;       
    double yPrev_ = 0.0;  
};

class NonlinearModel25 : public Model {
public:
    NonlinearModel25(double a, double b, double y0);

    double nextStep(double u) override;
    double current() const override { return y_; }

    std::string name() const override;
    std::string tag() const override { return "model25"; }
    std::string equation() const override;
    std::string params() const override;

private:
    static double signOf(double u);

    double a_;
    double b_;
    double y_;
};

class DiffModel37 : public Model {
public:
    DiffModel37(double a, double b, double dt, double y0);

    double nextStep(double u) override;
    double current() const override { return y_; }
    double timeStep() const override { return dt_; }

    std::string name() const override;
    std::string tag() const override { return "model37"; }
    std::string equation() const override;
    std::string params() const override;

private:
    double a_;
    double b_;
    double dt_;
    double y_;
};

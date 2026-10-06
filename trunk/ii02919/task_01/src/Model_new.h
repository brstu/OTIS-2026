#pragma once

#include <cmath>
#include <memory>
#include <sstream>
#include <string>

// ============================================================================
//  DynamicProcess — абстрактный базовый класс динамических процессов
// ============================================================================
class DynamicProcess
{
public:
    virtual ~DynamicProcess() = default;

    virtual double advance(double u) = 0;
    virtual void reset() = 0;
    virtual std::string title() const = 0;

    virtual bool isStableSystem() const { return true; }
    virtual std::string alertText() const { return {}; }
};

// ============================================================================
//  MultiStepModel — Model 1.7
//  y(t+1) = a*y(t) + b1*u(t) + b2*u(t-1) + b3*u(t-2)
// ============================================================================
class MultiStepModel : public DynamicProcess
{
private:
    double a = 0.0;
    double b1 = 0.0;
    double b2 = 0.0;
    double b3 = 0.0;
    double y = 0.0;
    double u_prev1 = 0.0;
    double u_prev2 = 0.0;

public:
    MultiStepModel(double a_, double b1_, double b2_, double b3_)
        : a(a_), b1(b1_), b2(b2_), b3(b3_)
    {
    }

    double advance(double u) override
    {
        const double y_next = a * y + b1 * u + b2 * u_prev1 + b3 * u_prev2;
        u_prev2 = u_prev1;
        u_prev1 = u;
        y = y_next;
        return y;
    }

    void reset() override
    {
        y = 0.0;
        u_prev1 = 0.0;
        u_prev2 = 0.0;
    }

    std::string title() const override
    {
        return "Model 1.7 (Multi-Step Control History)";
    }

    bool isStableSystem() const override
    {
        return std::abs(a) < 1.0;
    }

    std::string alertText() const override
    {
        const double abs_a = std::abs(a);
        if (abs_a < 1.0) return {};

        std::ostringstream oss;
        if (abs_a == 1.0)
        {
            oss << "[WARN] Model 1.7: |a| = 1, система на границе устойчивости.";
        }
        else
        {
            oss << "[WARN] Model 1.7: |a| = " << abs_a
                << " >= 1, система НЕУСТОЙЧИВА.";
        }
        return oss.str();
    }
};

// ============================================================================
//  QuadraticFeedback — Model 2.1
//  y(t+1) = a*y(t) - b*y(t-1)^2 + c*u(t) + d*sin(u(t-1))
// ============================================================================
class QuadraticFeedback : public DynamicProcess
{
private:
    double a = 0.0;
    double b = 0.0;
    double c = 0.0;
    double d = 0.0;
    double y = 0.0;
    double y_prev = 0.0;
    double u_prev = 0.0;

public:
    QuadraticFeedback(double a_, double b_, double c_, double d_)
        : a(a_), b(b_), c(c_), d(d_)
    {
    }

    double advance(double u) override
    {
        const double y_next = a * y
                            - b * y_prev * y_prev
                            + c * u
                            + d * std::sin(u_prev);
        y_prev = y;
        y = y_next;
        u_prev = u;
        return y;
    }

    void reset() override
    {
        y = 0.0;
        y_prev = 0.0;
        u_prev = 0.0;
    }

    std::string title() const override
    {
        return "Model 2.1 (Quadratic Feedback and Harmonic Control)";
    }
};

// ============================================================================
//  HarmonicODE — Model 3.5
//  dy/dt = b*sin(u), решается методом Эйлера
//  y(t+1) = y(t) + dt*b*sin(u)
// ============================================================================
class HarmonicODE : public DynamicProcess
{
private:
    double b = 0.0;
    double dt = 0.01;
    double y = 0.0;

public:
    HarmonicODE(double b_, double dt_)
        : b(b_), dt(dt_)
    {
    }

    double advance(double u) override
    {
        const double dy = b * std::sin(u);
        y = y + dt * dy;
        return y;
    }

    void reset() override
    {
        y = 0.0;
    }

    std::string title() const override
    {
        return "Model 3.5 (Harmonic Driving Force)";
    }
};

// ============================================================================
//  ProcessFactory — фабрика моделей
// ============================================================================
class ProcessFactory
{
public:
    static std::unique_ptr<DynamicProcess> create(int type,
                                                   double p1,
                                                   double p2,
                                                   double p3,
                                                   double p4)
    {
        switch (type)
        {
        case 1:
            return std::make_unique<MultiStepModel>(p1, p2, p3, p4);
        case 2:
            return std::make_unique<QuadraticFeedback>(p1, p2, p3, p4);
        case 3:
            return std::make_unique<HarmonicODE>(p1, p2);
        default:
            return nullptr;
        }
    }
};

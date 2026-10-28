#pragma once
#include <algorithm>
#include <memory>
#include <string>
#include <vector>


class Model {
public:
    explicit Model(double y0 = 0.0) : y0_(y0), y_(y0) {}
    virtual ~Model() = default;

    
    virtual double nextStep(double u) = 0;
    virtual std::string name() const = 0;

    virtual void reset() { y_ = y0_; }
    double y() const { return y_; }

protected:
    double y0_;
    double y_;
};


// Модель 1.10

class DistributedControlModel : public Model {
public:
    DistributedControlModel(double a1, double b1, double b2, double y0 = 0.0)
        : Model(y0), a1_(a1), b1_(b1), b2_(b2) {}

    double nextStep(double u) override {
        // u_{t-2}: в начале процесса управление считается нулевым
        double uDelayed = (hist_.size() >= 2) ? hist_[hist_.size() - 2] : 0.0;
        hist_.push_back(u);
        y_ = a1_ * y_ + b1_ * u + b2_ * uDelayed;
        return y_;
    }
    std::string name() const override { return "Model_1_10_DistributedControl"; }
    void reset() override { Model::reset(); hist_.clear(); }

private:
    double a1_, b1_, b2_;
    std::vector<double> hist_;   // u_0 ... u_t
};


// Модель 2.2
class SaturationModel : public Model {
public:
    SaturationModel(double a, double b, double uMin, double uMax, double y0 = 0.0)
        : Model(y0), a_(a), b_(b), uMin_(uMin), uMax_(uMax) {}

    double nextStep(double u) override {
        y_ = a_ * y_ + b_ * sat(u);
        return y_;
    }
    std::string name() const override { return "Model_2_2_Saturation"; }

private:
    double sat(double u) const { return std::clamp(u, uMin_, uMax_); }
    double a_, b_, uMin_, uMax_;
};


// Модель 3.4

class QuadraticDecayModel : public Model {
public:
    QuadraticDecayModel(double a, double h, double y0)
        : Model(y0), a_(a), h_(h) {}

    double nextStep(double ) override {
        y_ = y_ - h_ * a_ * y_ * y_;
        return y_;
    }
    std::string name() const override { return "Model_3_4_QuadraticDecay"; }

  
    double exact(double t) const { return y0_ / (1.0 + a_ * y0_ * t); }
    double h() const { return h_; }

private:
    double a_, h_;
};

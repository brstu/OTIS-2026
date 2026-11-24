#pragma once

class model {
public:
    virtual ~model() noexcept = default;

    virtual bool isStable() const {
        return true;
    }

    virtual double step(double previous_y, double previous_u) = 0;
};

class model3_5 : public model {
public:
    model3_5(double b, double dt);
    double step(double previous_y, double previous_u) override;

private:
    double b;
    double dt;
};

class model2_3 : public model {
public:
    model2_3(double a, double b, double delta);
    double step(double previous_y, double previous_u) override;

private:
    double deadzone(double u) const;
    double a;
    double b;
    double delta;
};

class model1_1 : public model {
public:
    model1_1(double a, double b);

    bool isStable() const override;
    double step(double previous_y, double previous_u) override;

private:
    double a;
    double b;
};
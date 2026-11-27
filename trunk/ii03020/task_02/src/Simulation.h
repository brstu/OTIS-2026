#ifndef SIMULATION_H
#define SIMULATION_H

// Базовый класс модели
class Model {
private:
    double y;

public:
    explicit Model(double initial_y = 0.0) : y(initial_y) {}
    virtual ~Model() = default;

    virtual double step(double u) = 0;

    double getState() const { return y; }
    double getY() const { return y; }
    void setY(double new_y) { y = new_y; }
    void reset(double initial_y = 0.0) { y = initial_y; }
};

// Модель 1.1: Линейная
class Model1_1 : public Model {
private:
    double a;
    double b;

public:
    explicit Model1_1(double a_val, double b_val, double initial_y = 0.0);
    double step(double u) override;
    double getA() const { return a; }
    double getB() const { return b; }
};

// Модель 2.5: Нелинейная модель с насыщением
class Model2_5 : public Model {
private:
    double a;
    double b;

public:
    static double sign(double val);
    explicit Model2_5(double a_val, double b_val, double initial_y = 0.0);
    double step(double u) override;
    double getA() const { return a; }
    double getB() const { return b; }
};

// Модель 3.9: Дифференциальное уравнение с методом Эйлера
class Model3_9 : public Model {
private:
    double b;
    double dt;

public:
    explicit Model3_9(double b_val, double dt_val = 0.1, double initial_y = 0.0);
    double step(double u) override;
    double getB() const { return b; }
    double getDt() const { return dt; }
};

#endif
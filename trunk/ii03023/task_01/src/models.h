#pragma once
#include <cmath>
#include <string>

// ===================== Входные воздействия =====================

// Базовый абстрактный класс входного сигнала
class Input {
public:
    virtual ~Input() = default;
    virtual double value(int tau) const = 0;   // значение входа в момент tau
    virtual std::string name() const = 0;
};

// Ступенчатое воздействие: постоянное значение на всех шагах
class StepInput : public Input {
    double c;
public:
    explicit StepInput(double c = 1.0) : c(c) {}
    double value(int) const override { return c; }
    std::string name() const override { return "step"; }
};

// Импульсное воздействие: единица на нулевом шаге, далее ноль
class ImpulseInput : public Input {
public:
    double value(int tau) const override { return tau == 0 ? 1.0 : 0.0; }
    std::string name() const override { return "impulse"; }
};

// Гармоническое воздействие: синус от номера шага
class HarmonicInput : public Input {
public:
    double value(int tau) const override { return std::sin(static_cast<double>(tau)); }
    std::string name() const override { return "harmonic"; }
};

// ===================== Модели =====================

// Базовый абстрактный класс модели
class Model {
    double y;        // текущее значение выхода
    double y0;       // начальное значение выхода
protected:
    void setY(double value) { y = value; }
public:
    explicit Model(double start = 0.0) : y(start), y0(start) {}
    virtual ~Model() = default;
    // Рассчитать следующий шаг по текущему входу
    virtual double next(double u) = 0;
    virtual std::string name() const = 0;
    virtual void reset() { y = y0; }
    double current() const { return y; }
};

// Model 1.10 (Distributed Control Window Model).
// Выход зависит от текущего выхода, текущего входа и входа двумя шагами ранее.
class LinearModel : public Model {
    double a1;
    double b1;
    double b2;
    double u1 = 0.0;   // вход на предыдущем шаге
    double u2 = 0.0;   // вход два шага назад
public:
    LinearModel(double a1, double b1, double b2, double y0 = 0.0)
        : Model(y0), a1(a1), b1(b1), b2(b2) {}
    double next(double u) override {
        setY(a1 * current() + b1 * u + b2 * u2);
        u2 = u1;     // сдвигаем историю входа
        u1 = u;
        return current();
    }
    void reset() override {
        Model::reset();
        u1 = 0.0;
        u2 = 0.0;
    }
    std::string name() const override { return "Model 1.10 (linear)"; }

    // Анализ устойчивости: корень характеристического уравнения равен a1.
    // Система устойчива, если модуль корня меньше единицы.
    bool isStable() const { return std::fabs(a1) < 1.0; }
    double root() const { return a1; }
};

// Model 2.4 (Logarithmic Multi-Variable Interaction).
// Обратная связь через косинус выхода и логарифмическая реакция на модуль входа.
class NonlinearModel : public Model {
    double a;
    double b;
public:
    NonlinearModel(double a, double b, double y0 = 0.0) : Model(y0), a(a), b(b) {}
    double next(double u) override {
        double y = current();
        setY(a * y * std::cos(y) + b * std::log(1.0 + std::fabs(u)));
        return current();
    }
    std::string name() const override { return "Model 2.4 (nonlinear)"; }
};

// Model 3.8 (Combined Linear-Quadratic Decay), решение явным методом Эйлера.
// Линейное и квадратичное затухание выхода плюс внешнее воздействие.
class DiffModel : public Model {
    double a1;
    double a2;
    double b;
    double dt;
public:
    DiffModel(double a1, double a2, double b, double dt, double y0 = 0.0)
        : Model(y0), a1(a1), a2(a2), b(b), dt(dt) {}
    double next(double u) override {
        double y = current();
        setY(y + dt * (-a1 * y - a2 * y * y + b * u));
        return current();
    }
    std::string name() const override { return "Model 3.8 (Euler)"; }
};

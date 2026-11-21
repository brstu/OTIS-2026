// Тесты моделей из ЛР №1 (Model11, Model28, Model36 подключаются из task_01).
#include <gtest/gtest.h>

#include <cmath>
#include <tuple>
#include <vector>

#include "Simulator.h"
#include "InputSignal.h"
#include "model11.h"
#include "model28.h"
#include "model36.h"

namespace {
constexpr double kTol = 1e-9;
}

// ---------- 1. Значения, посчитанные вручную ----------

TEST(Model11Values, StepHandComputed) {
    // y(t+1) = 0.5*y + 2*u, u = 1: 2; 3; 3.5
    Model11 m(0.5, 2.0);
    EXPECT_NEAR(m.nextStep(1.0, 0.1), 2.0, kTol);
    EXPECT_NEAR(m.nextStep(1.0, 0.1), 3.0, kTol);
    EXPECT_NEAR(m.nextStep(1.0, 0.1), 3.5, kTol);
}

TEST(Model11Values, PulseHandComputed) {
    // a = 0.5, b = 1, u0 = 1: 1; 0.5; 0.25
    Model11 m(0.5, 1.0);
    EXPECT_NEAR(m.nextStep(1.0, 0.1), 1.0, kTol);
    EXPECT_NEAR(m.nextStep(0.0, 0.1), 0.5, kTol);
    EXPECT_NEAR(m.nextStep(0.0, 0.1), 0.25, kTol);
}

TEST(Model28Values, StepHandComputed) {
    // a = b = c = 1, u = 1:
    // y1 = 0 + 1 + sin(0) = 1;  y2 = 1*(1-1) + 1 + sin(0) = 1;
    // y3 = 0 + 1 + sin(1*1) = 1 + sin(1);  y4 = y3*(1-y3) + 1 + sin(1)
    Model28 m(1.0, 1.0, 1.0);
    EXPECT_NEAR(m.nextStep(1.0, 0.1), 1.0, kTol);
    EXPECT_NEAR(m.nextStep(1.0, 0.1), 1.0, kTol);
    const double y3 = 1.0 + std::sin(1.0);
    EXPECT_NEAR(m.nextStep(1.0, 0.1), y3, kTol);
    EXPECT_NEAR(m.nextStep(1.0, 0.1), 0.2919265817264287, 1e-12);
}

TEST(Model36Values, EulerHandComputed) {
    // a = b = 1, u = 1, dt = 0.1:
    // y1 = 0 + 0.1*(0 + 1) = 0.1;  y2 = 0.1 + 0.1*(0.001 + 1) = 0.2001
    Model36 m(1.0, 1.0);
    EXPECT_NEAR(m.nextStep(1.0, 0.1), 0.1, kTol);
    EXPECT_NEAR(m.nextStep(1.0, 0.1), 0.2001, kTol);
}

// ---------- 2. Инварианты: покой при u = 0, y(0) = 0 ----------

TEST(ModelInvariants, ZeroInputKeepsRest) {
    Model11 m1(0.7, 3.0);
    Model28 m2(2.5, 1.0, 4.0);
    Model36 m3(-1.0, 2.0);
    for (int i = 0; i < 50; ++i) {
        EXPECT_DOUBLE_EQ(m1.nextStep(0.0, 0.1), 0.0);
        EXPECT_DOUBLE_EQ(m2.nextStep(0.0, 0.1), 0.0);
        EXPECT_DOUBLE_EQ(m3.nextStep(0.0, 0.1), 0.0);
    }
}

TEST(ModelInvariants, ResetReturnsToInitialState) {
    Model11 m1(0.5, 2.0);
    Model28 m2(1.0, 1.0, 1.0);
    Model36 m3(1.0, 1.0);
    const double f1 = m1.nextStep(1.0, 0.1), f2 = m2.nextStep(1.0, 0.1), f3 = m3.nextStep(1.0, 0.1);
    for (int i = 0; i < 5; ++i) { m1.nextStep(1.0, 0.1); m2.nextStep(1.0, 0.1); m3.nextStep(1.0, 0.1); }
    m1.reset(); m2.reset(); m3.reset();
    EXPECT_DOUBLE_EQ(m1.nextStep(1.0, 0.1), f1);
    EXPECT_DOUBLE_EQ(m2.nextStep(1.0, 0.1), f2);
    EXPECT_DOUBLE_EQ(m3.nextStep(1.0, 0.1), f3);
}

TEST(ModelInvariants, NamesAreDistinctAndNonEmpty) {
    Model11 m1(0, 0); Model28 m2(0, 0, 0); Model36 m3(0, 0);
    EXPECT_FALSE(m1.getName().empty());
    EXPECT_NE(m1.getName(), m2.getName());
    EXPECT_NE(m2.getName(), m3.getName());
}

TEST(Model28Nonlinearity, ReducesToLinearWhenAandCAreZero) {
    // a = 0, c = 0  =>  y(t+1) = b*u
    Model28 m(0.0, 3.0, 0.0);
    EXPECT_NEAR(m.nextStep(2.0, 0.1), 6.0, kTol);
    EXPECT_NEAR(m.nextStep(-1.0, 0.1), -3.0, kTol);
}

// Базовая реализация Model::reset() (в наследниках она переопределена)
namespace {
class DummyModel : public Model {
public:
    double nextStep(double u, double) override { return y += u; }
    std::string getName() const override { return "dummy"; }
};
}

TEST(ModelBase, DefaultResetZeroesState) {
    DummyModel m;
    EXPECT_DOUBLE_EQ(m.nextStep(5.0, 0.1), 5.0);
    m.reset();
    EXPECT_DOUBLE_EQ(m.nextStep(2.0, 0.1), 2.0);
    EXPECT_EQ(m.getName(), "dummy");
}

// ---------- 3. Граничные значения нелинейности: deadzone() из model.h ----------

TEST(Deadzone, BoundaryPointsGiveZero) {
    EXPECT_DOUBLE_EQ(deadzone(1.0), 0.0);    // u = +delta
    EXPECT_DOUBLE_EQ(deadzone(-1.0), 0.0);   // u = -delta
    EXPECT_DOUBLE_EQ(deadzone(0.0), 0.0);
}

TEST(Deadzone, OutsideZoneIsShiftedByThreshold) {
    EXPECT_DOUBLE_EQ(deadzone(3.0), 2.0);
    EXPECT_DOUBLE_EQ(deadzone(-3.0), -2.0);
    EXPECT_NEAR(deadzone(0.6, 0.5), 0.1, kTol);
    EXPECT_NEAR(deadzone(-0.6, 0.5), -0.1, kTol);
}

// ---------- 4. Параметризованные тесты (TEST_P): сходимость без дублирования кода ----------

// Model11: при |a| < 1 выход сходится к y* = b*A/(1-a) для ступенчатого воздействия.
using LinearParams = std::tuple<double /*a*/, double /*b*/, double /*A*/>;
class Model11Convergence : public ::testing::TestWithParam<LinearParams> {};

TEST_P(Model11Convergence, StepConvergesToSteadyState) {
    const auto [a, b, A] = GetParam();
    Model11 m(a, b);
    StepSignal step(A);
    const auto s = simulate(m, step, 2000, 0.1);
    EXPECT_NEAR(s.back().y, b * A / (1.0 - a), 1e-6);
}

TEST_P(Model11Convergence, PulseDecaysToZero) {
    const auto [a, b, A] = GetParam();
    Model11 m(a, b);
    PulseSignal pulse(A);
    const auto s = simulate(m, pulse, 2000, 0.1);
    EXPECT_NEAR(s.front().y, b * A, kTol);   // первая реакция: y(0) = b*A
    EXPECT_NEAR(s.back().y, 0.0, 1e-6);
}

INSTANTIATE_TEST_SUITE_P(StableCoefficients, Model11Convergence,
    ::testing::Values(
        LinearParams{0.5, 1.0, 1.0},
        LinearParams{-0.5, 2.0, 3.0},
        LinearParams{0.9, 0.1, 5.0},
        LinearParams{0.0, 4.0, 1.0},
        LinearParams{-0.9, 1.0, -2.0}));

// Model11: при |a| > 1 процесс расходится.
class Model11Divergence : public ::testing::TestWithParam<double> {};

TEST_P(Model11Divergence, UnstableCoefficientGrows) {
    Model11 m(GetParam(), 1.0);
    PulseSignal pulse(1.0);
    const auto s = simulate(m, pulse, 60, 0.1);
    EXPECT_GT(std::fabs(s.back().y), 1e3);
}

INSTANTIATE_TEST_SUITE_P(UnstableCoefficients, Model11Divergence,
    ::testing::Values(1.5, -1.5, 2.0, -3.0));

// Model36 (метод Эйлера): при a < 0 равновесие y* = cbrt(-b*u/a), сходимость при малом dt.
using EulerParams = std::tuple<double /*a*/, double /*b*/, double /*u*/, double /*dt*/>;
class Model36Convergence : public ::testing::TestWithParam<EulerParams> {};

TEST_P(Model36Convergence, EulerConvergesToEquilibrium) {
    const auto [a, b, u, dt] = GetParam();
    Model36 m(a, b);
    StepSignal step(u);
    const auto s = simulate(m, step, 20000, dt);
    EXPECT_NEAR(s.back().y, std::cbrt(-b * u / a), 1e-4);
}

INSTANTIATE_TEST_SUITE_P(DifferentSteps, Model36Convergence,
    ::testing::Values(
        EulerParams{-1.0, 1.0, 1.0, 0.1},
        EulerParams{-1.0, 1.0, 1.0, 0.01},
        EulerParams{-2.0, 2.0, 1.0, 0.05},
        EulerParams{-1.0, 8.0, 1.0, 0.02},
        EulerParams{-0.5, 1.0, -4.0, 0.05}));

// Model36: при a = 0 уравнение dy/dt = b*u решается Эйлером точно: y(k) = b*u*dt*k.
using ExactParams = std::tuple<double /*b*/, double /*u*/, double /*dt*/>;
class Model36AnalyticSolution : public ::testing::TestWithParam<ExactParams> {};

TEST_P(Model36AnalyticSolution, EqualsAnalyticForZeroA) {
    const auto [b, u, dt] = GetParam();
    Model36 m(0.0, b);
    StepSignal step(u);
    const auto s = simulate(m, step, 50, dt);
    for (const auto& p : s) {
        const double t = (p.tau + 1) * dt;   // после (tau+1)-го шага
        EXPECT_NEAR(p.y, b * u * t, 1e-9);
    }
}

INSTANTIATE_TEST_SUITE_P(Slopes, Model36AnalyticSolution,
    ::testing::Values(ExactParams{1.0, 1.0, 0.1}, ExactParams{2.0, -3.0, 0.05}, ExactParams{0.5, 4.0, 0.2}));

// Model36: ошибка метода Эйлера уменьшается пропорционально шагу (порядок O(dt)).
TEST(Model36Accuracy, ErrorShrinksWithStep) {
    // y' = -y^3 + 1, y(0) = 0; эталон - тот же метод с очень малым шагом
    auto finalValue = [](double dt) {
        Model36 m(-1.0, 1.0);
        double y = 0;
        const int steps = static_cast<int>(std::round(2.0 / dt));
        for (int i = 0; i < steps; ++i) y = m.nextStep(1.0, dt);
        return y;
    };
    const double ref = finalValue(1e-5);
    const double e1 = std::fabs(finalValue(0.1) - ref);
    const double e2 = std::fabs(finalValue(0.05) - ref);
    EXPECT_LT(e2, e1);
    EXPECT_NEAR(e1 / e2, 2.0, 0.3);   // метод Эйлера - первого порядка
}

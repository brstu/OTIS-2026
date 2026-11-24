// Значения моделей (ручной расчёт), граничные значения и инварианты.
#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <string>

#include "InputSignals.h"
#include "Model1_7.h"
#include "Model2_9.h"
#include "Model3_1.h"
#include "Simulation.h"

namespace {
// Прогон модели на последовательности входов
std::vector<double> run(Model& m, const std::vector<double>& u) {
    std::vector<double> y;
    for (double x : u) y.push_back(m.next(x));
    return y;
}
}  // namespace

// =============== Model 1.7 ===============
// a=0.5, b1=0.4, b2=0.2, b3=0.1:  y1 = 0.4, y2 = 0.5*0.4+0.4+0.2 = 0.8, y3 = 1.1, y4 = 1.25
TEST(Model1_7Values, StepResponseMatchesHandCalculation) {
    Model1_7 m(0.5, 0.4, 0.2, 0.1);
    const auto y = run(m, {1, 1, 1, 1, 1});
    const double expected[] = {0.4, 0.8, 1.1, 1.25, 1.325};
    for (int i = 0; i < 5; ++i) EXPECT_NEAR(y[i], expected[i], 1e-12) << "tau=" << i + 1;
}

TEST(Model1_7Values, ImpulseResponseMatchesHandCalculation) {
    Model1_7 m(0.5, 0.4, 0.2, 0.1);
    const auto y = run(m, {1, 0, 0, 0, 0});
    const double expected[] = {0.4, 0.4, 0.3, 0.15, 0.075};
    for (int i = 0; i < 5; ++i) EXPECT_NEAR(y[i], expected[i], 1e-12) << "tau=" << i + 1;
}

TEST(Model1_7Values, PureControlHistoryWhenAIsZero) {
    // a=0, b=(1,1,1), step: y = сумма трёх последних входов = 1, 2, 3, 3
    Model1_7 m(0.0, 1.0, 1.0, 1.0);
    const auto y = run(m, {1, 1, 1, 1});
    EXPECT_DOUBLE_EQ(y[0], 1.0);
    EXPECT_DOUBLE_EQ(y[1], 2.0);
    EXPECT_DOUBLE_EQ(y[2], 3.0);
    EXPECT_DOUBLE_EQ(y[3], 3.0);
}

TEST(Model1_7Invariants, RestStateWithZeroInput) {
    Model1_7 m(0.9, 1.0, 2.0, 3.0);
    for (double y : run(m, {0, 0, 0, 0, 0, 0})) EXPECT_DOUBLE_EQ(y, 0.0);
}

TEST(Model1_7Invariants, ResetClearsOutputAndControlHistory) {
    Model1_7 m(0.5, 0.4, 0.2, 0.1);
    run(m, {1, 1, 1});
    m.reset();
    EXPECT_NEAR(m.next(1.0), 0.4, 1e-12);  // как с нуля: истории u нет
}

TEST(Model1_7Invariants, SteadyStateIsSumOfBOverOneMinusA) {
    Model1_7 m(0.5, 0.4, 0.2, 0.1);
    double y = 0.0;
    for (int i = 0; i < 100; ++i) y = m.next(1.0);
    EXPECT_NEAR(y, 0.7 / 0.5, 1e-9);
}

TEST(Model1_7Misc, NameAndStabilityDelegation) {
    EXPECT_NE(Model1_7(0.5, 0, 0, 0).name().find("1.7"), std::string::npos);

    const Model1_7 stable(0.5, 0.4, 0.2, 0.1);
    EXPECT_TRUE(stable.isStable());
    EXPECT_EQ(stable.stabilityWarning(), "");

    const Model1_7 boundary(1.0, 0.4, 0.2, 0.1);
    EXPECT_FALSE(boundary.isStable());
    EXPECT_NE(boundary.stabilityWarning().find("boundary"), std::string::npos);

    const Model1_7 unstable(1.5, 0.4, 0.2, 0.1);
    EXPECT_FALSE(unstable.isStable());
    EXPECT_NE(unstable.stabilityWarning().find("UNSTABLE"), std::string::npos);
}

// =============== Model 2.9 ===============
// a=0.6, b=0.5, step u=1: y1 = 0.5, y2 = 0.8, y3 = 0.98
TEST(Model2_9Values, StepResponseMatchesHandCalculation) {
    Model2_9 m(0.6, 0.5);
    const auto y = run(m, {1, 1, 1});
    EXPECT_NEAR(y[0], 0.5, 1e-12);
    EXPECT_NEAR(y[1], 0.8, 1e-12);
    EXPECT_NEAR(y[2], 0.98, 1e-12);
}

TEST(Model2_9Values, ImpulseResponseMatchesHandCalculation) {
    Model2_9 m(0.6, 0.5);
    const auto y = run(m, {1, 0, 0});
    EXPECT_NEAR(y[0], 0.5, 1e-12);
    EXPECT_NEAR(y[1], 0.3, 1e-12);
    EXPECT_NEAR(y[2], 0.18, 1e-12);
}

TEST(Model2_9Values, SquareRootOfFour) {
    Model2_9 m(0.0, 0.5);  // y = 0.5 * sqrt(4) = 1
    EXPECT_DOUBLE_EQ(m.next(4.0), 1.0);
}

// Граничные значения нелинейности sqrt(|u|)*sign(u)
TEST(Model2_9Boundary, ZeroInputGivesZeroSignAndSqrt) {
    Model2_9 m(0.0, 5.0);
    EXPECT_DOUBLE_EQ(m.next(0.0), 0.0);
}

TEST(Model2_9Boundary, UnitInputPassesThroughUnchanged) {
    Model2_9 pos(0.0, 1.0), neg(0.0, 1.0);
    EXPECT_DOUBLE_EQ(pos.next(1.0), 1.0);   // sqrt(1) = 1
    EXPECT_DOUBLE_EQ(neg.next(-1.0), -1.0);
}

TEST(Model2_9Boundary, SmallInputsAreAmplifiedLargeOnesCompressed) {
    Model2_9 small(0.0, 1.0), large(0.0, 1.0);
    EXPECT_GT(small.next(0.01), 0.01);     // sqrt(0.01) = 0.1 > 0.01
    EXPECT_LT(large.next(100.0), 100.0);   // sqrt(100) = 10 < 100
}

TEST(Model2_9Boundary, TinyInputDoesNotProduceNaN) {
    Model2_9 m(0.0, 1.0);
    const double y = m.next(1e-300);
    EXPECT_TRUE(std::isfinite(y));
    EXPECT_GT(y, 0.0);
}

TEST(Model2_9Invariants, OddSymmetryOfNonlinearity) {
    Model2_9 p(0.0, 0.5), n(0.0, 0.5);
    EXPECT_DOUBLE_EQ(p.next(4.0), 1.0);
    EXPECT_DOUBLE_EQ(n.next(-4.0), -1.0);
}

TEST(Model2_9Invariants, RestStateWithZeroInput) {
    Model2_9 m(0.7, 3.0);
    for (double y : run(m, {0, 0, 0, 0})) EXPECT_DOUBLE_EQ(y, 0.0);
}

TEST(Model2_9Invariants, ResetReturnsToZero) {
    Model2_9 m(0.6, 0.5);
    run(m, {1, 1, 1});
    m.reset();
    EXPECT_NEAR(m.next(1.0), 0.5, 1e-12);
}

TEST(Model2_9Misc, NameAndStabilityFollowsCoefficientA) {
    EXPECT_NE(Model2_9(0.5, 1).name().find("2.9"), std::string::npos);
    EXPECT_TRUE(Model2_9(0.6, 0.5).isStable());
    EXPECT_EQ(Model2_9(0.6, 0.5).stabilityWarning(), "");
    EXPECT_FALSE(Model2_9(1.5, 0.5).isStable());
    EXPECT_NE(Model2_9(1.5, 0.5).stabilityWarning().find("Model 2.9"), std::string::npos);
    EXPECT_FALSE(Model2_9(1.0, 0.5).isStable());
}

// =============== Model 3.1 (метод Эйлера) ===============
// a=0.8, dt=0.05: множитель 1 - 0.04 = 0.96
TEST(Model3_1Values, EulerStepsMatchHandCalculation) {
    Model3_1 m(0.8, 0.05);
    const auto y = run(m, {0, 0, 0});
    EXPECT_NEAR(y[0], 0.96, 1e-12);
    EXPECT_NEAR(y[1], 0.9216, 1e-12);
    EXPECT_NEAR(y[2], 0.884736, 1e-12);
}

TEST(Model3_1Values, InputSignalIsIgnored) {
    Model3_1 a(0.8, 0.05), b(0.8, 0.05);
    EXPECT_DOUBLE_EQ(a.next(0.0), b.next(123.0));
}

TEST(Model3_1Values, DefaultTimeStepIsPointZeroOne) {
    Model3_1 m(1.0);  // dt = 0.01 -> y1 = 1 - 0.01
    EXPECT_NEAR(m.next(0.0), 0.99, 1e-12);
}

TEST(Model3_1Invariants, ZeroDecayKeepsInitialState) {
    Model3_1 m(0.0, 0.1);
    for (double y : run(m, {0, 0, 0})) EXPECT_DOUBLE_EQ(y, 1.0);
}

TEST(Model3_1Invariants, DecaysMonotonicallyForSmallStep) {
    Model3_1 m(0.8, 0.05);
    double prev = 1.0;
    for (int i = 0; i < 50; ++i) {
        const double y = m.next(0.0);
        EXPECT_LT(y, prev);
        EXPECT_GT(y, 0.0);
        prev = y;
    }
}

TEST(Model3_1Invariants, ResetRestoresInitialState) {
    Model3_1 m(0.8, 0.05);
    m.next(0.0);
    m.reset();
    EXPECT_NEAR(m.next(0.0), 0.96, 1e-12);
}

// Сравнение с аналитическим решением y(t) = exp(-a t), a=1, t=1
double eulerErrorAtT1(double dt) {
    Model3_1 m(1.0, dt);
    const int n = static_cast<int>(std::lround(1.0 / dt));
    double y = 0.0;
    for (int i = 0; i < n; ++i) y = m.next(0.0);
    return std::abs(y - std::exp(-1.0));
}

TEST(Model3_1Analytic, CloseToExponentialSolution) {
    EXPECT_NEAR(eulerErrorAtT1(0.1), 0.0192, 1e-3);      // 0.9^10 = 0.34868 vs 0.36788
    EXPECT_NEAR(eulerErrorAtT1(0.01), 0.00185, 1e-4);    // 0.99^100 = 0.36603
    EXPECT_LT(eulerErrorAtT1(0.001), 2e-4);
}

TEST(Model3_1Analytic, ErrorIsFirstOrderInDt) {
    // метод Эйлера: ошибка ~ dt, при уменьшении шага в 10 раз ошибка падает ~ в 10 раз
    const double ratio = eulerErrorAtT1(0.01) / eulerErrorAtT1(0.001);
    EXPECT_GT(ratio, 8.0);
    EXPECT_LT(ratio, 12.0);
}

TEST(Model3_1Misc, NameAndStability) {
    EXPECT_NE(Model3_1(0.8).name().find("3.1"), std::string::npos);
    EXPECT_TRUE(Model3_1(0.8, 0.05).isStable());
    EXPECT_EQ(Model3_1(0.8, 0.05).stabilityWarning(), "");
    EXPECT_FALSE(Model3_1(1.0, 2.5).isStable());                 // a*dt > 2
    EXPECT_FALSE(Model3_1(1.0, 2.0).isStable());                 // граница
    EXPECT_FALSE(Model3_1(-1.0, 0.1).isStable());                // растущий объект
    EXPECT_FALSE(Model3_1(1.0, 0.0).isStable());                 // dt = 0
    EXPECT_NE(Model3_1(-1.0, 0.1).stabilityWarning().find("object itself"), std::string::npos);
}

// =============== Входные сигналы ===============
TEST(Signals, StepIsConstant) {
    StepInput def;
    EXPECT_DOUBLE_EQ(def.value(1), 1.0);
    StepInput s(3.5);
    EXPECT_DOUBLE_EQ(s.value(1), 3.5);
    EXPECT_DOUBLE_EQ(s.value(1000), 3.5);
    EXPECT_NE(s.name().find("Step"), std::string::npos);
}

TEST(Signals, ImpulseIsOneOnlyAtFirstStep) {
    ImpulseInput s;
    EXPECT_DOUBLE_EQ(s.value(1), 1.0);
    EXPECT_DOUBLE_EQ(s.value(0), 0.0);
    EXPECT_DOUBLE_EQ(s.value(2), 0.0);
    EXPECT_DOUBLE_EQ(s.value(100), 0.0);
    EXPECT_NE(s.name().find("Impulse"), std::string::npos);
}

TEST(Signals, HarmonicIsSineOfTau) {
    HarmonicInput s;
    for (int tau = 0; tau <= 10; ++tau) EXPECT_NEAR(s.value(tau), std::sin(tau), 1e-12);
    EXPECT_NE(s.name().find("Harmonic"), std::string::npos);
}

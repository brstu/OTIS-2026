#include <gtest/gtest.h>

#include <cmath>
#include <string>
#include <vector>

#include "Model1_10.h"
#include "Model2_2.h"
#include "Model3_4.h"
#include "Simulation.h"

// ===================================================== Model 1.10 (линейная)
// y(t+1) = a1*y(t) + b1*u(t) + b2*u(t-2)

TEST(Model1_10, NextMatchesHandCalculation) {
    // a1=0.5, b1=2, b2=3; y=4, u=1, u(t-2)=2  ->  0.5*4 + 2*1 + 3*2 = 10
    const Model1_10 model(0.5, 2.0, 3.0);

    EXPECT_DOUBLE_EQ(model.next(4.0, 1.0, 2.0, 0.1), 10.0);
}

TEST(Model1_10, DtDoesNotAffectDiscreteModel) {
    const Model1_10 model(0.5, 2.0, 3.0);

    EXPECT_DOUBLE_EQ(model.next(4.0, 1.0, 2.0, 0.1),
                     model.next(4.0, 1.0, 2.0, 123.0));
}

TEST(Model1_10, Name) {
    EXPECT_EQ(Model1_10(0.5, 1.0, 1.0).name(), "Linear Model 1.10");
}

TEST(Model1_10, CharacteristicPolynomialIsZMinusA1) {
    const std::vector<double> expected{1.0, -0.7};

    EXPECT_EQ(Model1_10(0.7, 1.0, 1.0).characteristicPolynomial(), expected);
}

TEST(Model1_10, StableModelHasNoWarning) {
    const Model1_10 model(0.5, 1.0, 1.0);

    EXPECT_TRUE(model.isStable());
    EXPECT_TRUE(model.stabilityWarning().empty());
}

TEST(Model1_10, DivergentModelIsUnstableAndWarns) {
    const Model1_10 model(1.5, 1.0, 1.0);

    EXPECT_FALSE(model.isStable());

    const std::string warning = model.stabilityWarning();
    EXPECT_NE(warning.find("WARNING"), std::string::npos);
    EXPECT_NE(warning.find("Linear Model 1.10"), std::string::npos);
}

TEST(Model1_10, NegativePoleBeyondUnitCircleIsUnstable) {
    EXPECT_FALSE(Model1_10(-1.5, 1.0, 1.0).isStable());
}

TEST(Model1_10, BoundaryPolesAreNotStable) {
    EXPECT_FALSE(Model1_10(1.0, 1.0, 1.0).isStable());
    EXPECT_FALSE(Model1_10(-1.0, 1.0, 1.0).isStable());
}

TEST(Model1_10, InputCoefficientsDoNotInfluenceStability) {
    EXPECT_TRUE(Model1_10(0.5, 1000.0, -1000.0).isStable());
}

// ================================================ Model 2.2 (насыщение входа)
// y(t+1) = a*y(t) + b*sat(u),  sat(u) = clamp(u, uMin, uMax)

struct SaturationCase {
    double u;
    double expectedNext;
};

class Model2_2Saturation : public ::testing::TestWithParam<SaturationCase> {};

// a = 0.5, b = 2, [uMin; uMax] = [-1; 1], y = 2  ->  1 + 2*sat(u)
TEST_P(Model2_2Saturation, NextRespectsSaturationLimits) {
    const Model2_2 model(0.5, 2.0, -1.0, 1.0);
    const auto param = GetParam();

    EXPECT_DOUBLE_EQ(model.next(2.0, param.u, 0.0, 0.1), param.expectedNext)
        << "u = " << param.u;
}

INSTANTIATE_TEST_SUITE_P(
    BoundaryValues,
    Model2_2Saturation,
    ::testing::Values(
        SaturationCase{1.0, 3.0},     // u = Umax
        SaturationCase{-1.0, -1.0},   // u = Umin
        SaturationCase{2.0, 3.0},     // выше Umax -> ограничено
        SaturationCase{-3.0, -1.0},   // ниже Umin -> ограничено
        SaturationCase{0.5, 2.0},     // внутри диапазона
        SaturationCase{0.0, 1.0}      // ноль
    )
);

TEST(Model2_2, IgnoresDelayedInputAndDt) {
    const Model2_2 model(0.5, 2.0, -1.0, 1.0);

    EXPECT_DOUBLE_EQ(model.next(2.0, 0.5, 0.0, 0.1),
                     model.next(2.0, 0.5, 99.0, 5.0));
}

TEST(Model2_2, Name) {
    EXPECT_EQ(Model2_2(0.5, 1.0, -1.0, 1.0).name(), "Nonlinear Model 2.2");
}

TEST(Model2_2, StabilityDependsOnLinearPartOnly) {
    EXPECT_TRUE(Model2_2(0.9, 1.0, -1.0, 1.0).isStable());
    EXPECT_FALSE(Model2_2(1.2, 1.0, -1.0, 1.0).isStable());
    EXPECT_FALSE(Model2_2(-1.0, 1.0, -1.0, 1.0).isStable());
}

TEST(Model2_2, WarningForDivergentLinearPart) {
    const Model2_2 model(1.2, 1.0, -1.0, 1.0);

    EXPECT_NE(model.stabilityWarning().find("Nonlinear Model 2.2"),
              std::string::npos);
    EXPECT_TRUE(Model2_2(0.5, 1.0, -1.0, 1.0).stabilityWarning().empty());
}

// ================================================== Model 3.4 (дифференциальная)
// y(t+1) = y - dt*a*y^2   (метод Эйлера для dy/dt = -a*y^2)

TEST(Model3_4, NextMatchesHandCalculation) {
    // a = 1, y = 2, dt = 0.1 -> 2 - 0.1*1*4 = 1.6
    const Model3_4 model(1.0);

    EXPECT_DOUBLE_EQ(model.next(2.0, 0.0, 0.0, 0.1), 1.6);
}

TEST(Model3_4, IgnoresInputs) {
    const Model3_4 model(1.0);

    EXPECT_DOUBLE_EQ(model.next(2.0, 0.0, 0.0, 0.1),
                     model.next(2.0, 55.0, -7.0, 0.1));
}

TEST(Model3_4, ZeroCoefficientKeepsState) {
    EXPECT_DOUBLE_EQ(Model3_4(0.0).next(3.0, 0.0, 0.0, 0.1), 3.0);
}

TEST(Model3_4, Name) {
    EXPECT_EQ(Model3_4(1.0).name(), "Differential Model 3.4");
}

TEST(Model3_4, NonlinearModelIsNotAnalysedByRoots) {
    const Model3_4 model(1.0);

    EXPECT_TRUE(model.characteristicPolynomial().empty());
    EXPECT_TRUE(model.isStable());
    EXPECT_TRUE(model.stabilityWarning().empty());
}

// Сравнение шага Эйлера с аналитическим решением:
// dy/dt = -a*y^2, y(0)=y0  ->  y(t) = y0 / (1 + a*y0*t)
namespace {

double eulerError(double a, double y0, double dt, double tEnd) {
    const Model3_4 model(a);
    const int steps = static_cast<int>(std::lround(tEnd / dt));
    const std::vector<double> u(static_cast<std::size_t>(steps) + 1, 0.0);

    const std::vector<double> y = simulate(model, u, y0, dt);

    const double analytic = y0 / (1.0 + a * y0 * tEnd);
    return std::fabs(y.back() - analytic);
}

} // namespace

TEST(Model3_4, EulerStepConvergesToAnalyticSolution) {
    EXPECT_LT(eulerError(1.0, 2.0, 0.001, 1.0), 2e-3);
}

TEST(Model3_4, EulerErrorShrinksLinearlyWithStep) {
    // Метод Эйлера имеет первый порядок: dt/2 -> погрешность примерно вдвое меньше
    const double coarse = eulerError(1.0, 2.0, 0.01, 1.0);
    const double fine = eulerError(1.0, 2.0, 0.005, 1.0);

    const double ratio = coarse / fine;
    EXPECT_GT(ratio, 1.8);
    EXPECT_LT(ratio, 2.2);
}

// ===================================== Инварианты (нулевой вход, нулевое состояние)

TEST(Invariants, ZeroInputAndZeroStateStayAtRest) {
    EXPECT_DOUBLE_EQ(Model1_10(0.7, 1.0, 1.0).next(0.0, 0.0, 0.0, 0.1), 0.0);
    EXPECT_DOUBLE_EQ(Model2_2(0.7, 1.0, -1.0, 1.0).next(0.0, 0.0, 0.0, 0.1), 0.0);
    EXPECT_DOUBLE_EQ(Model3_4(1.0).next(0.0, 0.0, 0.0, 0.1), 0.0);
}

TEST(Invariants, WholeSimulationStaysZeroForZeroInputAndState) {
    const std::vector<double> u(20, 0.0);

    for (double value : simulate(Model1_10(0.7, 1.0, 1.0), u, 0.0, 0.1)) {
        EXPECT_DOUBLE_EQ(value, 0.0);
    }

    for (double value : simulate(Model2_2(0.7, 1.0, -1.0, 1.0), u, 0.0, 0.1)) {
        EXPECT_DOUBLE_EQ(value, 0.0);
    }
}

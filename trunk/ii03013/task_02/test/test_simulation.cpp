#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "Model1_10.h"
#include "Simulation.h"

using ::testing::DoubleEq;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::_;

// ============================================================================
// TEST_P: расчёт шагов симуляции Model1_10 на разных наборах данных
// (один код проверки - много входных наборов, без дублирования)
// ============================================================================

struct StepCase {
    const char* name;
    double a1, b1, b2;
    InputSignalType signal;
    double amplitude;
    double initialY;
    int steps;
    std::vector<double> expected;   // рассчитано вручную
};

// Чтобы gtest печатал понятное имя набора при падении
void PrintTo(const StepCase& c, std::ostream* os) {
    *os << c.name;
}

class Model1_10Steps : public ::testing::TestWithParam<StepCase> {};

TEST_P(Model1_10Steps, SimulationMatchesHandCalculatedValues) {
    const StepCase& c = GetParam();

    const Model1_10 model(c.a1, c.b1, c.b2);
    const std::vector<double> u =
        buildInputSignal(c.signal, c.steps, 0.1, c.amplitude, 1.0);

    const std::vector<double> y = simulate(model, u, c.initialY, 0.1);

    ASSERT_EQ(y.size(), c.expected.size());
    for (std::size_t i = 0; i < y.size(); ++i) {
        EXPECT_NEAR(y[i], c.expected[i], 1e-12) << "step " << i;
    }
}

INSTANTIATE_TEST_SUITE_P(
    ConstantAndImpulse,
    Model1_10Steps,
    ::testing::Values(
        // y1 = 1; y2 = .5+1 = 1.5; y3 = .75+1 = 1.75; y4 = .875+1 = 1.875
        StepCase{"constant_no_delay", 0.5, 1.0, 0.0,
                 InputSignalType::Constant, 1.0, 0.0, 4,
                 {0.0, 1.0, 1.5, 1.75, 1.875}},

        // запаздывающий вход u(t-2) включается со 2-го шага:
        // y1 = 1; y2 = 1.5; y3 = .75+1+2 = 3.75; y4 = 1.875+1+2 = 4.875
        StepCase{"constant_with_delay", 0.5, 1.0, 2.0,
                 InputSignalType::Constant, 1.0, 0.0, 4,
                 {0.0, 1.0, 1.5, 3.75, 4.875}},

        // импульс амплитуды 2: u = {2,0,0,0,0}
        // y1 = 2; y2 = 1; y3 = .5 + 0 + 1*u0 = 2.5; y4 = 1.25
        StepCase{"impulse_with_delay", 0.5, 1.0, 1.0,
                 InputSignalType::Impulse, 2.0, 0.0, 4,
                 {0.0, 2.0, 1.0, 2.5, 1.25}},

        // отрицательный полюс, ненулевое начальное состояние
        // y1 = -.5+1 = .5; y2 = -.25+1 = .75; y3 = .625; y4 = .6875
        StepCase{"negative_pole_nonzero_start", -0.5, 1.0, 0.0,
                 InputSignalType::Constant, 1.0, 1.0, 4,
                 {1.0, 0.5, 0.75, 0.625, 0.6875}},

        // свободное движение (u = 0): y_{t+1} = 0.5*y_t
        StepCase{"free_response_zero_amplitude", 0.5, 1.0, 1.0,
                 InputSignalType::Constant, 0.0, 8.0, 3,
                 {8.0, 4.0, 2.0, 1.0}}
    ),
    [](const ::testing::TestParamInfo<StepCase>& info) {
        return std::string(info.param.name);
    }
);

// ---------------------------------------------------------------------------
// TEST_P: гармоническое воздействие. При a1 = 0, b1 = 1, b2 = 0 модель
// превращается в y[t+1] = u[t] = A*sin(omega*t*dt)
// ---------------------------------------------------------------------------

struct HarmonicCase {
    double amplitude;
    double omega;
    double dt;
};

class HarmonicSteps : public ::testing::TestWithParam<HarmonicCase> {};

TEST_P(HarmonicSteps, OutputFollowsDelayedSine) {
    const HarmonicCase c = GetParam();
    const int steps = 50;

    const Model1_10 model(0.0, 1.0, 0.0);
    const std::vector<double> u =
        buildInputSignal(InputSignalType::Harmonic, steps, c.dt, c.amplitude, c.omega);
    const std::vector<double> y = simulate(model, u, 0.0, c.dt);

    ASSERT_EQ(y.size(), static_cast<std::size_t>(steps) + 1);
    EXPECT_DOUBLE_EQ(y[0], 0.0);

    for (int t = 0; t < steps; ++t) {
        const double expected = c.amplitude * std::sin(c.omega * t * c.dt);
        EXPECT_NEAR(y[static_cast<std::size_t>(t) + 1], expected, 1e-12) << "t = " << t;
    }
}

INSTANTIATE_TEST_SUITE_P(
    Frequencies,
    HarmonicSteps,
    ::testing::Values(
        HarmonicCase{1.0, 1.0, 0.1},
        HarmonicCase{2.5, 0.5, 0.1},
        HarmonicCase{1.0, 10.0, 0.01},
        HarmonicCase{-3.0, 2.0, 0.2}
    )
);

// ============================================================================
// Подмена входных сигналов: ступенчатый / импульсный / гармонический
// (сигнал задаётся вручную, без generateInput)
// ============================================================================

TEST(InputSubstitution, StepSignalGivesGeometricApproachToSteadyState) {
    // y = 0.5*y + 1*u + 1*u(t-2), u = 1 -> y_ss = (b1+b2)/(1-a1) = 4
    const Model1_10 model(0.5, 1.0, 1.0);
    const std::vector<double> u(200, 1.0);

    const std::vector<double> y = simulate(model, u, 0.0, 0.1);

    EXPECT_NEAR(y.back(), 4.0, 1e-9);
}

TEST(InputSubstitution, ImpulseResponseOfStableModelDecaysToZero) {
    const Model1_10 model(0.5, 1.0, 0.0);
    std::vector<double> u(60, 0.0);
    u[0] = 1.0;

    const std::vector<double> y = simulate(model, u, 0.0, 0.1);

    EXPECT_DOUBLE_EQ(y[1], 1.0);
    EXPECT_DOUBLE_EQ(y[2], 0.5);
    EXPECT_NEAR(y.back(), 0.0, 1e-12);
}

TEST(InputSubstitution, ImpulseResponseOfUnstableModelGrows) {
    const Model1_10 model(1.5, 1.0, 0.0);
    std::vector<double> u(20, 0.0);
    u[0] = 1.0;

    const std::vector<double> y = simulate(model, u, 0.0, 0.1);

    for (std::size_t i = 2; i < y.size(); ++i) {
        EXPECT_GT(std::fabs(y[i]), std::fabs(y[i - 1]));
    }
}

TEST(InputSubstitution, StepResponseOfUnstableModelDiverges) {
    const Model1_10 model(1.2, 1.0, 0.0);
    const std::vector<double> u(60, 1.0);

    const std::vector<double> y = simulate(model, u, 0.0, 0.1);

    EXPECT_GT(y.back(), 1000.0);
}

TEST(InputSubstitution, StabilityVerdictAgreesWithSimulation) {
    const std::vector<double> u(100, 1.0);

    const Model1_10 stable(0.8, 1.0, 0.0);
    const Model1_10 unstable(1.1, 1.0, 0.0);

    EXPECT_TRUE(stable.isStable());
    EXPECT_FALSE(unstable.isStable());
    EXPECT_LT(simulate(stable, u, 0.0, 0.1).back(), 10.0);
    EXPECT_GT(simulate(unstable, u, 0.0, 0.1).back(), 1000.0);
}

// ---------------------------------------------------------------------------
// Граничные случаи simulate() на реальной модели
// ---------------------------------------------------------------------------

TEST(SimulateEdgeCases, EmptyInputGivesEmptyOutput) {
    const Model1_10 model(0.5, 1.0, 0.0);

    EXPECT_TRUE(simulate(model, {}, 3.0, 0.1).empty());
}

TEST(SimulateEdgeCases, SingleSampleKeepsInitialState) {
    const Model1_10 model(0.5, 1.0, 0.0);
    const std::vector<double> y = simulate(model, {1.0}, 3.0, 0.1);

    ASSERT_EQ(y.size(), 1u);
    EXPECT_DOUBLE_EQ(y[0], 3.0);
}

TEST(SimulateEdgeCases, WorksThroughBaseClassPointer) {
    std::unique_ptr<Model> model = std::make_unique<Model1_10>(0.5, 1.0, 0.0);

    model->reset();   // по умолчанию ничего не делает

    EXPECT_DOUBLE_EQ(model->next(2.0, 1.0, 0.0, 0.1), 2.0);   // 0.5*2 + 1*1
    EXPECT_TRUE(model->isStable());
}

// ============================================================================
// Google Mock: проверяем, КАК simulate() обращается к модели
// ============================================================================

class MockModel : public Model {
public:
    MOCK_METHOD(double, next, (double, double, double, double),
                (const, override));
    MOCK_METHOD(std::string, name, (), (const, override));
};

TEST(SimulateWithMock, PassesStateInputDelayedInputAndDtInOrder) {
    MockModel model;
    const std::vector<double> u{1.0, 2.0, 3.0, 4.0};
    const double dt = 0.5;

    {
        InSequence sequence;

        // t = 0: y = 10, u = 1, задержка 0 (t < 2)
        EXPECT_CALL(model, next(DoubleEq(10.0), DoubleEq(1.0), DoubleEq(0.0), DoubleEq(dt)))
            .WillOnce(Return(20.0));
        // t = 1: y = 20, u = 2, задержка 0 (t < 2)
        EXPECT_CALL(model, next(DoubleEq(20.0), DoubleEq(2.0), DoubleEq(0.0), DoubleEq(dt)))
            .WillOnce(Return(30.0));
        // t = 2: y = 30, u = 3, задержка u[0] = 1
        EXPECT_CALL(model, next(DoubleEq(30.0), DoubleEq(3.0), DoubleEq(1.0), DoubleEq(dt)))
            .WillOnce(Return(40.0));
    }

    const std::vector<double> y = simulate(model, u, 10.0, dt);

    const std::vector<double> expected{10.0, 20.0, 30.0, 40.0};
    EXPECT_EQ(y, expected);
}

TEST(SimulateWithMock, EmptyInputDoesNotCallModel) {
    MockModel model;
    EXPECT_CALL(model, next(_, _, _, _)).Times(0);

    EXPECT_TRUE(simulate(model, {}, 5.0, 0.1).empty());
}

TEST(SimulateWithMock, SingleSampleReturnsOnlyInitialState) {
    MockModel model;
    EXPECT_CALL(model, next(_, _, _, _)).Times(0);

    const std::vector<double> y = simulate(model, {1.0}, 5.0, 0.1);

    ASSERT_EQ(y.size(), 1u);
    EXPECT_DOUBLE_EQ(y[0], 5.0);
}

TEST(SimulateWithMock, CallsModelExactlyOncePerStep) {
    MockModel model;
    EXPECT_CALL(model, next(_, _, _, _)).Times(9).WillRepeatedly(Return(0.0));

    simulate(model, std::vector<double>(10, 1.0), 0.0, 0.1);
}

TEST(SimulateWithMock, ModelOutputIsStoredWithoutModification) {
    MockModel model;
    EXPECT_CALL(model, next(_, _, _, _))
        .WillOnce(Return(-1.5))
        .WillOnce(Return(2.25));

    const std::vector<double> y = simulate(model, {0.0, 0.0, 0.0}, 7.0, 0.1);

    const std::vector<double> expected{7.0, -1.5, 2.25};
    EXPECT_EQ(y, expected);
}

// ============================================================================
// buildInputSignal
// ============================================================================

TEST(BuildInputSignal, ProducesStepsPlusOneSamples) {
    EXPECT_EQ(buildInputSignal(InputSignalType::Constant, 10, 0.1, 1.0, 1.0).size(), 11u);
    EXPECT_EQ(buildInputSignal(InputSignalType::Constant, 0, 0.1, 1.0, 1.0).size(), 1u);
}

TEST(BuildInputSignal, ConstantSignalIsFlat) {
    for (double v : buildInputSignal(InputSignalType::Constant, 5, 0.1, 3.0, 1.0)) {
        EXPECT_DOUBLE_EQ(v, 3.0);
    }
}

TEST(BuildInputSignal, ImpulseOnlyAtFirstSample) {
    const auto u = buildInputSignal(InputSignalType::Impulse, 5, 0.1, 3.0, 1.0);

    EXPECT_DOUBLE_EQ(u[0], 3.0);
    for (std::size_t i = 1; i < u.size(); ++i) {
        EXPECT_DOUBLE_EQ(u[i], 0.0);
    }
}

TEST(BuildInputSignal, HarmonicUsesTimeEqualToStepTimesDt) {
    const auto u = buildInputSignal(InputSignalType::Harmonic, 4, 0.5, 2.0, 3.0);

    for (std::size_t i = 0; i < u.size(); ++i) {
        EXPECT_NEAR(u[i], 2.0 * std::sin(3.0 * static_cast<double>(i) * 0.5), 1e-12);
    }
}

TEST(BuildInputSignal, ThrowsOnNegativeSteps) {
    EXPECT_THROW(buildInputSignal(InputSignalType::Constant, -1, 0.1, 1.0, 1.0),
                 std::invalid_argument);
}

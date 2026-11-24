// Параметризованные тесты (TEST_P): шаги симуляции и сходимость без дублирования кода.
#include <gtest/gtest.h>

#include <cmath>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "InputSignals.h"
#include "Model1_7.h"
#include "Model2_9.h"
#include "Model3_1.h"
#include "Simulation.h"
#include "StabilityAnalyzer.h"

// ---------- 1. Значения по шагам: набор (модель, сигнал, ожидаемый y_1..y_k) ----------
struct SimCase {
    std::string label;
    std::function<std::unique_ptr<Model>()> makeModel;
    std::function<std::unique_ptr<InputSignal>()> makeSignal;
    std::vector<double> expectedY;  // первые k значений (посчитаны вручную)
};

void PrintTo(const SimCase& c, std::ostream* os) { *os << c.label; }

class SimulateStepsTest : public ::testing::TestWithParam<SimCase> {};

TEST_P(SimulateStepsTest, OutputMatchesHandCalculation) {
    const SimCase& c = GetParam();
    auto model = c.makeModel();
    auto signal = c.makeSignal();
    const int n = static_cast<int>(c.expectedY.size());

    const auto steps = simulate(*model, *signal, n);

    ASSERT_EQ(static_cast<int>(steps.size()), n);
    for (int i = 0; i < n; ++i) {
        EXPECT_EQ(steps[i].tau, i + 1);
        EXPECT_NEAR(steps[i].u, signal->value(i + 1), 1e-15);
        EXPECT_NEAR(steps[i].y, c.expectedY[i], 1e-9) << c.label << ", tau=" << i + 1;
    }
}

namespace {
template <class M, class... A>
std::function<std::unique_ptr<Model>()> model(A... args) {
    return [=] { return std::unique_ptr<Model>(new M(args...)); };
}
template <class S, class... A>
std::function<std::unique_ptr<InputSignal>()> signal(A... args) {
    return [=] { return std::unique_ptr<InputSignal>(new S(args...)); };
}
}  // namespace

INSTANTIATE_TEST_SUITE_P(
    AllModelsAndSignals, SimulateStepsTest,
    ::testing::Values(
        SimCase{"M1_7_step", model<Model1_7>(0.5, 0.4, 0.2, 0.1), signal<StepInput>(1.0),
                {0.4, 0.8, 1.1, 1.25, 1.325}},
        SimCase{"M1_7_impulse", model<Model1_7>(0.5, 0.4, 0.2, 0.1), signal<ImpulseInput>(),
                {0.4, 0.4, 0.3, 0.15, 0.075}},
        SimCase{"M1_7_harmonic", model<Model1_7>(0.5, 0.4, 0.2, 0.1), signal<HarmonicInput>(),
                {0.33658839392315865, 0.7003073646534314, 0.6726082693965887,
                 0.15273688086966464, -0.444449767686022}},
        SimCase{"M1_7_fir_step", model<Model1_7>(0.0, 1.0, 1.0, 1.0), signal<StepInput>(1.0),
                {1.0, 2.0, 3.0, 3.0, 3.0}},
        SimCase{"M2_9_step", model<Model2_9>(0.6, 0.5), signal<StepInput>(1.0),
                {0.5, 0.8, 0.98}},
        SimCase{"M2_9_impulse", model<Model2_9>(0.6, 0.5), signal<ImpulseInput>(),
                {0.5, 0.3, 0.18}},
        SimCase{"M2_9_step_plus4", model<Model2_9>(0.0, 0.5), signal<StepInput>(4.0),
                {1.0, 1.0, 1.0}},
        SimCase{"M2_9_step_minus4", model<Model2_9>(0.0, 0.5), signal<StepInput>(-4.0),
                {-1.0, -1.0, -1.0}},
        SimCase{"M2_9_harmonic", model<Model2_9>(0.6, 0.5), signal<HarmonicInput>(),
                {0.45865863798905404, 0.7519806237481876, 0.6390180897227076,
                 -0.05156112667429513}},
        SimCase{"M3_1_decay", model<Model3_1>(0.8, 0.05), signal<StepInput>(1.0),
                {0.96, 0.9216, 0.884736}},
        SimCase{"M3_1_a_zero", model<Model3_1>(0.0, 0.1), signal<ImpulseInput>(),
                {1.0, 1.0, 1.0}}),
    [](const ::testing::TestParamInfo<SimCase>& info) { return info.param.label; });

// ---------- 2. Сходимость модели 1.7 к установившемуся значению ----------
// ступень A: y_inf = A * (b1 + b2 + b3) / (1 - a)
struct ConvCase {
    double a, b1, b2, b3, amplitude;
};

class ConvergenceTest : public ::testing::TestWithParam<ConvCase> {};

TEST_P(ConvergenceTest, StepResponseReachesSteadyState) {
    const ConvCase& c = GetParam();
    ASSERT_TRUE(stability::isStable(stability::classifyFirstOrder(c.a)));
    Model1_7 m(c.a, c.b1, c.b2, c.b3);
    StepInput u(c.amplitude);

    const auto steps = simulate(m, u, 400);

    const double yInf = c.amplitude * (c.b1 + c.b2 + c.b3) / (1.0 - c.a);
    EXPECT_NEAR(steps.back().y, yInf, 1e-6);
}

INSTANTIATE_TEST_SUITE_P(
    StableA, ConvergenceTest,
    ::testing::Values(
        ConvCase{0.5, 0.4, 0.2, 0.1, 1.0},     // y_inf = 1.4
        ConvCase{0.0, 1.0, 0.0, 0.0, 1.0},     // y_inf = 1
        ConvCase{0.2, 1.0, 0.0, 0.0, 2.0},     // y_inf = 2.5
        ConvCase{-0.5, 0.3, 0.3, 0.3, 1.0},    // y_inf = 0.6, знакопеременная сходимость
        ConvCase{0.9, 0.1, 0.1, 0.1, 1.0},     // y_inf = 3, медленная сходимость
        ConvCase{0.5, 0.4, 0.2, 0.1, -3.0}));  // y_inf = -4.2

// ---------- 3. Предсказание анализатора совпадает с поведением симуляции ----------
struct BehaviourCase {
    double a;
    stability::Class expected;
};

class StabilityVsSimulationTest : public ::testing::TestWithParam<BehaviourCase> {};

TEST_P(StabilityVsSimulationTest, ImpulseResponseBehaviourMatchesPrediction) {
    const BehaviourCase& c = GetParam();
    ASSERT_EQ(stability::classifyFirstOrder(c.a), c.expected);

    Model1_7 m(c.a, 0.4, 0.2, 0.1);
    ImpulseInput impulse;
    const double last = std::abs(simulate(m, impulse, 200).back().y);

    switch (c.expected) {
        case stability::Class::Stable:   EXPECT_LT(last, 1e-3); break;   // затухает
        case stability::Class::Boundary: EXPECT_GT(last, 0.1);           // не затухает
                                         EXPECT_LT(last, 1.0); break;    // и не растёт
        case stability::Class::Unstable: EXPECT_GT(last, 1e3); break;    // расходится
        default: FAIL();
    }
}

INSTANTIATE_TEST_SUITE_P(
    Regimes, StabilityVsSimulationTest,
    ::testing::Values(
        BehaviourCase{0.9, stability::Class::Stable},
        BehaviourCase{-0.9, stability::Class::Stable},
        BehaviourCase{1.0, stability::Class::Boundary},
        BehaviourCase{-1.0, stability::Class::Boundary},
        BehaviourCase{1.1, stability::Class::Unstable},
        BehaviourCase{-1.1, stability::Class::Unstable}));

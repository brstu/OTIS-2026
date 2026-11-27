#include "Model1_10.h"
#include "Model2_5.h"
#include "Model3_10.h"
#include "simulation.hpp"
#include "stability.hpp"
#include <gtest/gtest.h>
#include <cmath>
#include <limits>
#include <sstream>
#include <vector>

namespace {
template<class Case>
std::string caseName(const testing::TestParamInfo<Case>& info) {
    return info.param.name;
}

struct PoleCase {
    const char* name; double root; Stability expected; };
class PoleTest : public testing::TestWithParam<PoleCase> {};
TEST_P(PoleTest, ClassifiesRootAndBuildsWarning) {
    const auto p = GetParam();
    EXPECT_EQ(classifyPole(p.root), p.expected);
    const auto message = poleWarning(p.root);
    if (p.expected == Stability::Stable) EXPECT_TRUE(message.empty());
    else if (p.expected == Stability::Boundary) EXPECT_NE(message.find("|z| = 1"), std::string::npos);
    else if (p.expected == Stability::Unstable) EXPECT_NE(message.find("|z| > 1"), std::string::npos);
    else EXPECT_NE(message.find("not finite"), std::string::npos);
}
INSTANTIATE_TEST_SUITE_P(UnitCircle, PoleTest, testing::Values(
    PoleCase{"Zero", 0, Stability::Stable}, PoleCase{"Negative", -0.5, Stability::Stable},
    PoleCase{"InsidePlusOne", std::nextafter(1.0, 0.0), Stability::Stable},
    PoleCase{"InsideMinusOne", std::nextafter(-1.0, 0.0), Stability::Stable},
    PoleCase{"PlusOne", 1, Stability::Boundary}, PoleCase{"MinusOne", -1, Stability::Boundary},
    PoleCase{"OutsidePlusOne", std::nextafter(1.0, 2.0), Stability::Unstable},
    PoleCase{"OutsideMinusOne", std::nextafter(-1.0, -2.0), Stability::Unstable},
    PoleCase{"Infinity", std::numeric_limits<double>::infinity(), Stability::NonFinite},
    PoleCase{"NaN", std::numeric_limits<double>::quiet_NaN(), Stability::NonFinite}), caseName<PoleCase>);

struct LinearCase {
    const char* name;
    double a, b1, b2, initial;
    std::vector<double> input, expected;
};
class LinearTest : public testing::TestWithParam<LinearCase> {};
TEST_P(LinearTest, CalculatesSuccessiveSteps) {
    const auto& p = GetParam();
    Model1_10 model(p.a, p.b1, p.b2, p.initial);
    ASSERT_EQ(p.input.size(), p.expected.size());
    for (std::size_t i = 0; i < p.input.size(); ++i)
        EXPECT_NEAR(model.next(p.input[i]), p.expected[i], 1e-12) << "step " << i + 1;
}
INSTANTIATE_TEST_SUITE_P(Variant25, LinearTest, testing::Values(
    LinearCase{"Step", 0.5, 0.4, 0.1, 0, {1,1,1,1}, {0.4,0.6,0.8,0.9}},
    LinearCase{"Impulse", 0.5, 0.4, 0.1, 0, {1,0,0,0}, {0.4,0.2,0.2,0.1}},
    LinearCase{"PureDelay", 0, 0, 2, 0, {3,-4,1,0}, {0,0,6,-8}},
    LinearCase{"AlternatingDecay", -0.5, 0, 0, 8, {0,0,0}, {-4,2,-1}},
    LinearCase{"Divergent", 1.5, 0, 0, 2, {0,0,0}, {3,4.5,6.75}},
    LinearCase{"ZeroState", -1, 1, 2, 0, {0,0,0}, {0,0,0}}), caseName<LinearCase>);

struct DecayCase {
    const char* name;
    double a;
    int steps;
    double expectedDifference;
};
class DecayTest : public testing::TestWithParam<DecayCase> {};
TEST_P(DecayTest, FollowsPredictedDeviationUnderIdenticalControl) {
    const auto p = GetParam();
    Model1_10 first(p.a, 0.4, 0.1, 0);
    Model1_10 second(p.a, 0.4, 0.1, 1);
    StepInput control(1);
    for (int tau = 0; tau < p.steps; ++tau) {
        first.next(control.value(tau));
        second.next(control.value(tau));
    }
    EXPECT_NEAR(second.value() - first.value(), p.expectedDifference, 1e-12);
}
INSTANTIATE_TEST_SUITE_P(InitialDeviation, DecayTest, testing::Values(
    DecayCase{"Decay", 0.5, 10, 1.0 / 1024},
    DecayCase{"AlternatingDecay", -0.5, 9, -1.0 / 512},
    DecayCase{"Constant", 1, 20, 1},
    DecayCase{"Alternating", -1, 19, -1},
    DecayCase{"Growth", 2, 8, 256}), caseName<DecayCase>);

struct NonlinearCase {
    const char* name; double u, expected, tolerance; };
class NonlinearTest : public testing::TestWithParam<NonlinearCase> {};
TEST_P(NonlinearTest, VerifiesSignedInputIncrement) {
    Model2_5 model(0.5, 1.0, 0.0);
    const auto p = GetParam();
    EXPECT_NEAR(model.next(p.u), p.expected, p.tolerance);
    EXPECT_NEAR(model.next(0), p.expected / 2.0, p.tolerance);
}
INSTANTIATE_TEST_SUITE_P(Variant25, NonlinearTest, testing::Values(
    NonlinearCase{"Zero", 0,0,0}, NonlinearCase{"Positive", std::log(2.0),0.5,1e-15},
    NonlinearCase{"Negative", -std::log(2.0),-0.5,1e-15}, NonlinearCase{"SaturatesPositive", 50,1,1e-15},
    NonlinearCase{"SaturatesNegative", -50,-1,1e-15}, NonlinearCase{"TinyControl", 1e-14,1e-14,1e-27}), caseName<NonlinearCase>);

struct EulerCase {
    const char* name; double a,b,dt,initial,u,expected; };
class EulerTest : public testing::TestWithParam<EulerCase> {};
TEST_P(EulerTest, CalculatesOneEulerStep) {
    const auto p = GetParam();
    Model3_10 model(p.a, p.b, p.dt, p.initial);
    EXPECT_NEAR(model.next(p.u), p.expected, 1e-12);
}
INSTANTIATE_TEST_SUITE_P(Variant25, EulerTest, testing::Values(
    EulerCase{"Driven", 1,0.5,0.1,0,1,0.15}, EulerCase{"Offset", 1,0.5,0.1,0,0,0.05},
    EulerCase{"Equilibrium", 1,0.5,0.1,1.5,1,1.5}, EulerCase{"ConstantDerivative", 0,2,0.25,3,-1,3.25},
    EulerCase{"Deadbeat", 2,0,0.5,3,0,0}, EulerCase{"Boundary", 2,0,1,3,0,-3},
    EulerCase{"Rest", 1,0,0.25,0,0,0}), caseName<EulerCase>);

TEST(EulerMethod, HalvingTheStepReducesErrorAtSameTime) {
    // y(0)=0, y'=-y+1.5. Exact solution at t=1.
    const double exact = 1.5 * (1.0 - std::exp(-1.0));
    double previousError = 0.0;
    for (int n : {10, 20, 40}) {
        Model3_10 model(1, 0.5, 1.0 / n);
        for (int i = 0; i < n; ++i) model.next(1);
        const double error = std::abs(model.value() - exact);
        EXPECT_GT(error, 0);
        if (previousError > 0) {
            EXPECT_GT(previousError / error, 1.9);
            EXPECT_LT(previousError / error, 2.2);
        }
        previousError = error;
    }
}

TEST(ModelState, ResetRestoresInitialValueAndClearsDelay) {
    Model1_10 linear(0.5, 1, 3, 4);
    Model2_5 nonlinear(0.5, 2, 4);
    Model3_10 euler(1, 0, 0.5, 4);
    for (Model* m : std::vector<Model*>{&linear, &nonlinear, &euler}) {
        const double first = m->next(3);
        m->next(7);
        m->reset();
        EXPECT_DOUBLE_EQ(m->value(), 4);
        EXPECT_DOUBLE_EQ(m->next(3), first);
    }
    EXPECT_DOUBLE_EQ(linear.next(0), 2.5); // u(-1) ещё равно нулю.
}
TEST(ModelState, UsesCoefficientOfStateDifferenceForEachModel) {
    EXPECT_TRUE(Model1_10(-0.5, 100, -50).isStable());
    EXPECT_FALSE(Model1_10(-1.0, 0, 0).isStable());
    EXPECT_DOUBLE_EQ(Model2_5(-1.2, 100).pole(), -1.2);
    EXPECT_FALSE(Model2_5(-1.2, 100).isStable());
    EXPECT_TRUE(Model3_10(2, 10, 0.25).isStable());
    EXPECT_DOUBLE_EQ(Model3_10(2, 10, 1).pole(), -1);
    EXPECT_FALSE(Model3_10(2, 10, 1).isStable());
}

struct SignalCase {
    const char* name; int tau; double step, impulse, harmonic; };
class SignalTest : public testing::TestWithParam<SignalCase> {};
TEST_P(SignalTest, ReturnsCorrectControlValues) {
    const auto p = GetParam();
    EXPECT_DOUBLE_EQ(StepInput(-2).value(p.tau), p.step);
    EXPECT_DOUBLE_EQ(ImpulseInput().value(p.tau), p.impulse);
    EXPECT_NEAR(HarmonicInput().value(p.tau), p.harmonic, 1e-14);
}
INSTANTIATE_TEST_SUITE_P(TimeIndex, SignalTest, testing::Values(
    SignalCase{"BeforeStart", -1,0,0,0}, SignalCase{"AtStart", 0,-2,1,0},
    SignalCase{"FirstStep", 1,-2,0,0.8414709848078965}, SignalCase{"SecondStep", 2,-2,0,0.9092974268256817}), caseName<SignalCase>);

// Подмена управления таблицей. Симулятор получает тот же интерфейс InputSignal.
class TableSignal : public InputSignal {
    std::vector<double> inputs;
public:
    explicit TableSignal(const std::vector<double>& samples) : inputs(samples) {}
    double value(int tau) const override { return inputs.at(tau); }
    std::string name() const override { return "Table"; }
};
struct SimulationCase {
    const char* name; std::vector<double> inputs, outputs; };
class SimulationTest : public testing::TestWithParam<SimulationCase> {};
TEST_P(SimulationTest, ChecksEveryCsvRowWithSubstitutedSignal) {
    const auto& p = GetParam();
    TableSignal signal(p.inputs);
    Model1_10 model(0.5, 1, 2, 0);
    model.next(9); // simulate должен восстановить y0 и историю до первого шага.
    std::ostringstream csv, console;
    simulate(model, signal, static_cast<int>(p.inputs.size()) - 1, csv, console);
    std::istringstream rows(csv.str());
    std::string header;
    std::getline(rows, header);
    EXPECT_EQ(header, "tau,u_tau,y_tau");
    for (std::size_t i = 0; i < p.outputs.size(); ++i) {
        int tau;
        char first, second;
        double u, y;
        ASSERT_TRUE(rows >> tau >> first >> u >> second >> y);
        EXPECT_EQ(tau, static_cast<int>(i));
        EXPECT_EQ(first, ','); EXPECT_EQ(second, ',');
        EXPECT_NEAR(u, p.inputs[i], 1e-14);
        EXPECT_NEAR(y, p.outputs[i], 1e-14);
    }
    EXPECT_TRUE((rows >> std::ws).eof());
}
INSTANTIATE_TEST_SUITE_P(SubstitutedInputs, SimulationTest, testing::Values(
    SimulationCase{"Step", {1,1,1,1},{0,1,1.5,3.75}},
    SimulationCase{"Impulse", {1,0,0,0},{0,1,0.5,2.25}},
    SimulationCase{"Harmonic", {0,0.8414709848078965,0.9092974268256817,0.1411200080598672},
                   {0,0,0.8414709848078965,1.33003291922963}}), caseName<SimulationCase>);
}  // namespace

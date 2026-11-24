#include <gtest/gtest.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include "InputSignal.h"
#include "Model1_6.h"
#include "Model2_1.h"
#include "Model3_7.h"

int validateStepLimit(int raw_steps);
int parseStepsFromArgsOrStream(int argc, char* const argv[], std::istream& in, std::ostream& out);
std::string resolveOutputCsvPath(int argc, char* const argv[]);
double fetchFloatingParameter(const std::string& msg, double default_val);
int querySystemSelection();
SignalType querySignalSelection();

TEST(ModelsManualCalculation, Model1_6KnownValues) {
    Model1_6 model(0.5, 0.2, 0.1, 1.0, 0.0);

    EXPECT_NEAR(model.step(1.0), 1.0, 1e-9);
    EXPECT_NEAR(model.step(1.0), 1.5, 1e-9);
    EXPECT_NEAR(model.step(1.0), 1.95, 1e-9);
}

TEST(ModelsManualCalculation, Model2_1KnownValues) {
    Model2_1 model(0.5, 0.25, 1.0, 0.0, 0.0);

    EXPECT_NEAR(model.step(2.0), 2.0, 1e-9);
    EXPECT_NEAR(model.step(1.0), 2.0, 1e-9);
    EXPECT_NEAR(model.step(0.0), 0.0, 1e-9);
}

TEST(ModelsManualCalculation, ResetAndNonlinearSineAndSignalNames) {
    Model1_6 m1(0.4, 0.2, 0.1, 0.5, 1.0);
    EXPECT_DOUBLE_EQ(m1.getState(), 1.0);
    m1.reset(0.0);
    EXPECT_DOUBLE_EQ(m1.getY(), 0.0);
    EXPECT_DOUBLE_EQ(m1.step(0.0), 0.0);

    Model2_1 m2(0.0, 0.0, 0.0, 0.5, 0.0);
    EXPECT_NEAR(m2.step(1.0), 0.0, 1e-12);
    EXPECT_NEAR(m2.step(0.0), 0.5 * std::sin(1.0), 1e-12);
    m2.reset(0.0);
    EXPECT_DOUBLE_EQ(m2.getY(), 0.0);

    EXPECT_EQ(InputSignal::getSignalName(SignalType::STEP), "Step");
    EXPECT_EQ(InputSignal::getSignalName(SignalType::IMPULSE), "Impulse");
    EXPECT_EQ(InputSignal::getSignalName(SignalType::HARMONIC), "Harmonic");
    EXPECT_EQ(InputSignal::getSignalName(static_cast<SignalType>(99)), "Undefined");

    EXPECT_DOUBLE_EQ(InputSignal::getSignal(SignalType::STEP, 0), 1.0);
    EXPECT_DOUBLE_EQ(InputSignal::getSignal(SignalType::STEP, 3), 1.0);
    EXPECT_DOUBLE_EQ(InputSignal::getSignal(SignalType::IMPULSE, 0), 1.0);
    EXPECT_DOUBLE_EQ(InputSignal::getSignal(SignalType::IMPULSE, 2), 0.0);
    EXPECT_NEAR(InputSignal::getSignal(SignalType::HARMONIC, 1), std::sin(1.0), 1e-12);
    EXPECT_DOUBLE_EQ(InputSignal::getSignal(static_cast<SignalType>(99), 0), 0.0);
}

TEST(ModelsInvariantTest, ZeroInputZeroStateRemainsZero) {
    Model1_6 m1(0.4, 0.2, 0.1, 0.5, 0.0);
    Model2_1 m2(0.5, 0.1, 0.4, 0.2, 0.0);
    Model3_7 m3(0.2, 0.5, 0.1, 0.0);

    for (int i = 0; i < 20; ++i) {
        EXPECT_DOUBLE_EQ(m1.step(0.0), 0.0);
        EXPECT_DOUBLE_EQ(m2.step(0.0), 0.0);
        EXPECT_DOUBLE_EQ(m3.step(0.0), 0.0);
    }
}

TEST(ModelsInvariantTest, Model3_7EulerVsAnalyticalSolution) {
    const double a = 0.0;
    const double k = std::exp(a);
    const double dt = 0.001;
    const double y0 = 2.0;

    Model3_7 model(a, 0.0, dt, y0);
    const int steps = 100;
    double y_sim = y0;
    for (int i = 0; i < steps; ++i) {
        y_sim = model.step(0.0);
    }

    const double y_exact = y0 * std::exp(-k * steps * dt);
    EXPECT_NEAR(y_sim, y_exact, 1e-3);
}

struct ConvergenceParams {
    double a1;
    double a2;
    double a3;
    double b;
    int steps;
    double expected_final_bound;
};

class Model1_6ParameterizedTest : public ::testing::TestWithParam<ConvergenceParams> {};

TEST_P(Model1_6ParameterizedTest, ConvergesForStableCoefficients) {
    const auto& params = GetParam();
    Model1_6 model(params.a1, params.a2, params.a3, params.b, 0.0);

    double y = 0.0;
    for (int i = 0; i < params.steps; ++i) {
        y = model.step(1.0);
        ASSERT_FALSE(std::isnan(y));
        ASSERT_FALSE(std::isinf(y));
    }

    EXPECT_LE(std::abs(y), params.expected_final_bound);
}

INSTANTIATE_TEST_SUITE_P(
    StableStepConvergence,
    Model1_6ParameterizedTest,
    ::testing::Values(
        ConvergenceParams{0.3, 0.2, 0.1, 0.4, 50, 2.0},
        ConvergenceParams{0.1, 0.1, 0.1, 0.7, 50, 2.0},
        ConvergenceParams{-0.2, 0.15, 0.05, 1.0, 50, 2.0}
    )
);

TEST(MainHelpersTest, ValidationAndInteractivePrompts) {
    EXPECT_EQ(validateStepLimit(10), 10);
    EXPECT_EQ(validateStepLimit(-5), 30);
    EXPECT_EQ(validateStepLimit(5000), 30);

    char arg0[] = "app";
    char arg1_valid[] = "25";
    char arg1_invalid[] = "bad";
    char arg2_valid[] = "custom.csv";
    char arg2_empty[] = "";

    char* argv_valid_step[] = {arg0, arg1_valid, arg2_valid, nullptr};
    char* argv_invalid_step[] = {arg0, arg1_invalid, arg2_empty, nullptr};
    char* argv_no_args[] = {arg0, nullptr};

    std::ostringstream dummy_out;
    std::istringstream empty_in("");
    EXPECT_EQ(parseStepsFromArgsOrStream(2, argv_valid_step, empty_in, dummy_out), 25);
    EXPECT_EQ(parseStepsFromArgsOrStream(2, argv_invalid_step, empty_in, dummy_out), 30);
    EXPECT_EQ(parseStepsFromArgsOrStream(1, argv_no_args, empty_in, dummy_out), 30);

    std::istringstream blank_line_in("\n");
    EXPECT_EQ(parseStepsFromArgsOrStream(1, argv_no_args, blank_line_in, dummy_out), 30);

    std::istringstream non_num_in("abc\n");
    EXPECT_EQ(parseStepsFromArgsOrStream(1, argv_no_args, non_num_in, dummy_out), 30);

    std::istringstream num_in("42\n");
    EXPECT_EQ(parseStepsFromArgsOrStream(1, argv_no_args, num_in, dummy_out), 42);

    EXPECT_EQ(resolveOutputCsvPath(1, argv_no_args), "results.csv");
    EXPECT_EQ(resolveOutputCsvPath(3, argv_invalid_step), "results.csv");
    EXPECT_EQ(resolveOutputCsvPath(3, argv_valid_step), "custom.csv");

    std::streambuf* orig_cin = std::cin.rdbuf();
    std::streambuf* orig_cout = std::cout.rdbuf();
    std::cout.rdbuf(dummy_out.rdbuf());

    std::istringstream in_defaults("\n\n\nabc\nxyz\nqwe\n");
    std::cin.rdbuf(in_defaults.rdbuf());
    EXPECT_NEAR(fetchFloatingParameter("p", 0.5), 0.5, 1e-9);
    EXPECT_EQ(querySystemSelection(), 1);
    EXPECT_EQ(querySignalSelection(), SignalType::STEP);
    EXPECT_NEAR(fetchFloatingParameter("p", 0.5), 0.5, 1e-9);
    EXPECT_EQ(querySystemSelection(), 1);
    EXPECT_EQ(querySignalSelection(), SignalType::STEP);

    std::istringstream in_custom("1.25\n2\n3\n0\n99\n1\n2\n3\n");
    std::cin.rdbuf(in_custom.rdbuf());
    EXPECT_NEAR(fetchFloatingParameter("p", 0.5), 1.25, 1e-9);
    EXPECT_EQ(querySystemSelection(), 2);
    EXPECT_EQ(querySignalSelection(), SignalType::HARMONIC);
    EXPECT_EQ(querySystemSelection(), 1);
    EXPECT_EQ(querySystemSelection(), 1);
    EXPECT_EQ(querySignalSelection(), SignalType::STEP);
    EXPECT_EQ(querySignalSelection(), SignalType::IMPULSE);
    EXPECT_EQ(querySignalSelection(), SignalType::HARMONIC);

    std::cin.rdbuf(orig_cin);
    std::cout.rdbuf(orig_cout);
}
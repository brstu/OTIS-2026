#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cmath>
#include <memory>
#include "../src/model.h"
#include "../src/input.h"

using ::testing::Return;

const double PI = std::acos(-1.0);

class MockInputGenerator : public InputGenerator {
public:
    MOCK_METHOD(double, generate, (int tau), (const, override));
};

struct Model1_1StepParams {
    double a, b, y_tau, u_tau, expected_y;
};

class Model1_1StepTest : public ::testing::TestWithParam<Model1_1StepParams> {};

TEST_P(Model1_1StepTest, CalculatesStepCorrectly) {
    auto p = GetParam();
    model1_1 m(p.a, p.b);
    EXPECT_DOUBLE_EQ(m.step(p.y_tau, p.u_tau), p.expected_y);
}

INSTANTIATE_TEST_SUITE_P(
    StepCalculations, Model1_1StepTest,
    ::testing::Values(
        Model1_1StepParams{ 0.5, 2.0, 1.0, 1.0, 2.5 },
        Model1_1StepParams{ -0.5, 1.0, 2.0, -1.0, -2.0 },
        Model1_1StepParams{ 0.0, 5.0, 10.0, 2.0, 10.0 }
    )
);

struct Model1_1StabilityParams {
    double a, b;
    bool expected_stable;
};

class Model1_1StabilityTest : public ::testing::TestWithParam<Model1_1StabilityParams> {};

TEST_P(Model1_1StabilityTest, ChecksStability) {
    auto p = GetParam();
    model1_1 m(p.a, p.b);
    EXPECT_EQ(m.isStable(), p.expected_stable);
}

INSTANTIATE_TEST_SUITE_P(
    StabilityChecks, Model1_1StabilityTest,
    ::testing::Values(
        Model1_1StabilityParams{ 0.5, 1.0, true }, 
        Model1_1StabilityParams{ -0.99, 1.0, true },
        Model1_1StabilityParams{ 1.0, 1.0, false }, 
        Model1_1StabilityParams{ -1.0, 1.0, false },
        Model1_1StabilityParams{ 1.5, 1.0, false }
    )
);

struct Model2_3StepParams {
    double a, b, delta, y_tau, u_tau, expected_y;
};

class Model2_3StepTest : public ::testing::TestWithParam<Model2_3StepParams> {};

TEST_P(Model2_3StepTest, CalculatesStepCorrectly) {
    auto p = GetParam();
    model2_3 m(p.a, p.b, p.delta);
    EXPECT_DOUBLE_EQ(m.step(p.y_tau, p.u_tau), p.expected_y);
}

INSTANTIATE_TEST_SUITE_P(
    StepCalculations, Model2_3StepTest,
    ::testing::Values(
        Model2_3StepParams{ 0.5, 2.0, 1.0, 1.0, 2.0, 0.5 * 1.0 + 2.0 * (2.0 - 1.0) },
        Model2_3StepParams{ 0.5, 2.0, 1.0, 1.0, -2.0, 0.5 * 1.0 + 2.0 * (-2.0 + 1.0) },
        Model2_3StepParams{ 0.5, 2.0, 1.0, 1.0, 0.5, 0.5 * 1.0 + 2.0 * 0.0 },
        Model2_3StepParams{ 0.5, 2.0, 1.0, 1.0, -0.5, 0.5 * 1.0 + 2.0 * 0.0 }
    )
);

struct Model3_5StepParams {
    double b, dt, y_tau, u_tau, expected_y;
};

class Model3_5StepTest : public ::testing::TestWithParam<Model3_5StepParams> {};

TEST_P(Model3_5StepTest, CalculatesStepCorrectly) {
    auto p = GetParam();
    model3_5 m(p.b, p.dt);
    EXPECT_DOUBLE_EQ(m.step(p.y_tau, p.u_tau), p.expected_y);
}

INSTANTIATE_TEST_SUITE_P(
    StepCalculations, Model3_5StepTest,
    ::testing::Values(
        Model3_5StepParams{ 1.0, 0.1, 0.0, 0.0, 0.0 + 0.1 * 1.0 * std::sin(0.0) },
        Model3_5StepParams{ 2.0, 0.5, 1.0, PI / 2.0, 1.0 + 0.5 * 2.0 * std::sin(PI / 2.0) },
        Model3_5StepParams{ 1.0, 1.0, 5.0, PI, 5.0 + 1.0 * 1.0 * std::sin(PI) }
    )
);

TEST(InputGeneratorTest, StepInput) {
    StepInput gen;
    EXPECT_DOUBLE_EQ(gen.generate(0), 1.0);
    EXPECT_DOUBLE_EQ(gen.generate(10), 1.0);
}

TEST(InputGeneratorTest, ImpulseInput) {
    ImpulseInput gen;
    EXPECT_DOUBLE_EQ(gen.generate(0), 1.0);
    EXPECT_DOUBLE_EQ(gen.generate(1), 0.0);
}

TEST(InputGeneratorTest, HarmonicInput) {
    HarmonicInput gen;
    EXPECT_NEAR(gen.generate(1), std::sin(1.0), 1e-9);
}

TEST(SimulationLoopTest, UsesMockedInputsCorrectly) {
    MockInputGenerator mock_gen;

    EXPECT_CALL(mock_gen, generate(0)).WillOnce(Return(1.0));
    EXPECT_CALL(mock_gen, generate(1)).WillOnce(Return(0.5));
    EXPECT_CALL(mock_gen, generate(2)).WillOnce(Return(-0.5));

    model1_1 m(0.5, 2.0);
    double y = 0.0;

    for (int tau = 0; tau < 3; ++tau) {
        double u = mock_gen.generate(tau);
        y = m.step(y, u);
    }

    EXPECT_DOUBLE_EQ(y, 0.0);
}
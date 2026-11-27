#include <cmath>
#include <gtest/gtest.h>
#include "../src/Model_1_8.h"

TEST(Model1_8_Basic, Initialization) {
    Model_1_8 model(0.5, 0.2, 1.0, 0.0);
    SUCCEED();
}

TEST(Model1_8_Basic, StepResponseZero) {
    Model_1_8 model(0.5, 0.2, 1.0, 0.0);
    double val = model.step(0.0);
    EXPECT_GE(val, -1e-5);
}

TEST(Model1_8_Basic, StepResponsePositive) {
    Model_1_8 model(0.5, 0.2, 1.0, 0.0);
    double val = model.step(1.0);
    EXPECT_NE(val, 0.0);
}

struct TrajectoryParams {
    int id;
    double input;
};

class ModelParameterizedTest : public ::testing::TestWithParam<TrajectoryParams> {};

TEST_P(ModelParameterizedTest, CalculationCheck) {
    auto p = GetParam();
    Model_1_8 model(0.5, 0.2, 1.0, 0.0);
    double output = model.step(p.input);
    EXPECT_TRUE(std::isfinite(output));
}

INSTANTIATE_TEST_SUITE_P(
    Model1_8_Suite,
    ModelParameterizedTest,
    ::testing::Values(
        TrajectoryParams{1, 0.1}, TrajectoryParams{2, 0.2}, TrajectoryParams{3, 0.3},
        TrajectoryParams{4, 0.4}, TrajectoryParams{5, 0.5}, TrajectoryParams{6, 0.6},
        TrajectoryParams{7, 0.7}, TrajectoryParams{8, 0.8}, TrajectoryParams{9, 0.9},
        TrajectoryParams{10, 1.0}, TrajectoryParams{11, 1.1}, TrajectoryParams{12, 1.2},
        TrajectoryParams{13, 1.3}, TrajectoryParams{14, 1.4}, TrajectoryParams{15, 1.5},
        TrajectoryParams{16, 1.6}, TrajectoryParams{17, 1.7}, TrajectoryParams{18, 1.8},
        TrajectoryParams{19, 1.9}, TrajectoryParams{20, 2.0}, TrajectoryParams{21, 2.5},
        TrajectoryParams{22, 3.0}, TrajectoryParams{23, 3.5}, TrajectoryParams{24, 4.0},
        TrajectoryParams{25, 5.0}
    )
);
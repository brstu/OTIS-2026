#include <gtest/gtest.h>
#include <cmath>
#include <memory>
#include "../../task_01/src/Model 1.10.h"

struct SimParam {
    double a;
    double b;
    double input;
    double expected_steady_state;
};

class ModelConvergenceTest : public ::testing::TestWithParam<SimParam> {};

TEST_P(ModelConvergenceTest, ConvergesToSteadyState) {
    SimParam p = GetParam();
    LineModel_1_10 model(p.a, p.b, 0.0);
    double y = 0.0;
    for (int i = 0; i < 100; ++i) {
        y = model.step(p.input);
    }
    EXPECT_NEAR(y, p.expected_steady_state, 0.01);
}

// 22 и 23. Параметризованные тесты сходимости (считаются как 2 теста)
INSTANTIATE_TEST_SUITE_P(
    Default,
    ModelConvergenceTest,
    ::testing::Values(
        SimParam{0.5, 1.0, 1.0, 2.0},
        SimParam{0.2, 1.0, 2.0, 2.5}
    )
);

// 24. Подмена сигналов (Импульс и Гармоника)
TEST(SignalModes, ImpulseAndHarmonic) {
    LineModel_1_10 model_imp(0.5, 1.0, 0.0);
    EXPECT_DOUBLE_EQ(model_imp.step(1.0), 1.0); 
    EXPECT_DOUBLE_EQ(model_imp.step(0.0), 0.5); 

    LineModel_1_10 model_harm(0.5, 1.0, 0.0);
    double u1 = std::sin(1.0); 
    EXPECT_NEAR(model_harm.step(u1), u1, 1e-9); 
}

// 25. Нулевой вход (Подмена сигнала)
TEST(SignalModes, ZeroInput) {
    LineModel_1_10 model(0.8, 1.0, 0.5);
    EXPECT_DOUBLE_EQ(model.step(0.0), 0.0);
}
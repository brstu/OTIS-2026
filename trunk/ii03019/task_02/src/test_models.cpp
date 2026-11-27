#include <gtest/gtest.h>
#include "Linear_Model.h"
#include "Non_Linear_Model.h"

TEST(LinearModelTest, ManualCalculation) {
    Linear_Model m(0.5, 1.0, 0.0, 0.0, 0, 2.0);
    EXPECT_DOUBLE_EQ(m.NextStep(), 2.0);
    EXPECT_DOUBLE_EQ(m.NextStep(), 3.0);
    EXPECT_DOUBLE_EQ(m.NextStep(), 3.5);
}

class LinearModelParamTest : public ::testing::TestWithParam<int> {};

TEST_P(LinearModelParamTest, ConvergenceCheck) {
    int steps = GetParam();
    Linear_Model m(0.5, 1.0, 0.0, 0.0, 0, 1.0);
    double y = 0;
    for (int i = 0; i < steps; ++i) {
        y = m.NextStep();
    }
    EXPECT_NEAR(y, 2.0, 0.1); 
}

INSTANTIATE_TEST_SUITE_P(StepsVariations, LinearModelParamTest, ::testing::Values(10, 50, 100));
#include <gtest/gtest.h>
#include "Linear_Model.h"
#include "Non_Linear_Model.h"
#include "Simple_Differential_Equation.h"

TEST(GettersTest, LinearModelGetters) {
    Linear_Model m(0.5, 1.0, 0.0, 0.0, 0, 2.0);
    m.NextStep();
    EXPECT_DOUBLE_EQ(m.GetY(), 2.0);
    EXPECT_DOUBLE_EQ(m.GetLastU(), 2.0);
}

TEST(GettersTest, NonLinearModelGetters) {
    Non_linear_Model m(0.5, 1.0, 1.0, 0.0, 0, 2.0);
    m.NextStep();
    EXPECT_NE(m.GetY(), 0.0); // Проверяем, что значение обновилось
    EXPECT_DOUBLE_EQ(m.GetLastU(), 2.0);
}

TEST(GettersTest, SdeModelGetters) {
    Simple_Differential_Equation m(1.0, 0.1, 0, 2.0);
    m.NextStep();
    EXPECT_NE(m.GetY(), 0.0);
    EXPECT_DOUBLE_EQ(m.GetLastU(), 2.0);
}
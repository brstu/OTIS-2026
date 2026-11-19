#include <gtest/gtest.h>
#include "Non_Linear_Model.h"
#include "Simple_Differential_Equation.h"
#include <cmath>

TEST(InvariantsTest, ZeroInputZeroState) {
    Non_linear_Model m(0.9, 1.0, 1.0, 0.0, 0, 0.0);
    for (int i = 0; i < 10; ++i) {
        EXPECT_DOUBLE_EQ(m.NextStep(), 0.0);
    }
}

TEST(BoundaryValueTest, EulerIntegrationStep) {
    double b = 1.0;
    double dt = 0.1;
    Simple_Differential_Equation m(b, dt, 0, M_PI / 2.0); 
    EXPECT_DOUBLE_EQ(m.NextStep(), 0.1);
    EXPECT_DOUBLE_EQ(m.NextStep(), 0.2);
}
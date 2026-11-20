/// @file test_regulator.cpp
/// @brief Шесть модульных тестов ПИД-регулятора.

#include <gtest/gtest.h>
#include <array>
#include <cmath>

#include "pid_regulator.h"
#include "../../task_01/src/model1.6.h"

TEST(PidLaw, FirstStepIsProportionalPlusIntegral){
    PidRegulator pid(2.0, 0.5, 0.2, 1.0, -10.0, 10.0);
    EXPECT_DOUBLE_EQ(pid.compute(1.0, 0.0), 2.5);
}

TEST(PidLaw, SecondStepUsesErrorHistory){
    PidRegulator pid(2.0, 0.5, 0.2, 1.0, -10.0, 10.0);
    pid.compute(1.0, 0.0);
    EXPECT_NEAR(pid.compute(1.0, 0.5), 1.65, 1e-12);
}

TEST(PidLimits, RespectsActuatorLimits){
    PidRegulator upper(100.0, 0.0, 0.0, 1.0, -5.0, 5.0);
    EXPECT_DOUBLE_EQ(upper.compute(50.0, 0.0), 5.0);
    PidRegulator lower(100.0, 0.0, 0.0, 1.0, -5.0, 5.0);
    EXPECT_DOUBLE_EQ(lower.compute(-50.0, 0.0), -5.0);
}

TEST(PidReset, RestoresInitialState){
    PidRegulator pid(2.0, 0.5, 0.2, 1.0, -10.0, 10.0);
    pid.compute(1.0, 0.0);
    pid.compute(1.0, 0.5);
    pid.reset();
    PidRegulator fresh(2.0, 0.5, 0.2, 1.0, -10.0, 10.0);
    EXPECT_DOUBLE_EQ(pid.compute(1.0, 0.0), fresh.compute(1.0, 0.0));
}

TEST(ClosedLoop, FollowsReferenceTrajectory){
    PidRegulator pid(0.5, 0.1, 0.1, 1.0, -100.0, 100.0);
    model1_6 plant(0.3, 0.2, 0.1, 0.5);
    const std::array<double, 5> expectedU{{6.0, 4.9, 5.655, 5.59725, 5.7961375}};
    const std::array<double, 5> expectedY{{3.0, 3.35, 4.4325, 5.098375, 5.64908125}};
    double y = 0.0;
    for (std::size_t t = 0; t < expectedU.size(); ++t){
        const double u = pid.compute(10.0, y);
        EXPECT_NEAR(u, expectedU[t], 1e-9) << "такт " << t;
        y = plant.stepForward(u);
        EXPECT_NEAR(y, expectedY[t], 1e-9) << "такт " << t;
    }
}

TEST(ClosedLoop, ReachesSetpointWithinEps){
    PidRegulator pid(0.5, 0.2, 0.1, 1.0, -100.0, 100.0);
    model1_6 plant(0.3, 0.2, 0.1, 0.5);
    const double w = 10.0;
    const double eps = 0.05;
    double y = 0.0;
    int steps = 0;
    for (int t = 0; t < 100; ++t){
        y = plant.stepForward(pid.compute(w, y));
        ++steps;
        if (std::fabs(w - y) < eps) break;
    }
    EXPECT_LT(steps, 100) << "установившийся режим не достигнут";
    EXPECT_LT(std::fabs(w - y), eps);
}
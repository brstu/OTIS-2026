#include "control.hpp"
#include <gtest/gtest.h>
#include <cmath>
#include <stdexcept>

TEST(Plants, LinearFirstStep) { LinearPlant19 p; EXPECT_NEAR(p.update(1.0), 0.4, 1e-12); }
TEST(Plants, NonlinearFirstStep) { NonlinearPlant21 p; EXPECT_NEAR(p.update(1.0), 0.4, 1e-12); }
TEST(Plants, DifferentialEulerStep) { DifferentialPlant33 p; EXPECT_NEAR(p.update(1.0), 0.1, 1e-12); }
TEST(Plants, UserCoefficientsAreUsed) {
    LinearPlant19 linear({0.0, 0.0, 0.0, 2.0});
    EXPECT_DOUBLE_EQ(linear.update(3.0), 6.0);
    NonlinearPlant21 nonlinear({0.0, 0.0, 2.0, 0.0});
    EXPECT_DOUBLE_EQ(nonlinear.update(3.0), 6.0);
    DifferentialPlant33 differential({0.0, 2.0, 0.5});
    EXPECT_DOUBLE_EQ(differential.update(3.0), 3.0);
}
TEST(Plants, DifferentialRejectsInvalidTimeStep) {
    EXPECT_THROW(DifferentialPlant33({0.6, 1.0, 0.0}), std::invalid_argument);
}
TEST(Plants, ResetRestoresZero) {
    LinearPlant19 p; p.update(1.0); p.reset(); EXPECT_DOUBLE_EQ(p.output(), 0.0);
}
TEST(PID, ProportionalPart) {
    PIDController pid(2.0, 0.0, 0.0, 0.1, -10.0, 10.0);
    EXPECT_DOUBLE_EQ(pid.update(3.0), 6.0);
}
TEST(PID, OutputIsLimited) {
    PIDController pid(10.0, 1.0, 0.0, 0.1, -2.0, 2.0);
    EXPECT_DOUBLE_EQ(pid.update(10.0), 2.0);
    EXPECT_DOUBLE_EQ(pid.update(-10.0), -2.0);
}
TEST(PID, ResetClearsHistory) {
    PIDController pid(1.0, 1.0, 1.0, 0.1, -100.0, 100.0);
    const double first = pid.update(1.0); pid.update(0.0); pid.reset();
    EXPECT_DOUBLE_EQ(pid.update(1.0), first);
}
TEST(PID, InvalidSettingsAreRejected) {
    EXPECT_THROW(PIDController(1, 0, 0, 0.0, -5, 5), std::invalid_argument);
    EXPECT_THROW(PIDController(1, 0, 0, 0.1, 5, 5), std::invalid_argument);
}
TEST(Simulation, FixedModeHasRequestedLength) {
    LinearPlant19 plant; PIDController pid(0.8,0.12,0.03,0.1,-5,5);
    EXPECT_EQ(runFixed(plant,pid,1.0,25).size(), 25u);
}
TEST(Simulation, SettlingModeStopsInsideMaximum) {
    DifferentialPlant33 plant; PIDController pid(0.8,0.12,0.03,0.1,-5,5);
    const auto result = runUntilSettled(plant,pid,1.0,0.05,1000,5);
    EXPECT_LE(result.size(), 1000u);
    EXPECT_LE(std::abs(result.back().error), 0.05);
}
TEST(Simulation, InvalidArgumentsAreRejected) {
    LinearPlant19 plant; PIDController pid(1,0,0,0.1,-5,5);
    EXPECT_THROW(runFixed(plant,pid,1.0,0), std::invalid_argument);
    EXPECT_THROW(runUntilSettled(plant,pid,1.0,0.0,10), std::invalid_argument);
}
TEST(Simulation, RowContainsRequiredTableValues) {
    LinearPlant19 plant;
    PIDController pid(1.0, 0.0, 0.0, 0.1, -10.0, 10.0);
    const auto result = runFixed(plant, pid, 1.0, 1);
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].step, 0);
    EXPECT_DOUBLE_EQ(result[0].setpoint, 1.0);
    EXPECT_DOUBLE_EQ(result[0].control, 1.0);
    EXPECT_DOUBLE_EQ(result[0].output, 0.4);
    EXPECT_DOUBLE_EQ(result[0].error, 0.6);
}

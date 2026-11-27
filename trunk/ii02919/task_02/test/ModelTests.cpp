#include <gtest/gtest.h>
#include "MultiStepModel.h"
#include "QuadraticFeedback.h"
#include "HarmonicODE.h"
#include "ProcessFactory.h"
#include "InputSignal.h"

#include <cmath>
#include <memory>

TEST(MultiStepModelTest, ComputesKnownSequenceAndUsesTwoStepInputHistory)
{
    // y[k+1] = 0.5*y[k] + 2*u[k] + 3*u[k-1] + 4*u[k-2]
    MultiStepModel model(0.5, 2.0, 3.0, 4.0);
    EXPECT_DOUBLE_EQ(model.advance(1.0), 2.0);
    EXPECT_DOUBLE_EQ(model.advance(2.0), 8.0);   // 1 + 4 + 3
    EXPECT_DOUBLE_EQ(model.advance(0.0), 14.0); // 4 + 0 + 6 + 4
}

TEST(MultiStepModelTest, ResetClearsOutputAndInputHistory)
{
    MultiStepModel model(0.5, 2.0, 3.0, 4.0);
    model.advance(1.0);
    model.advance(2.0);
    model.reset();
    EXPECT_DOUBLE_EQ(model.advance(0.0), 0.0);
}

TEST(QuadraticFeedbackTest, ComputesKnownValuesIncludingDelayedSine)
{
    // y[k+1] = a*y[k] - b*y[k-1]^2 + c*u[k] + d*sin(u[k-1])
    QuadraticFeedback model(0.5, 0.25, 2.0, 3.0);
    EXPECT_NEAR(model.advance(1.0), 2.0, 1e-12);
    EXPECT_NEAR(model.advance(0.0), 1.0 + 3.0 * std::sin(1.0), 1e-12);
}

TEST(QuadraticFeedbackTest, ZeroInputAndZeroInitialStateRemainAtRest)
{
    QuadraticFeedback model(0.8, 0.2, 5.0, 3.0);
    for (int i = 0; i < 20; ++i) EXPECT_DOUBLE_EQ(model.advance(0.0), 0.0);
}

TEST(QuadraticFeedbackTest, ResetClearsDelayedState)
{
    QuadraticFeedback model(0.5, 0.25, 2.0, 3.0);
    model.advance(1.0);
    model.advance(0.0);
    model.reset();
    EXPECT_DOUBLE_EQ(model.advance(0.0), 0.0);
}

TEST(HarmonicODETest, EulerStepMatchesHandCalculation)
{
    HarmonicODE model(2.0, 0.1);
    const double expected = 0.1 * 2.0 * std::sin(1.0);
    EXPECT_NEAR(model.advance(1.0), expected, 1e-12);
    EXPECT_NEAR(model.advance(1.0), 2.0 * expected, 1e-12);
}

TEST(HarmonicODETest, ZeroInputKeepsZeroState)
{
    HarmonicODE model(3.0, 0.01);
    for (int i = 0; i < 20; ++i) EXPECT_DOUBLE_EQ(model.advance(0.0), 0.0);
}

TEST(HarmonicODETest, ResetReturnsStateToZero)
{
    HarmonicODE model(1.5, 0.2);
    model.advance(1.0);
    model.reset();
    EXPECT_DOUBLE_EQ(model.advance(0.0), 0.0);
}

TEST(InputSignalTest, StepSignalIsConstantAtAnyStep)
{
    UnitStep signal(2.5);
    EXPECT_DOUBLE_EQ(signal.value(0), 2.5);
    EXPECT_DOUBLE_EQ(signal.value(1), 2.5);
    EXPECT_DOUBLE_EQ(signal.value(100), 2.5);
}

TEST(InputSignalTest, ImpulseIsOneOnlyAtStepOne)
{
    SingleImpulse signal;
    EXPECT_DOUBLE_EQ(signal.value(1), 1.0);
    EXPECT_DOUBLE_EQ(signal.value(0), 0.0);
    EXPECT_DOUBLE_EQ(signal.value(2), 0.0);
}

TEST(InputSignalTest, SinusoidMatchesSine)
{
    Sinusoid signal;
    EXPECT_DOUBLE_EQ(signal.value(0), 0.0);
    EXPECT_NEAR(signal.value(1), std::sin(1.0), 1e-12);
    EXPECT_NEAR(signal.value(3), std::sin(3.0), 1e-12);
}

TEST(ProcessFactoryTest, CreatesAllThreeModelsAndRejectsInvalidChoice)
{
    EXPECT_NE(ProcessFactory::create(1, 0.5, 1, 2, 3), nullptr);
    EXPECT_NE(ProcessFactory::create(2, 0.5, 1, 2, 3), nullptr);
    EXPECT_NE(ProcessFactory::create(3, 1, 0.1, 0, 0), nullptr);
    EXPECT_EQ(ProcessFactory::create(0, 0, 0, 0, 0), nullptr);
    EXPECT_EQ(ProcessFactory::create(4, 0, 0, 0, 0), nullptr);
}

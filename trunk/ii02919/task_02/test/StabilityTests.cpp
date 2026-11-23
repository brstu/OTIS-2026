#include <limits>
#include <gtest/gtest.h>
#include "StabilityAnalyzer.h"
#include "MultiStepModel.h"

TEST(StabilityAnalyzerTest, StableRootInsideUnitCircle)
{
    const auto result = StabilityAnalyzer::analyzeFirstOrder(0.75);
    ASSERT_EQ(result.roots.size(), 1u);
    EXPECT_DOUBLE_EQ(result.roots[0].real(), 0.75);
    EXPECT_TRUE(result.asymptoticallyStable);
    EXPECT_FALSE(result.marginallyStable);
}

TEST(StabilityAnalyzerTest, NegativeStableRootInsideUnitCircle)
{
    EXPECT_TRUE(StabilityAnalyzer::analyzeFirstOrder(-0.9).asymptoticallyStable);
}

TEST(StabilityAnalyzerTest, UnitCircleBoundaryAtPositiveOne)
{
    const auto result = StabilityAnalyzer::analyzeFirstOrder(1.0);
    EXPECT_FALSE(result.asymptoticallyStable);
    EXPECT_TRUE(result.marginallyStable);
}

TEST(StabilityAnalyzerTest, UnitCircleBoundaryAtNegativeOne)
{
    const auto result = StabilityAnalyzer::analyzeFirstOrder(-1.0);
    EXPECT_FALSE(result.asymptoticallyStable);
    EXPECT_TRUE(result.marginallyStable);
}

TEST(StabilityAnalyzerTest, RootOutsideUnitCircleIsUnstable)
{
    const auto result = StabilityAnalyzer::analyzeFirstOrder(1.2);
    EXPECT_FALSE(result.asymptoticallyStable);
    EXPECT_FALSE(result.marginallyStable);
}

TEST(StabilityAnalyzerTest, RejectsNonFiniteCoefficient)
{
    const auto result = StabilityAnalyzer::analyzeFirstOrder(std::numeric_limits<double>::infinity());
    EXPECT_FALSE(result.asymptoticallyStable);
    EXPECT_TRUE(result.roots.empty());
    EXPECT_NE(result.message.find("Invalid"), std::string::npos);
}

TEST(MultiStepModelStabilityTest, ExposesWarningForUnstableParameters)
{
    MultiStepModel model(1.1, 1.0, 0.0, 0.0);
    EXPECT_FALSE(model.isStableSystem());
    EXPECT_FALSE(model.alertText().empty());
}

TEST(MultiStepModelStabilityTest, NoWarningForStableParameters)
{
    MultiStepModel model(0.5, 1.0, 0.0, 0.0);
    EXPECT_TRUE(model.isStableSystem());
    EXPECT_TRUE(model.alertText().empty());
}

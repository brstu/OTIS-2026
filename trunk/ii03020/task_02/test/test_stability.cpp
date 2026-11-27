#include <gtest/gtest.h>
#include "StabilityAnalyzer.h"

// 1. Устойчивые параметры внутри единичного круга |z| < 1
TEST(StabilityAnalysisTest, StrictlyStablePositive) {
    EXPECT_TRUE(StabilityAnalyzer::isStableFirstOrder(0.0, false));
    EXPECT_TRUE(StabilityAnalyzer::isStableFirstOrder(0.1, false));
    EXPECT_TRUE(StabilityAnalyzer::isStableFirstOrder(0.5, false));
    EXPECT_TRUE(StabilityAnalyzer::isStableFirstOrder(0.75, false));
    EXPECT_TRUE(StabilityAnalyzer::isStableFirstOrder(0.999, false));
}

TEST(StabilityAnalysisTest, StrictlyStableNegative) {
    EXPECT_TRUE(StabilityAnalyzer::isStableFirstOrder(-0.1, false));
    EXPECT_TRUE(StabilityAnalyzer::isStableFirstOrder(-0.5, false));
    EXPECT_TRUE(StabilityAnalyzer::isStableFirstOrder(-0.75, false));
    EXPECT_TRUE(StabilityAnalyzer::isStableFirstOrder(-0.999, false));
}

// 2. Граничные значения единичной окружности |z| = 1
TEST(StabilityAnalysisTest, BoundaryValuesReject) {
    EXPECT_FALSE(StabilityAnalyzer::isStableFirstOrder(1.0, false));
    EXPECT_FALSE(StabilityAnalyzer::isStableFirstOrder(-1.0, false));
}

// 3. Неустойчивые расходящиеся параметры |z| > 1
TEST(StabilityAnalysisTest, UnstableValuesReject) {
    EXPECT_FALSE(StabilityAnalyzer::isStableFirstOrder(1.0001, false));
    EXPECT_FALSE(StabilityAnalyzer::isStableFirstOrder(1.5, false));
    EXPECT_FALSE(StabilityAnalyzer::isStableFirstOrder(-1.5, false));
    EXPECT_FALSE(StabilityAnalyzer::isStableFirstOrder(10.0, false));
    EXPECT_FALSE(StabilityAnalyzer::isStableFirstOrder(-10.0, false));
}

// 4. Проверка вывода предупреждения в поток std::cerr
TEST(StabilityAnalysisTest, WarningOutputEmitted) {
    testing::internal::CaptureStderr();
    StabilityAnalyzer::isStableFirstOrder(1.2, true);
    std::string output = testing::internal::GetCapturedStderr();
    EXPECT_FALSE(output.empty());
    EXPECT_NE(output.find("[WARNING]"), std::string::npos);
}
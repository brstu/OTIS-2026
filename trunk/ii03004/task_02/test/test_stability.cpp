#include <gtest/gtest.h>
#include "../src/StabilityAnalyzer.h"

// Тест проверяет асимптотическую устойчивость линейной системы (|a1| < 1)
TEST(StabilityAnalyzerTest, StableSystem) {
    double a1 = 0.7; // Коэффициент, при котором модель устойчива
    bool stable = StabilityAnalyzer::isLinearSystemStable(a1);
    EXPECT_TRUE(stable);
    EXPECT_DOUBLE_EQ(StabilityAnalyzer::calculateRoot(a1), 0.7);
}

// Тест проверяет неустойчивость системы, когда коэффициент равен или превышает единицу (|a1| >= 1)
TEST(StabilityAnalyzerTest, UnstableSystem) {
    double a1 = 1.2; // Коэффициент, приводящий к расходящемуся процессу
    bool stable = StabilityAnalyzer::isLinearSystemStable(a1);
    EXPECT_FALSE(stable);
}
#include <gtest/gtest.h>
#include "../src/StabilityAnalyzer.h"

// 14. Дискретная устойчивость (значения)
TEST(StabilityDiscrete, Stable) {
    EXPECT_TRUE(StabilityAnalyzer::isDiscreteStable(0.5));
    EXPECT_TRUE(StabilityAnalyzer::isDiscreteStable(-0.9));
}

// 15. Дискретная неустойчивость
TEST(StabilityDiscrete, Unstable) {
    EXPECT_FALSE(StabilityAnalyzer::isDiscreteStable(2.0));
    EXPECT_FALSE(StabilityAnalyzer::isDiscreteStable(-1.5));
}

// 16. Границы единичной окружности
TEST(StabilityDiscrete, Boundary) {
    EXPECT_FALSE(StabilityAnalyzer::isDiscreteStable(1.0));
    EXPECT_FALSE(StabilityAnalyzer::isDiscreteStable(-1.0));
}

// 17. Нулевой коэффициент (устойчив)
TEST(StabilityDiscrete, ZeroCoefficient) {
    EXPECT_TRUE(StabilityAnalyzer::isDiscreteStable(0.0));
}

// 18. Устойчивость Эйлера
TEST(StabilityEuler, Stable) {
    EXPECT_TRUE(StabilityAnalyzer::isEulerStable(0.5, 0.1)); 
}

// 19. Неустойчивость Эйлера
TEST(StabilityEuler, Unstable) {
    EXPECT_FALSE(StabilityAnalyzer::isEulerStable(10.0, 0.5)); 
}

// 20. Граница Эйлера (z = -1)
TEST(StabilityEuler, BoundaryMinusOne) {
    EXPECT_FALSE(StabilityAnalyzer::isEulerStable(2.0, 1.0));
}

// 21. Логика предупреждения
TEST(StabilityWarning, DetectsUnstableSystem) {
    EXPECT_FALSE(StabilityAnalyzer::isDiscreteStable(1.2));
    EXPECT_FALSE(StabilityAnalyzer::isEulerStable(10.0, 0.5));
}
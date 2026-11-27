#include <gtest/gtest.h>
#include "LinearARXModel.h"
#include "CubicODE.h"

// ============================================================
// Тесты для LinearARXModel.checkStability()
// ============================================================

// Устойчивая модель (a1=0.5, a2=0.3) — корни внутри единичного круга
TEST(StabilityARX, StableCase)
{
    LinearARXModel model(0.5, 0.3, 1.0, 0.5);
    EXPECT_TRUE(model.checkStability());
}

// Неустойчивая модель (a1=1.5, a2=0.5) — корень z1 > 1
TEST(StabilityARX, UnstableCase)
{
    LinearARXModel model(1.5, 0.5, 1.0, 0.5);
    EXPECT_FALSE(model.checkStability());
}

// Граница |z| = 1 — a1 = 2, a2 = -1 → z1 = 1, z2 = 1 (на границе)
TEST(StabilityARX, BorderlineCase)
{
    LinearARXModel model(2.0, -1.0, 1.0, 0.5);
    EXPECT_FALSE(model.checkStability());
}

// Комплексные корни — D < 0, устойчиво при |a2| < 1
TEST(StabilityARX, ComplexRootsStable)
{
    LinearARXModel model(0.0, -0.5, 1.0, 0.5);
    EXPECT_TRUE(model.checkStability());
}

// Комплексные корни — D < 0, неустойчиво при |a2| > 1
TEST(StabilityARX, ComplexRootsUnstable)
{
    LinearARXModel model(0.0, -4.0, 1.0, 0.5);
    EXPECT_FALSE(model.checkStability());
}

// ============================================================
// Тесты для CubicODE.checkStability()
// ============================================================

// Устойчивая схема: a=-1, dt=0.1, y=1 → |1 + 3*(-1)*1*0.1| = 0.7 < 1
TEST(StabilityCubic, StableCase)
{
    CubicODE model(-1.0, 1.0, 0.1);
    EXPECT_TRUE(model.checkStability());
}

// Неустойчивая схема: a=1, dt=0.1, y=1 → |1 + 3*1*1*0.1| = 1.3 > 1
TEST(StabilityCubic, UnstableCase)
{
    CubicODE model(1.0, 1.0, 0.1);
    EXPECT_FALSE(model.checkStability());
}
TEST(StabilityARX, ComplexBorderline)
{
    LinearARXModel model(0.0, -1.0, 1.0, 0.5);
    EXPECT_FALSE(model.checkStability());
}

TEST(StabilityARX, DoubleRoot)
{
    LinearARXModel model(2.0, -1.0, 1.0, 0.5);
    EXPECT_FALSE(model.checkStability());
}
TEST(StabilityARX, WarningMessageComplexUnstable)
{
    LinearARXModel model(0.0, -4.0, 1.0, 0.5);  // D < 0, |a2| > 1
    EXPECT_FALSE(model.getWarningMessage().empty());
    EXPECT_NE(model.getWarningMessage().find("complex roots"), std::string::npos);
}
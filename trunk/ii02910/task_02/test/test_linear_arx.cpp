#include <gtest/gtest.h>
#include "LinearARXModel.h"

// Тест 1: Один шаг с нулевым входом и нулевым начальным состоянием
TEST(LinearARXModel, ZeroInputZeroState)
{
    LinearARXModel model(0.5, 0.3, 1.0, 0.5);
    double y = model.step(0.0);
    EXPECT_NEAR(y, 0.0, 1e-9);
}

// Тест 2: Один шаг — расчёт по формуле
TEST(LinearARXModel, OneStepCalculation)
{
    LinearARXModel model(0.5, 0.3, 1.0, 0.5);
    // y_1 = 0.5 * 0 + 0.3 * 0 + 1.0 * 1 + 0.5 * 0 = 1.0
    double y = model.step(1.0);
    EXPECT_NEAR(y, 1.0, 1e-9);
}

// Тест 3: Два шага — проверка "памяти" модели
TEST(LinearARXModel, TwoSteps)
{
    LinearARXModel model(0.5, 0.3, 1.0, 0.5);
    model.step(1.0);                // y1 = 1.0
    double y2 = model.step(1.0);    // y2 = 0.5*1 + 0.3*0 + 1*1 + 0.5*1 = 2.0
    EXPECT_NEAR(y2, 2.0, 1e-9);
}

// Тест 4: reset() сбрасывает модель
TEST(LinearARXModel, ResetWorks)
{
    LinearARXModel model(0.5, 0.3, 1.0, 0.5);
    model.step(1.0);
    model.step(1.0);
    model.reset();
    double y = model.step(0.0);    // после сброса — 0
    EXPECT_NEAR(y, 0.0, 1e-9);
}

// Тест 5: Устойчивость (a1=0.5, a2=0.3 — устойчиво)
TEST(LinearARXModel, StableCoefficients)
{
    LinearARXModel model(0.5, 0.3, 1.0, 0.5);
    EXPECT_TRUE(model.checkStability());
}

// Тест 6: Неустойчивость (a1=1.5, a2=0.5 — неустойчиво)
TEST(LinearARXModel, UnstableCoefficients)
{
    LinearARXModel model(1.5, 0.5, 1.0, 0.5);
    EXPECT_FALSE(model.checkStability());
}

// Тест 7: getWarningMessage() для устойчивой модели — пустая строка
TEST(LinearARXModel, NoWarningWhenStable)
{
    LinearARXModel model(0.5, 0.3, 1.0, 0.5);
    EXPECT_EQ(model.getWarningMessage(), "");
}

// Тест 8: getWarningMessage() для неустойчивой — непустая строка
TEST(LinearARXModel, WarningWhenUnstable)
{
    LinearARXModel model(1.5, 0.5, 1.0, 0.5);
    EXPECT_FALSE(model.getWarningMessage().empty());
}

// Тест 9: getDescription()
TEST(LinearARXModel, Description)
{
    LinearARXModel model(0.5, 0.3, 1.0, 0.5);
    EXPECT_EQ(model.getDescription(), "Model 1.8 (Generalized Autoregressive Linear Model)");
}
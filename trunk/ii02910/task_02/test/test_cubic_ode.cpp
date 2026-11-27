#include <gtest/gtest.h>
#include "CubicODE.h"

// Тест 1: Один шаг — расчёт по методу Эйлера
TEST(CubicODE, OneStepCalculation)
{
    CubicODE model(-1.0, 1.0, 0.1);
    // y0 = 1.0, dy/dt = -1*1 + 1*1 = 0, y1 = 1.0 + 0.1*0 = 1.0
    double y = model.step(1.0);
    EXPECT_NEAR(y, 1.0, 1e-9);
}

// Тест 2: Два шага с постоянным входом
TEST(CubicODE, TwoSteps)
{
    CubicODE model(-1.0, 2.0, 0.1);
    // y0 = 1.0, dy/dt = -1*1 + 2*1 = 1, y1 = 1.0 + 0.1*1 = 1.1
    model.step(1.0);
    // dy/dt = -1*(1.1)^3 + 2*1 = -1.331 + 2 = 0.669, y2 = 1.1 + 0.1*0.669 = 1.1669
    double y = model.step(1.0);
    EXPECT_NEAR(y, 1.1669, 1e-4);
}

// Тест 3: Нулевой вход, положительное a — быстрый рост
TEST(CubicODE, ZeroInputPositiveA)
{
    CubicODE model(1.0, 0.0, 0.1);
    double y = model.step(0.0);
    // dy/dt = 1*1 = 1, y1 = 1.0 + 0.1*1 = 1.1
    EXPECT_NEAR(y, 1.1, 1e-9);
}

// Тест 4: Reset()
TEST(CubicODE, ResetWorks)
{
    CubicODE model(-1.0, 1.0, 0.1);
    model.step(1.0);
    model.step(1.0);
    model.reset();
    double y = model.step(1.0);
    EXPECT_NEAR(y, 1.0, 1e-9);
}

// Тест 5: getDt()
TEST(CubicODE, GetDt)
{
    CubicODE model(1.0, 1.0, 0.05);
    EXPECT_NEAR(model.getDt(), 0.05, 1e-9);
}

// Тест 6: getDescription()
TEST(CubicODE, Description)
{
    CubicODE model(1.0, 1.0, 0.1);
    EXPECT_EQ(model.getDescription(), "Model 3.6 (Cubic Growth and Control)");
}
// Тест 7: getWarningMessage() для неустойчивой модели
TEST(CubicODE, WarningWhenUnstable)
{
    CubicODE model(1.0, 1.0, 0.1);  // |1 + 3*1*1*0.1| = 1.3 > 1
    EXPECT_FALSE(model.getWarningMessage().empty());
}

// Тест 8: getWarningMessage() для устойчивой модели — пусто
TEST(CubicODE, NoWarningWhenStable)
{
    CubicODE model(-1.0, 1.0, 0.1);  // |1 + 3*(-1)*1*0.1| = 0.7 < 1
    EXPECT_EQ(model.getWarningMessage(), "");
}

// Тест 9: checkStability() с большим dt — неустойчиво
TEST(CubicODE, LargeDtUnstable)
{
    CubicODE model(-1.0, 1.0, 1.0);  // |1 + 3*(-1)*1*1| = 2 > 1
    EXPECT_FALSE(model.checkStability());
}
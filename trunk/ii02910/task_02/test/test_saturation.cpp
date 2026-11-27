#include <gtest/gtest.h>
#include "SaturationModel.h"

// Тест 1: Нулевой вход — нулевой выход
TEST(SaturationModel, ZeroInputZeroOutput)
{
    SaturationModel model(0.9, 1.0, -1.0, 1.0);
    double y = model.step(0.0);
    EXPECT_NEAR(y, 0.0, 1e-9);
}

// Тест 2: Вход в диапазоне — без обрезки
TEST(SaturationModel, InputInRange)
{
    SaturationModel model(0.9, 1.0, -1.0, 1.0);
    // y = 0.9*0 + 1.0 * sat(0.5) = 0.5
    double y = model.step(0.5);
    EXPECT_NEAR(y, 0.5, 1e-9);
}

// Тест 3: Вход больше U_max — обрезка до U_max
TEST(SaturationModel, InputAboveMax)
{
    SaturationModel model(0.9, 1.0, -1.0, 1.0);
    // y = 0.9*0 + 1.0 * sat(5.0) = 1.0 (обрезано)
    double y = model.step(5.0);
    EXPECT_NEAR(y, 1.0, 1e-9);
}

// Тест 4: Вход меньше U_min — обрезка до U_min
TEST(SaturationModel, InputBelowMin)
{
    SaturationModel model(0.9, 1.0, -1.0, 1.0);
    // y = 0.9*0 + 1.0 * sat(-5.0) = -1.0 (обрезано)
    double y = model.step(-5.0);
    EXPECT_NEAR(y, -1.0, 1e-9);
}

// Тест 5: Граница U_max — sat(U_max) = U_max
TEST(SaturationModel, BoundaryMax)
{
    SaturationModel model(0.9, 1.0, -1.0, 1.0);
    double y = model.step(1.0);
    EXPECT_NEAR(y, 1.0, 1e-9);
}

// Тест 6: Граница U_min — sat(U_min) = U_min
TEST(SaturationModel, BoundaryMin)
{
    SaturationModel model(0.9, 1.0, -1.0, 1.0);
    double y = model.step(-1.0);
    EXPECT_NEAR(y, -1.0, 1e-9);
}

// Тест 7: Два шага с постоянным входом
TEST(SaturationModel, TwoSteps)
{
    SaturationModel model(0.9, 1.0, -1.0, 1.0);
    model.step(1.0);                 // y1 = 1.0
    double y2 = model.step(1.0);     // y2 = 0.9*1.0 + 1.0*1.0 = 1.9
    EXPECT_NEAR(y2, 1.9, 1e-9);
}

// Тест 8: Reset()
TEST(SaturationModel, ResetWorks)
{
    SaturationModel model(0.9, 1.0, -1.0, 1.0);
    model.step(1.0);
    model.step(1.0);
    model.reset();
    double y = model.step(0.0);
    EXPECT_NEAR(y, 0.0, 1e-9);
}

// Тест 9: getDescription()
TEST(SaturationModel, Description)
{
    SaturationModel model(0.9, 1.0, -1.0, 1.0);
    EXPECT_EQ(model.getDescription(), "Model 2.2 (Actuator Saturation Non-linearity)");
}
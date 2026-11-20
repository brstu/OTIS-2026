#include <gtest/gtest.h>
#include <cmath>
#include "../../task_01/src/Model 1.10.h"
#include "../../task_01/src/Model 2.5.h"
#include "../../task_01/src/Model 3.10.h"
#include "../src/DeadZoneModel.h"

// 1. Ручной расчет (Модель 1.10)
TEST(Model10, ManualSteps) {
    LineModel_1_10 m(0.8, 1.0, 0.5);
    EXPECT_DOUBLE_EQ(m.step(1.0), 1.0);
    EXPECT_DOUBLE_EQ(m.step(1.0), 1.8);
}

// 2. Память на 2 шага назад (Модель 1.10)
TEST(Model10, MemoryTwoSteps) {
    LineModel_1_10 m(0.0, 1.0, 1.0); 
    EXPECT_DOUBLE_EQ(m.step(5.0), 5.0); 
    EXPECT_DOUBLE_EQ(m.step(0.0), 0.0); 
    EXPECT_DOUBLE_EQ(m.step(0.0), 5.0); 
}

// 3. Суммирование входов (Модель 1.10)
TEST(Model10, SumOfInputs) {
    LineModel_1_10 m(0.0, 1.0, 1.0); 
    m.step(1.0);
    m.step(2.0);
    EXPECT_DOUBLE_EQ(m.step(0.0), 1.0);
}

// 4. Проверка нелинейности (Модель 2.5)
TEST(Model25, SignBehavior) {
    NonlinearModel_2_5 m(0.5, 2.0);
    EXPECT_DOUBLE_EQ(m.step(0.0), 0.0);
    EXPECT_GT(m.step(4.0), 0.0);
    EXPECT_LT(m.step(-9.0), 0.0);
}

// 5. Экстремальные значения нелинейности (Модель 2.5)
TEST(Model25, ExtremeValues) {
    NonlinearModel_2_5 m(0.0, 1.0); 
    EXPECT_NEAR(m.step(100.0), 1.0, 1e-5);   
    m.reset();
    EXPECT_NEAR(m.step(-100.0), -1.0, 1e-5); 
}

// 6. Малые значения нелинейности (Модель 2.5)
TEST(Model25, SmallValues) {
    NonlinearModel_2_5 m(0.0, 1.0);
    EXPECT_NEAR(m.step(0.001), 0.001, 1e-6);
}

// 7. Границы зоны нечувствительности (DeadZone)
TEST(ModelDeadZone, BoundaryValues) {
    DeadZoneModel m(0.5, 1.0, 0.2); 
    EXPECT_DOUBLE_EQ(m.step(0.2), 0.0);
    EXPECT_DOUBLE_EQ(m.step(-0.2), 0.0);
    EXPECT_DOUBLE_EQ(m.step(0.3), 0.1);
}

// 8. Внутри зоны нечувствительности (DeadZone)
TEST(ModelDeadZone, InsideZone) {
    DeadZoneModel m(0.5, 1.0, 0.2);
    EXPECT_DOUBLE_EQ(m.step(0.1), 0.0);
    EXPECT_DOUBLE_EQ(m.step(-0.1), 0.0);
}

// 9. Инвариант покоя (Все модели)
TEST(Models, RestState) {
    LineModel_1_10 m1(0.8, 1.0, 0.5);
    NonlinearModel_2_5 m2(0.7, 0.5);
    Diff_3_10 m3(0.5, 0.0, 0.1);
    DeadZoneModel m4(0.5, 1.0, 0.2);

    EXPECT_DOUBLE_EQ(m1.step(0.0), 0.0);
    EXPECT_DOUBLE_EQ(m2.step(0.0), 0.0);
    EXPECT_NEAR(m3.step(0.0), 0.0, 1e-9); 
    EXPECT_DOUBLE_EQ(m4.step(0.0), 0.0);
}

// 10. Сброс состояния (Все модели)
TEST(Models, ResetFunction) {
    LineModel_1_10 m1(0.8, 1.0, 0.5);
    m1.step(5.0); m1.reset();   
    EXPECT_DOUBLE_EQ(m1.step(0.0), 0.0);

    NonlinearModel_2_5 m2(0.7, 0.5);
    m2.step(5.0); m2.reset();
    EXPECT_DOUBLE_EQ(m2.step(0.0), 0.0);
}

// 11. Имена моделей (ИСПРАВЛЕНО под оригинальные названия из ЛР1)
TEST(Models, GetName) {
    LineModel_1_10 m1(0.8, 1.0, 0.5);
    EXPECT_STREQ(m1.name(), "Linear Model 1.10"); // Исправлено

    NonlinearModel_2_5 m2(0.7, 0.5);
    EXPECT_STREQ(m2.name(), "Nonlinear Model 2.5"); // Исправлено
    
    Diff_3_10 m3(0.5, 0.2, 0.1);
    EXPECT_STREQ(m3.name(), "Diff 3.10");
}

// 12. Затухание Эйлера (Модель 3.10)
TEST(Model310, StableDecay) {
    Diff_3_10 m(0.5, 0.0, 0.1);
    EXPECT_DOUBLE_EQ(m.step(0.0), 0.0); 
    EXPECT_DOUBLE_EQ(m.step(0.0), 0.0); 
}

// 13. Сравнение Эйлера с аналитикой (Модель 3.10)
TEST(Model310, EulerVsAnalytic) {
    double a = 0.5;
    double dt = 0.001;
    double y = 1.0; 
    for (int i = 0; i < 1000; ++i) {
        y = y + dt * (-a * y); 
    }
    double analytical = std::exp(-a * 1.0);
    EXPECT_NEAR(y, analytical, 1e-3);
}
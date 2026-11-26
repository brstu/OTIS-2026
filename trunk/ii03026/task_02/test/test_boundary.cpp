#include <gtest/gtest.h>
#include "../src/Models.h"
#include "../src/Model.h"
#include <cmath>

// ============================================================================
// Тесты граничных значений нелинейностей (Boundary Value Testing)
// ============================================================================

// --- Модель 2.3: зона нечувствительности, границы u = delta и u = -delta ---

TEST(BoundaryValue_NonLinear_Deadzone, AtPositiveDelta) {
    // u = delta => deadzone = 0
    NonLinearModel2_3 model(0.5, 2.0, 0.5);
    EXPECT_DOUBLE_EQ(model.calculateNextStep(0.5, 0.0), 0.0);
}

TEST(BoundaryValue_NonLinear_Deadzone, AtNegativeDelta) {
    // u = -delta => deadzone = 0
    NonLinearModel2_3 model(0.5, 2.0, 0.5);
    EXPECT_DOUBLE_EQ(model.calculateNextStep(-0.5, 0.0), 0.0);
}

TEST(BoundaryValue_NonLinear_Deadzone, JustAbovePositiveDelta) {
    // u = delta + epsilon => deadzone = epsilon
    NonLinearModel2_3 model(0.5, 2.0, 0.5);
    double epsilon = 0.001;
    double expected = 2.0 * epsilon; // y = 0.5*0 + 2.0*epsilon
    EXPECT_NEAR(model.calculateNextStep(0.5 + epsilon, 0.0), expected, 1e-9);
}

TEST(BoundaryValue_NonLinear_Deadzone, JustBelowNegativeDelta) {
    // u = -delta - epsilon => deadzone = -epsilon
    NonLinearModel2_3 model(0.5, 2.0, 0.5);
    double epsilon = 0.001;
    double expected = 2.0 * (-epsilon); // y = 0.5*0 + 2.0*(-epsilon)
    EXPECT_NEAR(model.calculateNextStep(-0.5 - epsilon, 0.0), expected, 1e-9);
}

TEST(BoundaryValue_NonLinear_Deadzone, ZeroDelta) {
    // delta = 0 => любое u != 0 вне зоны
    NonLinearModel2_3 model(0.5, 2.0, 0.0);
    // u = 0 => deadzone = 0
    EXPECT_DOUBLE_EQ(model.calculateNextStep(0.0, 0.0), 0.0);
    // u = 0.1 > 0 => deadzone = 0.1
    EXPECT_DOUBLE_EQ(model.calculateNextStep(0.1, 0.0), 0.2);
    // u = -0.1 < 0 => deadzone = -0.1
    EXPECT_DOUBLE_EQ(model.calculateNextStep(-0.1, 0.0), -0.2);
}

TEST(BoundaryValue_NonLinear_Deadzone, LargeDelta) {
    // delta велико => всё u внутри зоны
    NonLinearModel2_3 model(0.5, 2.0, 100.0);
    EXPECT_DOUBLE_EQ(model.calculateNextStep(1.0, 0.0), 0.0);
    EXPECT_DOUBLE_EQ(model.calculateNextStep(-50.0, 0.0), 0.0);
}

// --- Инварианты и физический смысл ---

TEST(Invariant_Test, ZeroInputZeroState_Linear) {
    // При u=0 и y=0 система в покое
    LinearModel1_1 model(0.5, 2.0);
    EXPECT_DOUBLE_EQ(model.calculateNextStep(0.0, 0.0), 0.0);
    EXPECT_DOUBLE_EQ(model.calculateNextStep(0.0, 0.0), 0.0);
}

TEST(Invariant_Test, ZeroInputZeroState_NonLinear) {
    NonLinearModel2_3 model(0.5, 2.0, 0.5);
    EXPECT_DOUBLE_EQ(model.calculateNextStep(0.0, 0.0), 0.0);
}

TEST(Invariant_Test, ZeroInputZeroState_Differential) {
    DifferentialModel model(0.5, 0.2, 1.0);
    // Первый шаг: y(0) = b0 * u = 1.0 * 0 = 0
    EXPECT_DOUBLE_EQ(model.calculateNextStep(0.0, 0.0), 0.0);
    // Второй шаг: y(1) = a1*0 + a2*0 + 1.0*0 = 0
    EXPECT_DOUBLE_EQ(model.calculateNextStep(0.0, 0.0), 0.0);
}

// --- Проверка сходимости алгоритмов (стабильные модели сходятся к пределу) ---

TEST(Convergence_Test, LinearModelConverges) {
    // |a| < 1 => система сходится к стабильному состоянию
    LinearModel1_1 model(0.5, 1.0);
    double y = 0.0;
    double u = 1.0; // постоянное воздействие

    for (int i = 0; i < 100; ++i) {
        y = model.calculateNextStep(u, y);
    }

    // Стабильное состояние: y = a*y + u => y = u / (1 - a) = 1.0 / 0.5 = 2.0
    EXPECT_NEAR(y, 2.0, 1e-6);
}

TEST(Convergence_Test, NonLinearModelConverges) {
    // |a| < 1 => система сходится
    NonLinearModel2_3 model(0.5, 1.0, 0.5);
    double y = 0.0;
    double u = 1.0; // > delta => deadzone = 0.5

    for (int i = 0; i < 100; ++i) {
        y = model.calculateNextStep(u, y);
    }

    // Стабильное состояние: y = a*y + b*deadzone(u)
    // y = 0.5*y + 1.0*0.5 => y = 0.5 / 0.5 = 1.0
    EXPECT_NEAR(y, 1.0, 1e-6);
}

TEST(Convergence_Test, DifferentialModelStable) {
    // a1=0.5, a2=0.2, b0=1.0 => устойчивая (критерий Юра)
    DifferentialModel model(0.5, 0.2, 1.0);
    double y = 0.0;
    double u = 1.0;

    for (int i = 0; i < 100; ++i) {
        y = model.calculateNextStep(u, y);
    }

    // Стабильное состояние: y = a1*y + a2*y + b0*u => y = b0*u / (1 - a1 - a2)
    // y = 1.0 / (1 - 0.5 - 0.2) = 1.0 / 0.3 = 3.333...
    EXPECT_NEAR(y, 1.0 / 0.3, 1e-4);
}
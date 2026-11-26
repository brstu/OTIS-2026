#include <gtest/gtest.h>
#include "../src/Models.h"
#include "../src/Model.h"
#include <cmath>

// ============================================================================
// Тесты для LinearModel1_1
// ============================================================================

TEST(LinearModel1_1_Test, BasicCalculation) {
    // y(k) = 0.5 * y(k-1) + 2.0 * u(k)
    LinearModel1_1 model(0.5, 2.0);

    // Шаг 0: y(0) = 0.5 * 0 + 2.0 * 1.0 = 2.0
    EXPECT_DOUBLE_EQ(model.calculateNextStep(1.0, 0.0), 2.0);

    // Шаг 1: y(1) = 0.5 * 2.0 + 2.0 * 1.0 = 3.0
    EXPECT_DOUBLE_EQ(model.calculateNextStep(1.0, 2.0), 3.0);

    // Шаг 2: y(2) = 0.5 * 3.0 + 2.0 * 1.0 = 3.5
    EXPECT_DOUBLE_EQ(model.calculateNextStep(1.0, 3.0), 3.5);
}

TEST(LinearModel1_1_Test, ZeroInputZeroState) {
    // Инвариант: при u=0 и y=0 система в покое
    LinearModel1_1 model(0.5, 2.0);
    EXPECT_DOUBLE_EQ(model.calculateNextStep(0.0, 0.0), 0.0);
}

TEST(LinearModel1_1_Test, KnownCoefficients) {
    // Проверка с заведомо известными значениями
    // y(k) = 0.8 * y(k-1) + 1.0 * u(k), u(k) = 1 для всех k
    LinearModel1_1 model(0.8, 1.0);

    double y = 0.0;
    y = model.calculateNextStep(1.0, y); // y(0) = 1.0
    EXPECT_DOUBLE_EQ(y, 1.0);

    y = model.calculateNextStep(1.0, y); // y(1) = 0.8*1 + 1 = 1.8
    EXPECT_DOUBLE_EQ(y, 1.8);

    y = model.calculateNextStep(1.0, y); // y(2) = 0.8*1.8 + 1 = 2.44
    EXPECT_DOUBLE_EQ(y, 2.44);

    y = model.calculateNextStep(1.0, y); // y(3) = 0.8*2.44 + 1 = 2.952
    EXPECT_DOUBLE_EQ(y, 2.952);
}

TEST(LinearModel1_1_Test, NegativeCoefficient) {
    LinearModel1_1 model(-0.5, 1.0);

    double y = 0.0;
    y = model.calculateNextStep(1.0, y); // y(0) = 1.0
    EXPECT_DOUBLE_EQ(y, 1.0);

    y = model.calculateNextStep(1.0, y); // y(1) = -0.5*1 + 1 = 0.5
    EXPECT_DOUBLE_EQ(y, 0.5);

    y = model.calculateNextStep(1.0, y); // y(2) = -0.5*0.5 + 1 = 0.75
    EXPECT_DOUBLE_EQ(y, 0.75);
}

// ============================================================================
// Тесты для NonLinearModel2_3 (зона нечувствительности)
// ============================================================================

TEST(NonLinearModel2_3_Test, BasicCalculation) {
    // delta = 0.5, a = 0.5, b = 2.0
    NonLinearModel2_3 model(0.5, 2.0, 0.5);

    // u = 1.0 > delta => deadzone = 1.0 - 0.5 = 0.5
    // y(0) = 0.5 * 0 + 2.0 * 0.5 = 1.0
    EXPECT_DOUBLE_EQ(model.calculateNextStep(1.0, 0.0), 1.0);

    // u = 0.3 < delta => deadzone = 0
    // y(1) = 0.5 * 1.0 + 2.0 * 0 = 0.5
    EXPECT_DOUBLE_EQ(model.calculateNextStep(0.3, 1.0), 0.5);
}

TEST(NonLinearModel2_3_Test, DeadzonePositiveBoundary) {
    // Граничное значение: u = delta
    NonLinearModel2_3 model(0.5, 2.0, 0.5);

    // u = 0.5 = delta => deadzone = 0 (граничная точка)
    EXPECT_DOUBLE_EQ(model.calculateNextStep(0.5, 0.0), 0.0);
}

TEST(NonLinearModel2_3_Test, DeadzoneNegativeBoundary) {
    // Граничное значение: u = -delta
    NonLinearModel2_3 model(0.5, 2.0, 0.5);

    // u = -0.5 = -delta => deadzone = 0
    EXPECT_DOUBLE_EQ(model.calculateNextStep(-0.5, 0.0), 0.0);
}

TEST(NonLinearModel2_3_Test, DeadzoneInside) {
    // Внутри зоны нечувствительности: |u| < delta
    NonLinearModel2_3 model(0.5, 2.0, 1.0);

    // u = 0.5 < 1.0 => deadzone = 0
    EXPECT_DOUBLE_EQ(model.calculateNextStep(0.5, 0.0), 0.0);

    // u = -0.5 > -1.0 => deadzone = 0
    EXPECT_DOUBLE_EQ(model.calculateNextStep(-0.5, 0.0), 0.0);
}

TEST(NonLinearModel2_3_Test, DeadzoneOutsidePositive) {
    // Снаружи зоны: u > delta
    NonLinearModel2_3 model(0.5, 2.0, 0.5);

    // u = 1.5 > 0.5 => deadzone = 1.5 - 0.5 = 1.0
    // y = 0.5 * 0 + 2.0 * 1.0 = 2.0
    EXPECT_DOUBLE_EQ(model.calculateNextStep(1.5, 0.0), 2.0);
}

TEST(NonLinearModel2_3_Test, DeadzoneOutsideNegative) {
    // Снаружи зоны: u < -delta
    NonLinearModel2_3 model(0.5, 2.0, 0.5);

    // u = -1.5 < -0.5 => deadzone = -1.5 + 0.5 = -1.0
    // y = 0.5 * 0 + 2.0 * (-1.0) = -2.0
    EXPECT_DOUBLE_EQ(model.calculateNextStep(-1.5, 0.0), -2.0);
}

TEST(NonLinearModel2_3_Test, ZeroInputZeroState) {
    // Инвариант: при u=0 и y=0 система в покое
    NonLinearModel2_3 model(0.5, 2.0, 0.5);
    EXPECT_DOUBLE_EQ(model.calculateNextStep(0.0, 0.0), 0.0);
}

// ============================================================================
// Тесты для DifferentialModel (2-го порядка)
// ============================================================================

TEST(DifferentialModel_Test, BasicCalculation) {
    // y(k) = 0.5*y(k-1) + 0.2*y(k-2) + 1.0*u(k)
    DifferentialModel model(0.5, 0.2, 1.0);

    // Шаг 0: y(0) = 1.0 * 1.0 = 1.0
    EXPECT_DOUBLE_EQ(model.calculateNextStep(1.0, 0.0), 1.0);

    // Шаг 1: y(1) = 0.5*1.0 + 0.2*0.0 + 1.0*1.0 = 1.5
    EXPECT_DOUBLE_EQ(model.calculateNextStep(1.0, 0.0), 1.5);

    // Шаг 2: y(2) = 0.5*1.5 + 0.2*1.0 + 1.0*1.0 = 0.75 + 0.2 + 1.0 = 1.95
    EXPECT_DOUBLE_EQ(model.calculateNextStep(1.0, 0.0), 1.95);
}

TEST(DifferentialModel_Test, ZeroInputZeroState) {
    // Инвариант: при u=0 и нулевом начальном состоянии система в покое
    DifferentialModel model(0.5, 0.2, 1.0);

    // Сброс внутреннего состояния через создание новой модели
    DifferentialModel model2(0.5, 0.2, 1.0);
    EXPECT_DOUBLE_EQ(model2.calculateNextStep(0.0, 0.0), 0.0);
    EXPECT_DOUBLE_EQ(model2.calculateNextStep(0.0, 0.0), 0.0);
}

TEST(DifferentialModel_Test, KnownAnalyticalSolution) {
    // Проверка с аналитическим решением для простого случая
    // y(k) = y(k-1) (a1=1, a2=0, b0=0) - интегратор
    DifferentialModel model(1.0, 0.0, 0.0);

    double y = model.calculateNextStep(1.0, 0.0); // y(0) = 0
    EXPECT_DOUBLE_EQ(y, 0.0);

    y = model.calculateNextStep(1.0, 0.0); // y(1) = 1*0 + 0*0 + 0*1 = 0
    // Примечание: первая итерация использует firstStep, поэтому y(0) = b0*u = 0
    // На следующих шагах: y(k) = a1*y(k-1)

    y = model.calculateNextStep(1.0, 0.0); // y(2) = 1.0*0 + 0.0*0 + 0.0 = 0
    EXPECT_DOUBLE_EQ(y, 0.0);
}

TEST(DifferentialModel_Test, GetCharacteristicRootsReal) {
    // z^2 - 0.5*z - 0.2 = 0
    // Дискриминант = 0.25 + 0.8 = 1.05 > 0
    DifferentialModel model(0.5, 0.2, 1.0);
    auto roots = model.getCharacteristicRoots();

    ASSERT_EQ(roots.size(), 2);
    // z = (0.5 +/- sqrt(1.05)) / 2
    double sqrt_disc = std::sqrt(1.05);
    EXPECT_NEAR(roots[0].real(), (0.5 + sqrt_disc) / 2.0, 1e-10);
    EXPECT_NEAR(roots[1].real(), (0.5 - sqrt_disc) / 2.0, 1e-10);
    EXPECT_DOUBLE_EQ(roots[0].imag(), 0.0);
    EXPECT_DOUBLE_EQ(roots[1].imag(), 0.0);
}

TEST(DifferentialModel_Test, GetCharacteristicRootsComplex) {
    // z^2 - 0.5*z - 0.5 = 0
    // Дискриминант = 0.25 + 2.0 = 2.25 > 0... wait, this is positive
    // Let's use a1=0.5, a2=-0.5: z^2 - 0.5*z + 0.5 = 0
    // Дискриминант = 0.25 - 2.0 = -1.75 < 0
    DifferentialModel model(0.5, -0.5, 1.0);
    auto roots = model.getCharacteristicRoots();

    ASSERT_EQ(roots.size(), 2);
    EXPECT_NEAR(roots[0].real(), 0.25, 1e-10);
    EXPECT_NEAR(roots[1].real(), 0.25, 1e-10);
    EXPECT_NEAR(std::abs(roots[0].imag()), std::sqrt(1.75) / 2.0, 1e-10);
    EXPECT_NEAR(std::abs(roots[1].imag()), std::sqrt(1.75) / 2.0, 1e-10);
    EXPECT_DOUBLE_EQ(roots[0].imag(), -roots[1].imag());
}
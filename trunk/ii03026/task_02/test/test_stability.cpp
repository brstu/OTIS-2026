#include <gtest/gtest.h>
#include "../src/Models.h"
#include "../src/Model.h"
#include <cmath>

// ============================================================================
// Тесты модуля анализа устойчивости
// ============================================================================

// --- LinearModel1_1: проверка граничных значений Z-плоскости |z| = 1 ---

TEST(StabilityAnalyzer_Linear, Stable_When_AbsA_LessThan1) {
    // |a| < 1 => устойчивая модель
    LinearModel1_1 model(0.5, 1.0);
    EXPECT_TRUE(model.isStable());
}

TEST(StabilityAnalyzer_Linear, Stable_When_A_CloseTo1) {
    // Граничное значение: |a| близко к 1, но меньше
    LinearModel1_1 model(0.999, 1.0);
    EXPECT_TRUE(model.isStable());
}

TEST(StabilityAnalyzer_Linear, Unstable_When_A_Equals1) {
    // |a| = 1 => нестабильная (корень на единичном круге)
    LinearModel1_1 model(1.0, 1.0);
    EXPECT_FALSE(model.isStable());
}

TEST(StabilityAnalyzer_Linear, Unstable_When_A_EqualsMinus1) {
    LinearModel1_1 model(-1.0, 1.0);
    EXPECT_FALSE(model.isStable());
}

TEST(StabilityAnalyzer_Linear, Unstable_When_AbsA_GreaterThan1) {
    LinearModel1_1 model(1.5, 1.0);
    EXPECT_FALSE(model.isStable());
}

TEST(StabilityAnalyzer_Linear, Unstable_When_A_LessThanMinus1) {
    LinearModel1_1 model(-1.5, 1.0);
    EXPECT_FALSE(model.isStable());
}

TEST(StabilityAnalyzer_Linear, Unstable_When_A_IsZero) {
    // a = 0 => |0| < 1 => устойчивая
    LinearModel1_1 model(0.0, 1.0);
    EXPECT_TRUE(model.isStable());
}

// --- NonLinearModel2_3: проверка граничных значений Z-плоскости ---

TEST(StabilityAnalyzer_NonLinear, Stable_When_AbsA_LessThan1) {
    NonLinearModel2_3 model(0.5, 1.0, 0.5);
    EXPECT_TRUE(model.isStable());
}

TEST(StabilityAnalyzer_NonLinear, Unstable_When_A_Equals1) {
    NonLinearModel2_3 model(1.0, 1.0, 0.5);
    EXPECT_FALSE(model.isStable());
}

TEST(StabilityAnalyzer_NonLinear, Unstable_When_AbsA_GreaterThan1) {
    NonLinearModel2_3 model(1.5, 1.0, 0.5);
    EXPECT_FALSE(model.isStable());
}

// --- DifferentialModel: проверка критерия Юра ---

TEST(StabilityAnalyzer_Differential, Stable_UryaCriterion) {
    // a1 = 0.5, a2 = 0.2
    // 1) |0.2| < 1 ✓
    // 2) |0.5| < 1 + 0.2 = 1.2 ✓
    DifferentialModel model(0.5, 0.2, 1.0);
    EXPECT_TRUE(model.isStable());
}

TEST(StabilityAnalyzer_Differential, Unstable_When_AbsA2_GreaterThan1) {
    // a2 = 1.5 => |1.5| >= 1 => нестабильная
    DifferentialModel model(0.1, 1.5, 1.0);
    EXPECT_FALSE(model.isStable());
}

TEST(StabilityAnalyzer_Differential, Unstable_When_A1_ViolatesUrya) {
    // a1 = 1.5, a2 = 0.2
    // |1.5| < 1 + 0.2 = 1.2? Нет => нестабильная
    DifferentialModel model(1.5, 0.2, 1.0);
    EXPECT_FALSE(model.isStable());
}

TEST(StabilityAnalyzer_Differential, MarginallyStable_A2_Equals1) {
    // a2 = 1.0 => |a2| = 1 (граница)
    DifferentialModel model(0.1, 1.0, 1.0);
    EXPECT_FALSE(model.isStable());
}

TEST(StabilityAnalyzer_Differential, Stable_ComplexRootsInsideUnitCircle) {
    // a1 = 0.0, a2 = -0.5 => z^2 + 0.5 = 0 => z = +/- sqrt(-0.5)i = +/- 0.707i
    // |z| = 0.707 < 1 => устойчивая
    DifferentialModel model(0.0, -0.5, 1.0);
    EXPECT_TRUE(model.isStable());
}

TEST(StabilityAnalyzer_Differential, Unstable_ComplexRootsOutsideUnitCircle) {
    // a1 = 0.0, a2 = -2.0 => z^2 + 2.0 = 0 => z = +/- sqrt(2)i
    // |z| = 1.414 > 1 => нестабильная
    DifferentialModel model(0.0, -2.0, 1.0);
    EXPECT_FALSE(model.isStable());
}

// --- Проверка метода printStabilityInfo (не должен падать, выводить предупреждение) ---

TEST(StabilityAnalyzer_Print, PrintStableModel) {
    LinearModel1_1 model(0.5, 1.0);
    EXPECT_NO_FATAL_FAILURE(model.printStabilityInfo());
}

TEST(StabilityAnalyzer_Print, PrintUnstableModel) {
    LinearModel1_1 model(1.5, 1.0);
    EXPECT_NO_FATAL_FAILURE(model.printStabilityInfo());
}

TEST(StabilityAnalyzer_Print, PrintDifferentialStable) {
    DifferentialModel model(0.5, 0.2, 1.0);
    EXPECT_NO_FATAL_FAILURE(model.printStabilityInfo());
}

TEST(StabilityAnalyzer_Print, PrintDifferentialUnstable) {
    DifferentialModel model(1.5, 0.2, 1.0);
    EXPECT_NO_FATAL_FAILURE(model.printStabilityInfo());
}

// ============================================================================
// Параметризованный тест для проверки устойчивости модели 1.1 при разных a
// ============================================================================

class LinearStabilityParamTest : public ::testing::TestWithParam<double> {};

TEST_P(LinearStabilityParamTest, StabilityMatchesCriterion) {
    double a = GetParam();
    LinearModel1_1 model(a, 1.0);
    bool expected = (std::abs(a) < 1.0);
    EXPECT_EQ(model.isStable(), expected);
}

INSTANTIATE_TEST_SUITE_P(
    LinearStabilityCases,
    LinearStabilityParamTest,
    ::testing::Values(
        0.0,       // устойчивая
        0.5,       // устойчивая
        0.9,       // устойчивая (близка к границе)
        -0.5,      // устойчивая
        -0.9,      // устойчивая (близка к границе)
        1.0,       // НЕустойчивая (граница |z|=1)
        -1.0,      // НЕустойчивая (граница |z|=1)
        1.1,       // НЕустойчивая
        -1.1,      // НЕустойчивая
        2.0,       // НЕустойчивая
        -2.0       // НЕустойчивая
    )
);

// ============================================================================
// Параметризованный тест для дифференциальной модели (критерий Юра)
// ============================================================================

class DifferentialStabilityParamTest : public ::testing::TestWithParam<std::tuple<double, double, bool>> {};

TEST_P(DifferentialStabilityParamTest, StabilityMatchesUryaCriterion) {
    auto [a1, a2, expectedStable] = GetParam();
    DifferentialModel model(a1, a2, 1.0);
    EXPECT_EQ(model.isStable(), expectedStable);
}

INSTANTIATE_TEST_SUITE_P(
    DifferentialStabilityCases,
    DifferentialStabilityParamTest,
    ::testing::Values(
        // Устойчивые случаи
        std::make_tuple(0.5, 0.2, true),
        std::make_tuple(0.0, 0.0, true),
        std::make_tuple(0.0, -0.5, true),
        std::make_tuple(-0.5, 0.2, true),
        std::make_tuple(0.1, -0.1, true),
        // Нестабильные случаи
        std::make_tuple(1.5, 0.2, false),
        std::make_tuple(0.5, 1.5, false),
        std::make_tuple(0.0, -2.0, false),
        std::make_tuple(1.0, 1.0, false)
    )
);
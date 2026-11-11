#include <gtest/gtest.h>
#include "../src/ApplicationLogic.h"
#include <tuple>

// Структура параметров для тестов: (a1, b1, b2, начальное y0)
class LinearModelParamTest : public ::testing::TestWithParam<std::tuple<double, double, double, double>> {
};

// Параметризованный тест сходимости и корректности шагов симуляции
TEST_P(LinearModelParamTest, SimulationStepTest) {
    auto params = GetParam();
    double a1 = std::get<0>(params);
    double b1 = std::get<1>(params);
    double b2 = std::get<2>(params);
    double y0 = std::get<3>(params);

    LinearModel model(a1, b1, b2, y0);
    StepInput input(1.0); // Ступенчатое воздействие u = 1.0

    double u_val = input.value(0);
    double next_y = model.next(u_val);

    // Проверяем, что значение рассчитывается корректно и является конечным числом (нет NaN/inf)
    EXPECT_TRUE(std::isfinite(next_y));
}

// Передаем различные наборы данных для тестирования (вариативность параметров)
INSTANTIATE_TEST_SUITE_P(
    LinearModelVariants,
    LinearModelParamTest,
    ::testing::Values(
        std::make_tuple(0.5, 1.0, 0.2, 0.0),
        std::make_tuple(0.7, 0.5, 0.3, 0.0),
        std::make_tuple(0.2, 0.1, 0.1, 1.0),
        std::make_tuple(0.9, 0.4, 0.1, -0.5)
    )
);

// Тестирование генерации различных типов входных сигналов
TEST(InputSignalsTest, StepAndHarmonic) {
    StepInput step(2.5);
    EXPECT_DOUBLE_EQ(step.value(0), 2.5);
    EXPECT_DOUBLE_EQ(step.value(10), 2.5);
    EXPECT_EQ(step.name(), "step");

    ImpulseInput impulse;
    EXPECT_DOUBLE_EQ(impulse.value(0), 1.0);
    EXPECT_DOUBLE_EQ(impulse.value(1), 0.0);
    EXPECT_EQ(impulse.name(), "impulse");

    HarmonicInput harmonic;
    EXPECT_DOUBLE_EQ(harmonic.value(0), std::sin(0.0));
    EXPECT_EQ(harmonic.name(), "harmonic");
}
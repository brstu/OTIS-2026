#include <gtest/gtest.h>

#include <vector>

#include "Model1_6.h"
#include "Model3_10.h"
#include "Simulation.h"
#include "TestUtils.h"

using otis_test::kEps;

TEST(Simulation, RowCountAndInputRecord)
{
    {
        SCOPED_TRACE("число строк и индексы тактов");
        Model3_10 model(0.5, 1.0);
        const CallableInputSource input([](int) { return 1.0; });
        const auto rows =
            simulate(model, SimulationConfig{5, 0.0, 0.1}, input);
        ASSERT_EQ(rows.size(), 6u);
        for (std::size_t i = 0; i < rows.size(); ++i)
            EXPECT_EQ(rows[i].tau, static_cast<int>(i));
    }
    {
        SCOPED_TRACE("вход записывается в каждую строку");
        Model3_10 model(0.5, 0.0);
        const CallableInputSource input([](int tau) { return static_cast<double>(tau); });
        const auto rows =
            simulate(model, SimulationConfig{3, 0.0, 0.1}, input);
        ASSERT_EQ(rows.size(), 4u);
        for (std::size_t i = 0; i < rows.size(); ++i)
            EXPECT_DOUBLE_EQ(rows[i].u, static_cast<double>(i));
    }
}

TEST(Simulation, ZeroAndNegativeSteps)
{
    {
        SCOPED_TRACE("нулевое число шагов");
        Model1_6 model(0.5, 0.2, 0.1, 2.0);
        const CallableInputSource input([](int) { return 7.0; });
        const auto rows = simulate(model, SimulationConfig{0, 3.5, 1.0}, input);
        ASSERT_EQ(rows.size(), 1u);
        EXPECT_EQ(rows[0].tau, 0);
        EXPECT_DOUBLE_EQ(rows[0].y, 3.5);
        EXPECT_DOUBLE_EQ(rows[0].u, 7.0);
    }
    {
        SCOPED_TRACE("отрицательное число шагов трактуется как ноль");
        Model1_6 model(0.5, 0.2, 0.1, 2.0);
        const CallableInputSource input([](int) { return 0.0; });
        const auto rows = simulate(model, SimulationConfig{-5, 1.0, 1.0}, input);
        EXPECT_EQ(rows.size(), 1u);
        EXPECT_DOUBLE_EQ(rows[0].y, 1.0);
    }
}

TEST(Simulation, HistoryIsShiftedCorrectly)
{
    {
        SCOPED_TRACE("постоянное состояние при единичном коэффициенте");
        Model1_6 model(1.0, 0.0, 0.0, 0.0);
        const CallableInputSource input([](int) { return 0.0; });
        const auto rows = simulate(model, SimulationConfig{3, 2.0, 1.0}, input);
        for (const SimStep& row : rows)
            EXPECT_DOUBLE_EQ(row.y, 2.0);
    }
    {
        SCOPED_TRACE("память первого порядка сохраняет значение");
        Model1_6 model(0.0, 1.0, 0.0, 0.0);
        const auto values = otis_test::runConstant(model, 1.0, 0.0, 4);
        for (double value : values)
            EXPECT_NEAR(value, 1.0, kEps);
    }
    {
        SCOPED_TRACE("память второго порядка сохраняет значение");
        Model1_6 model(0.0, 0.0, 1.0, 0.0);
        const auto values = otis_test::runConstant(model, 5.0, 0.0, 4);
        for (double value : values)
            EXPECT_NEAR(value, 5.0, kEps);
    }
}

TEST(Simulation, SignalsProduceTrajectories)
{
    {
        SCOPED_TRACE("ступенька отличается от импульса");
        Model1_6 model(0.0, 0.0, 0.0, 1.0);
        const auto step =
            otis_test::runSignal(model, 0.0, 3, [](int) { return 1.0; });
        const auto impulse =
            otis_test::runSignal(model, 0.0, 3, [](int tau) { return tau == 0 ? 1.0 : 0.0; });
        otis_test::expectNear(step, {0.0, 1.0, 1.0, 1.0});
        otis_test::expectNear(impulse, {0.0, 1.0, 0.0, 0.0});
    }
    {
        SCOPED_TRACE("гармонический вход повторяет sin(tau)*A");
        Model1_6 model(0.0, 0.0, 0.0, 1.0);
        const SignalInputSource input(InputKind::Harmonic, 2.0);
        const auto rows = simulate(model, SimulationConfig{6, 0.0, 1.0}, input);
        for (std::size_t i = 1; i < rows.size(); ++i)
            EXPECT_NEAR(rows[i].y, 2.0 * std::sin(static_cast<double>(i - 1)), 1e-12);
    }
}

TEST(Simulation, ConfigDefaults)
{
    const SimulationConfig config;
    EXPECT_EQ(config.steps, 10);
    EXPECT_DOUBLE_EQ(config.y0, 0.0);
    EXPECT_DOUBLE_EQ(config.dt, 1.0);
}

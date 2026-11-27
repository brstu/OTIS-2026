#include <gtest/gtest.h>

#include <cmath>
#include <vector>

#include "Model2_8.h"
#include "TestUtils.h"

using otis_test::kEps;

TEST(Model2_8, ManualCalculationsMatchFormula)
{
    {
        SCOPED_TRACE("последовательность первых шагов");

        Model2_8 model(1.5, 0.5, 0.25);
        otis_test::expectNear(
            otis_test::runConstant(model, 0.5, 0.2, 3),
            {0.5, 0.499958354161707, 0.499958351560143, 0.499956279669824}, 1e-12);
    }
    {
        SCOPED_TRACE("первый шаг из нуля");

        Model2_8 model(1.5, 2.0, 0.25);
        EXPECT_NEAR(model.computeNext(0.0, 0.0, 0.0, 0.5, 1.0), 1.0, kEps);
    }
    {
        SCOPED_TRACE("прямая проверка computeNext");
        Model2_8 model(0.8, 1.2, 0.4);
        const double y = 0.3;
        const double yPrev = 0.7;
        const double u = 0.9;
        const double expected = 0.8 * y * (1.0 - y) + 1.2 * u + 0.4 * std::sin(yPrev * u);
        EXPECT_NEAR(model.computeNext(y, yPrev, 0.0, u, 1.0), expected, kEps);
    }
}

TEST(Model2_8, IgnoresUnusedArguments)
{
    Model2_8 model(1.5, 0.5, 0.25);

    EXPECT_DOUBLE_EQ(model.computeNext(0.4, 0.6, 0.0, 1.0, 1.0),
                     model.computeNext(0.4, 0.6, 123.0, 1.0, 1.0));

    EXPECT_DOUBLE_EQ(model.computeNext(0.4, 0.6, 0.2, 1.0, 0.5),
                     model.computeNext(0.4, 0.6, 0.2, 1.0, 1.0));
}

TEST(Model2_8, ZeroAndFixedCases)
{
    {
        SCOPED_TRACE("a = b = c = 0: всегда ноль");
        Model2_8 model(0.0, 0.0, 0.0);
        const auto actual = otis_test::runConstant(model, 0.3, 1.0, 3);
        ASSERT_EQ(actual.size(), 4u);
        EXPECT_DOUBLE_EQ(actual[0], 0.3);
        for (std::size_t i = 1; i < actual.size(); ++i)
            EXPECT_DOUBLE_EQ(actual[i], 0.0);
    }
    {
        SCOPED_TRACE("неподвижная точка y = 0.5 (a = 2, u = 0)");
        Model2_8 model(2.0, 1.0, 0.5);
        for (double value : otis_test::runConstant(model, 0.5, 0.0, 3))
            EXPECT_NEAR(value, 0.5, kEps);
    }
    {
        SCOPED_TRACE("нулевое состояние и нулевой вход");
        Model2_8 model(1.0, 0.0, 1.0);
        for (double value : otis_test::runConstant(model, 0.0, 0.0, 3))
            EXPECT_DOUBLE_EQ(value, 0.0);
    }
    {
        SCOPED_TRACE("нулевое состояние, ненулевой вход: y(1) = b*u");
        Model2_8 model(1.0, 2.0, 5.0);
        EXPECT_NEAR(model.computeNext(0.0, 0.0, 0.0, 3.0, 1.0), 6.0, kEps);
    }
    {
        SCOPED_TRACE("логистический член равен нулю на границах");
        Model2_8 model(3.5, 0.0, 0.0);
        EXPECT_DOUBLE_EQ(model.computeNext(0.0, 0.0, 0.0, 0.0, 1.0), 0.0);
        EXPECT_DOUBLE_EQ(model.computeNext(1.0, 1.0, 1.0, 0.0, 1.0), 0.0);
    }
    {
        SCOPED_TRACE("синус исчезает при y(t-1) = 0");
        Model2_8 model(0.5, 0.0, 100.0);
        EXPECT_NEAR(model.computeNext(0.4, 0.0, 0.0, 5.0, 1.0), 0.5 * 0.4 * 0.6, kEps);
    }
}

TEST(Model2_8, BoundsAndFiniteValues)
{
    {
        SCOPED_TRACE("синусоидальный вклад ограничен |c|");
        Model2_8 model(0.0, 0.0, 0.25);
        for (int tau = 0; tau < 50; ++tau)
        {
            const double value = model.computeNext(0.0, 0.7, 0.0, 0.1 * tau, 1.0);
            EXPECT_LE(std::abs(value), 0.25 + kEps);
        }
    }
    {
        SCOPED_TRACE("жёсткие параметры не дают NaN/Inf");
        Model2_8 model(4.0, 3.0, 2.0);
        for (int tau = 0; tau < 100; ++tau)
            EXPECT_TRUE(std::isfinite(model.computeNext(0.6, 0.4, 0.2, 10.0 + tau, 1.0)))
                << "tau = " << tau;
    }
}

TEST(Model2_8, CoefficientsAndName)
{
    Model2_8 model(1.25, -0.5, 0.75);
    EXPECT_DOUBLE_EQ(model.getA(), 1.25);
    EXPECT_DOUBLE_EQ(model.getB(), -0.5);
    EXPECT_DOUBLE_EQ(model.getC(), 0.75);

    ASSERT_NE(model.label(), nullptr);
    EXPECT_NE(std::string(model.label()).find("2.8"), std::string::npos);
}

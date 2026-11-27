#include <gtest/gtest.h>

#include <cmath>
#include <vector>

#include "Model3_10.h"
#include "TestUtils.h"

using otis_test::kEps;

TEST(Model3_10, ManualCalculationsMatchFormula)
{
    {
        SCOPED_TRACE("первые шаги: a = 0.5, b = 1, dt = 0.1, u = 1");

        Model3_10 model(0.5, 1.0);
        otis_test::expectNear(otis_test::runConstant(model, 0.0, 1.0, 3, 0.1),
                              {0.0, 0.2, 0.39, 0.5705}, 1e-12);
    }
    {
        SCOPED_TRACE("свободное движение: q = 0.5");
        Model3_10 model(0.5, 0.0);
        otis_test::expectNear(otis_test::runConstant(model, 2.0, 0.0, 4, 1.0),
                              {2.0, 1.0, 0.5, 0.25, 0.125}, 1e-12);
    }
    {
        SCOPED_TRACE("прямая проверка computeNext");
        Model3_10 model(1.3, -0.4);
        const double y = 0.7;
        const double u = 2.5;
        const double dt = 0.05;
        EXPECT_NEAR(model.computeNext(y, y, y, u, dt), y + dt * (-1.3 * y + (-0.4) + u), kEps);
    }
}

TEST(Model3_10, IgnoresPreviousValues)
{

    Model3_10 model(1.0, 0.0);
    EXPECT_DOUBLE_EQ(model.computeNext(0.5, 0.0, 0.0, 1.0, 0.1),
                     model.computeNext(0.5, 9.0, -7.0, 1.0, 0.1));
}

TEST(Model3_10, EulerErrorDecreasesWithSmallerStep)
{

    const double a = 1.0;
    const double totalTime = 0.3;

    auto errorFor = [&](double dt)
    {
        Model3_10 model(a, 0.0);
        const auto steps = static_cast<int>(std::lround(totalTime / dt));
        const auto values = otis_test::runConstant(model, 1.0, 0.0, steps, dt);
        return std::abs(values.back() - std::exp(-a * dt * steps));
    };

    const double coarse = errorFor(0.1);
    const double fine = errorFor(0.01);

    EXPECT_LT(fine, coarse);
    EXPECT_NEAR(coarse, 0.011818, 1e-5);
    EXPECT_NEAR(fine, 0.0011178, 1e-5);
}

TEST(Model3_10, BoundaryCases)
{
    {
        SCOPED_TRACE("dt = 0: интегрирования нет");
        Model3_10 model(2.0, 5.0);
        for (double value : otis_test::runConstant(model, 3.0, 10.0, 5, 0.0))
            EXPECT_DOUBLE_EQ(value, 3.0);
    }
    {
        SCOPED_TRACE("a*dt = 2: незатухающие колебания");
        Model3_10 model(2.0, 0.0);
        otis_test::expectNear(otis_test::runConstant(model, 1.0, 0.0, 4, 1.0),
                              {1.0, -1.0, 1.0, -1.0, 1.0}, 1e-12);
    }
    {
        SCOPED_TRACE("чистый интегратор: a = b = 0");
        Model3_10 model(0.0, 0.0);
        otis_test::expectNear(otis_test::runConstant(model, 0.0, 1.0, 3, 0.5),
                              {0.0, 0.5, 1.0, 1.5}, 1e-12);
    }
    {
        SCOPED_TRACE("смещение равновесия к b/a = 2");
        Model3_10 model(1.0, 2.0);
        const auto actual = otis_test::runConstant(model, 0.0, 0.0, 200, 0.1);
        EXPECT_NEAR(actual.back(), 2.0, 1e-6);
    }
    {
        SCOPED_TRACE("антисимметрия (b, u) -> (-b, -u)");
        Model3_10 positive(1.0, 1.0);
        Model3_10 negative(1.0, -1.0);
        const auto a = otis_test::runConstant(positive, 0.0, 1.0, 5, 0.1);
        const auto b = otis_test::runConstant(negative, 0.0, -1.0, 5, 0.1);
        std::vector<double> negatedB(b.size());
        for (std::size_t i = 0; i < b.size(); ++i)
            negatedB[i] = -b[i];
        otis_test::expectNear(a, negatedB, 1e-12);
    }
}

TEST(Model3_10, CoefficientsAndName)
{
    Model3_10 model(0.75, -1.5);
    EXPECT_DOUBLE_EQ(model.getA(), 0.75);
    EXPECT_DOUBLE_EQ(model.getB(), -1.5);

    ASSERT_NE(model.label(), nullptr);
    EXPECT_NE(std::string(model.label()).find("3.10"), std::string::npos);
}

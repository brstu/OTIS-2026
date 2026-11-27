#include <gtest/gtest.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "Model1_6.h"
#include "TestUtils.h"

using otis_test::kEps;

TEST(Model1_6, ManualCalculationsMatchFormula)
{
    {
        SCOPED_TRACE("ступенька, y0 = 0");

        Model1_6 model(0.5, 0.2, 0.1, 2.0);
        otis_test::expectNear(otis_test::runConstant(model, 0.0, 1.0, 3),
                              {0.0, 2.0, 3.0, 3.9});
    }
    {
        SCOPED_TRACE("ступенька, y0 = 1");
        Model1_6 model(0.5, 0.2, 0.1, 2.0);
        otis_test::expectNear(otis_test::runConstant(model, 1.0, 1.0, 2),
                              {1.0, 2.8, 3.7});
    }
    {
        SCOPED_TRACE("импульс A = 2, y0 = 0.25");
        Model1_6 model(0.3, -0.1, 0.05, 0.5);
        otis_test::expectNear(
            otis_test::runSignal(model, 0.25, 2, [](int tau) { return tau == 0 ? 2.0 : 0.0; }),
            {0.25, 1.0625, 0.30625});
    }
    {
        SCOPED_TRACE("прямая проверка computeNext");
        Model1_6 model(0.4, -0.3, 0.2, 1.5);
        const double result = model.computeNext(1.0, 2.0, 3.0, 4.0, 1.0);
        EXPECT_NEAR(result, 0.4 * 1.0 + (-0.3) * 2.0 + 0.2 * 3.0 + 1.5 * 4.0, kEps);
        EXPECT_DOUBLE_EQ(result, 6.4);
    }
}

TEST(Model1_6, IgnoresDtParameter)
{

    Model1_6 model(0.5, 0.2, 0.1, 2.0);
    EXPECT_DOUBLE_EQ(model.computeNext(1.0, 1.0, 1.0, 1.0, 1.0),
                     model.computeNext(1.0, 1.0, 1.0, 1.0, 100.0));
    EXPECT_DOUBLE_EQ(model.computeNext(1.0, 1.0, 1.0, 1.0, 0.001),
                     model.computeNext(1.0, 1.0, 1.0, 1.0, 1.0));
}

TEST(Model1_6, BoundaryCases)
{
    {
        SCOPED_TRACE("u = 0: только свободное движение");
        Model1_6 model(0.5, 0.2, 0.1, 2.0);
        otis_test::expectNear(otis_test::runConstant(model, 1.0, 0.0, 2),
                              {1.0, 0.8, 0.7});
    }
    {
        SCOPED_TRACE("u = 0, y0 = 0: состояние покоя");
        Model1_6 model(0.9, -0.5, 0.3, 7.0);
        for (double value : otis_test::runConstant(model, 0.0, 0.0, 20))
            EXPECT_DOUBLE_EQ(value, 0.0);
    }
    {
        SCOPED_TRACE("a1 = a2 = a3 = 0: проход входа");
        Model1_6 model(0.0, 0.0, 0.0, 1.0);
        otis_test::expectNear(otis_test::runConstant(model, 0.7, 0.3, 3),
                              {0.7, 0.3, 0.3, 0.3});
    }
    {
        SCOPED_TRACE("b = 0: вход не влияет");
        Model1_6 model(0.5, 0.2, 0.1, 0.0);
        otis_test::expectNear(otis_test::runConstant(model, 0.0, 100.0, 3),
                              otis_test::runConstant(model, 0.0, 0.0, 3));
    }
    {
        SCOPED_TRACE("a1 = 1: состояние сохраняется (граница)");
        Model1_6 model(1.0, 0.0, 0.0, 0.0);
        for (double value : otis_test::runConstant(model, 1.0, 0.0, 5))
            EXPECT_DOUBLE_EQ(value, 1.0);
    }
    {
        SCOPED_TRACE("знак входа учитывается линейно");
        Model1_6 model(0.0, 0.0, 0.0, 2.0);
        EXPECT_DOUBLE_EQ(model.computeNext(0.0, 0.0, 0.0, -1.0, 1.0), -2.0);
        EXPECT_DOUBLE_EQ(model.computeNext(0.0, 0.0, 0.0, 0.0, 1.0), 0.0);
    }
}

TEST(Model1_6, LinearityAndSuperposition)
{
    {
        SCOPED_TRACE("линейность по начальному состоянию");
        Model1_6 model(0.5, 0.2, 0.1, 0.0);
        const auto a = otis_test::runConstant(model, 1.0, 0.0, 4);
        const auto b = otis_test::runConstant(model, 2.0, 0.0, 4);
        ASSERT_EQ(a.size(), b.size());
        for (std::size_t i = 0; i < a.size(); ++i)
            EXPECT_NEAR(b[i], 2.0 * a[i], kEps);
    }
    {
        SCOPED_TRACE("суперпозиция входов");
        Model1_6 model(0.9, -0.3, 0.1, 1.0);
        const auto y1 = otis_test::runConstant(model, 0.0, 1.0, 4);
        const auto y2 = otis_test::runConstant(model, 0.0, 2.0, 4);
        ASSERT_EQ(y1.size(), y2.size());
        for (std::size_t i = 0; i < y1.size(); ++i)
            EXPECT_NEAR(y2[i], 2.0 * y1[i], kEps);
    }
}

TEST(Model1_6, CoefficientsAndName)
{
    Model1_6 model(0.11, -0.22, 0.33, 0.44);
    EXPECT_DOUBLE_EQ(model.getA1(), 0.11);
    EXPECT_DOUBLE_EQ(model.getA2(), -0.22);
    EXPECT_DOUBLE_EQ(model.getA3(), 0.33);
    EXPECT_DOUBLE_EQ(model.getB(), 0.44);

    ASSERT_NE(model.label(), nullptr);
    EXPECT_NE(std::string(model.label()).find("1.6"), std::string::npos);
}

struct Model16Case
{
    double a1;
    double a2;
    double a3;
    double b;
    double y0;
    double u;
    int steps;
    std::vector<double> expected;
};

class Model16ParamTest : public ::testing::TestWithParam<Model16Case>
{
};

TEST_P(Model16ParamTest, SimulationMatchesManualCalculation)
{
    const Model16Case& param = GetParam();
    Model1_6 model(param.a1, param.a2, param.a3, param.b);

    const auto actual =
        otis_test::runConstant(model, param.y0, param.u, param.steps);

    otis_test::expectNear(actual, param.expected);
}

INSTANTIATE_TEST_SUITE_P(
    Coefficients, Model16ParamTest,
    ::testing::Values(

        Model16Case{0.5, 0.2, 0.1, 2.0, 0.0, 1.0, 3, {0.0, 2.0, 3.0, 3.9}},

        Model16Case{0.5, 0.2, 0.1, 2.0, 1.0, 1.0, 3, {1.0, 2.8, 3.7, 4.51}},

        Model16Case{0.9, -0.2, 0.05, 1.0, 0.5, 0.5, 3, {0.5, 0.875, 1.2125, 1.44125}},

        Model16Case{0.0, 0.0, 0.0, 1.0, 0.7, 0.3, 3, {0.7, 0.3, 0.3, 0.3}},

        Model16Case{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 3, {1.0, 1.0, 1.0, 1.0}},

        Model16Case{0.5, 0.2, 0.1, 2.0, 0.0, 0.0, 3, {0.0, 0.0, 0.0, 0.0}},

        Model16Case{-0.4, 0.3, -0.2, 0.5, 2.0, -1.5, 3, {2.0, -1.35, -0.01, -1.551}},

        Model16Case{0.33, 0.33, 0.34, 1.0, 0.0, 2.0, 3, {0.0, 2.0, 2.66, 3.5378}},

        Model16Case{0.8, -0.1, 0.0, 0.5, 1.0, 1.0, 3, {1.0, 1.2, 1.36, 1.468}}
    ),
    [](const ::testing::TestParamInfo<Model16Case>& info)
    {

        std::ostringstream name;
        name << "a1_" << info.param.a1 << "_a2_" << info.param.a2
             << "_a3_" << info.param.a3 << "_b_" << info.param.b
             << "_y0_" << info.param.y0 << "_u_" << info.param.u;
        std::string text = name.str();
        for (char& symbol : text)
        {
            if (!std::isalnum(static_cast<unsigned char>(symbol)))
                symbol = '_';
        }
        return text;
    });

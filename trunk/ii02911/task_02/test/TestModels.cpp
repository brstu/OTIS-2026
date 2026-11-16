#include <gtest/gtest.h>

#include "Model.h"
#include "Model1_9.h"
#include "Model2_6.h"
#include "Model3_9.h"

#include <memory>
#include <tuple>
#include <string>
#include <cmath>

class ModelParamTest :
    public ::testing::TestWithParam<
        std::tuple<std::string, double, double>>
{
};

TEST_P(ModelParamTest, CalculationTest)
{
    auto [modelName, uVal, dtVal] = GetParam();

    std::unique_ptr<Model> model;
    std::string expectedName;

    if (modelName == "Model1_9")
    {
        model = std::make_unique<Model1_9>(
            0.5, 0.3, 0.1, 1.0
        );

        expectedName =
            "Model 1.9 - Three-Step Moving Average State Model";
    }
    else if (modelName == "Model2_6")
    {
        model = std::make_unique<Model2_6>(
            0.4, 0.2, 0.8
        );

        expectedName =
            "Model 2.6 - Asymmetric Polynomial Backlash Simulation";
    }
    else
    {
        model = std::make_unique<Model3_9>(0.5);

        expectedName =
            "Model 3.9 - Bounded Saturation Rate";
    }

    ASSERT_NE(model, nullptr);

    EXPECT_STREQ(
        model->getName(),
        expectedName.c_str()
    );

    double y = 1.0;
    double yPrev = 0.8;
    double yPrev2 = 0.5;

    double nextY = model->calculateNext(
        y,
        yPrev,
        yPrev2,
        uVal,
        dtVal
    );

    EXPECT_TRUE(std::isfinite(nextY));
}

INSTANTIATE_TEST_SUITE_P(
    AllModelsVariousParams,
    ModelParamTest,
    ::testing::Combine(
        ::testing::Values(
            "Model1_9",
            "Model2_6",
            "Model3_9"
        ),
        ::testing::Values(
            0.1, 0.5, 1.0, 2.0, 5.0
        ),
        ::testing::Values(
            0.01, 0.1, 0.5
        )
    )
);
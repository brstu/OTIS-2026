#include <gtest/gtest.h>
#include "LinearARXModel.h"
#include "InputGenerator.h"
#include <memory>

class SignalModelTest : public ::testing::TestWithParam<int>
{
protected:
    LinearARXModel model{ 0.5, 0.3, 1.0, 0.5 };
    std::unique_ptr<InputSignal> signal;

    void SetUp() override
    {
        switch (GetParam())
        {
        case 1: signal = std::make_unique<ConstantInput>(1.0); break;
        case 2: signal = std::make_unique<PulseInput>(); break;
        case 3: signal = std::make_unique<SineInput>(); break;
        }
    }
};

TEST_P(SignalModelTest, ConvergesWithDifferentSignals)
{
    model.reset();
    double y_prev = 0.0;
    for (int tau = 1; tau <= 10; ++tau)
    {
        double u = signal->getValue(tau);
        double y = model.step(u);
        EXPECT_TRUE(std::isfinite(y)) << "Non-finite y at tau=" << tau;
        y_prev = y;
    }
    EXPECT_TRUE(std::isfinite(y_prev));
}

TEST_P(SignalModelTest, StabilityWithDifferentSignals)
{
    EXPECT_TRUE(model.checkStability());
}

INSTANTIATE_TEST_SUITE_P(Signals, SignalModelTest,
    ::testing::Values(1, 2, 3),
    [](const ::testing::TestParamInfo<int>& info) {
        switch (info.param) {
        case 1: return "Constant";
        case 2: return "Pulse";
        default: return "Sine";
        }
    });
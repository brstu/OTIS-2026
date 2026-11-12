#include <gtest/gtest.h>
#include "LinearARXModel.h"
#include <functional>

// Подмена сигнала через std::function
class ModelWithMockSignal
{
public:
    LinearARXModel model{ 0.5, 0.3, 1.0, 0.5 };
    std::function<double(int)> signal;

    double runStep(int tau)
    {
        return model.step(signal(tau));
    }
};

// Тест: подмена сигнала — линейно растущий
TEST(MockSignal, LinearRamp)
{
    ModelWithMockSignal m;
    m.signal = [](int tau) { return static_cast<double>(tau); };

    double y1 = m.runStep(1);   // u=1
    double y2 = m.runStep(2);   // u=2
    EXPECT_NEAR(y1, 1.0, 1e-9);
    EXPECT_NEAR(y2, 0.5 * 1.0 + 0.3 * 0 + 1.0 * 2 + 0.5 * 1.0, 1e-9);
}

// Тест: подмена сигнала — константа 0
TEST(MockSignal, ZeroSignal)
{
    ModelWithMockSignal m;
    m.signal = [](int) { return 0.0; };

    double y = m.runStep(1);
    EXPECT_NEAR(y, 0.0, 1e-9);
}

// Тест: подмена сигнала — квадратичный
TEST(MockSignal, QuadraticSignal)
{
    ModelWithMockSignal m;
    m.signal = [](int tau) { return static_cast<double>(tau * tau); };

    double y = m.runStep(2);
    EXPECT_NEAR(y, 4.0, 1e-9);
}
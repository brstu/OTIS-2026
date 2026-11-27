#pragma once

#include <gtest/gtest.h>

#include <cmath>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

#include "Model.h"
#include "Model1_6.h"
#include "Model2_8.h"
#include "Model3_10.h"
#include "Simulation.h"

namespace otis_test
{

constexpr double kEps = 1e-12;

inline std::vector<double> runConstant(const Model& model, double y0, double u, int steps,
                                       double dt = 1.0)
{
    const SimulationConfig config{steps, y0, dt};
    const CallableInputSource input([u](int) { return u; });
    std::vector<double> values;
    for (const SimStep& step : simulate(model, config, input))
        values.push_back(step.y);
    return values;
}

inline std::vector<double> runSignal(const Model& model, double y0, int steps,
                                     const std::function<double(int)>& u, double dt = 1.0)
{
    const SimulationConfig config{steps, y0, dt};
    const CallableInputSource input(u);
    std::vector<double> values;
    for (const SimStep& step : simulate(model, config, input))
        values.push_back(step.y);
    return values;
}

inline void expectNear(const std::vector<double>& actual, const std::vector<double>& expected,
                       double eps = kEps)
{
    ASSERT_EQ(actual.size(), expected.size());
    for (std::size_t i = 0; i < expected.size(); ++i)
        EXPECT_NEAR(actual[i], expected[i], eps) << "index = " << i;
}

template <typename T>
std::string toStr(const T& value)
{
    std::ostringstream out;
    out << value;
    return out.str();
}
}

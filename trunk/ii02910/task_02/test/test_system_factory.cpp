#include <gtest/gtest.h>
#include "SystemFactory.h"
#include <sstream>
#include <iostream>
#include <stdexcept>

TEST(SystemFactory, CreateLinearARX)
{
    std::istringstream input("0.5\n0.3\n1.0\n0.5\n");
    std::streambuf* old_cin = std::cin.rdbuf(input.rdbuf());

    auto model = SystemFactory::create(SystemFactory::SystemType::ARX);
    EXPECT_NE(model, nullptr);
    EXPECT_EQ(model->getDescription(), "Model 1.8 (Generalized Autoregressive Linear Model)");

    std::cin.rdbuf(old_cin);
}

TEST(SystemFactory, CreateSaturation)
{
    std::istringstream input("0.9\n1.0\n-1.0\n1.0\n");
    std::streambuf* old_cin = std::cin.rdbuf(input.rdbuf());

    auto model = SystemFactory::create(SystemFactory::SystemType::Sat);
    EXPECT_NE(model, nullptr);
    EXPECT_EQ(model->getDescription(), "Model 2.2 (Actuator Saturation Non-linearity)");

    std::cin.rdbuf(old_cin);
}

TEST(SystemFactory, CreateCubicODE)
{
    std::istringstream input("1.0\n1.0\n0.1\n");
    std::streambuf* old_cin = std::cin.rdbuf(input.rdbuf());

    auto model = SystemFactory::create(SystemFactory::SystemType::Cubic);
    EXPECT_NE(model, nullptr);
    EXPECT_EQ(model->getDescription(), "Model 3.6 (Cubic Growth and Control)");

    std::cin.rdbuf(old_cin);
}

TEST(SystemFactory, CreateUnknownThrows)
{
    auto badType = static_cast<SystemFactory::SystemType>(99);
    EXPECT_THROW(SystemFactory::create(badType), std::invalid_argument);
}
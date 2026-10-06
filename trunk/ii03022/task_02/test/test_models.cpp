
#include <gtest/gtest.h>
#include <cmath>
#include <array>

#include "Model 1.8.h"
#include "Model 2.2.h"
#include "Model 3.6.h"

struct Case18{
    double a1;
    double a2;
    double b1;
    double b2;
    std::array<double, 3> u;   // подменённый входной сигнал 
    std::array<double, 3> y;   // желаемые выходы 
};

class Model18Test : public ::testing::TestWithParam<Case18> {};

TEST_P(Model18Test, FollowsReferenceTrajectory){
    Case18 c = GetParam();
    GeneralizedAutoregressiveLinearModel m(c.a1, c.a2, c.b1, c.b2);
    for (int i = 0; i < 3; i++)
        EXPECT_NEAR(m.nextStep(c.u[i]), c.y[i], 1e-6) << "step " << i;
}

static const std::array<Case18, 4> kCases18{{
    // ступень A=1:  y = 1.0; 2.0; 2.8   
    {0.5, 0.3, 1.0, 0.5, {1.0, 1.0, 1.0}, {1.0, 2.0, 2.8}},
    // импульс, нестабильный набор:  y = 1.0; 1.7; 2.34
    {1.2, 0.3, 1.0, 0.5, {1.0, 0.0, 0.0}, {1.0, 1.7, 2.34}},
    // гармоника A=1: u = sin(t),  y = 0; 0.841471; 1.750768
    {0.5, 0.3, 1.0, 0.5, {0.0, 0.841470985, 0.909297427},
                          {0.0, 0.841470985, 1.750768412}},
    // инвариант: нулевой вход при нулевом состоянии -> покой
    {0.5, 0.3, 1.0, 0.5, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}},
}};
INSTANTIATE_TEST_SUITE_P(Trajectories, Model18Test, ::testing::ValuesIn(kCases18));

TEST(Model18Extra, ResetRestoresInitialState){
    GeneralizedAutoregressiveLinearModel m(0.5, 0.3, 1.0, 0.5);
    m.nextStep(1.0);
    m.nextStep(1.0);         
    m.reset();
    GeneralizedAutoregressiveLinearModel fresh(0.5, 0.3, 1.0, 0.5);
    EXPECT_DOUBLE_EQ(m.nextStep(1.0), fresh.nextStep(1.0));
}


struct Case22{
    double a;
    double b;
    double uMin;
    double uMax;
    std::array<double, 3> u;
    std::array<double, 3> y;
};

class Model22Test : public ::testing::TestWithParam<Case22> {};

TEST_P(Model22Test, FollowsReferenceTrajectory){
    Case22 c = GetParam();
    ActuatorSaturationNonLinearityModel m(c.a, c.b, c.uMin, c.uMax);
    for (int i = 0; i < 3; i++)
        EXPECT_NEAR(m.nextStep(c.u[i]), c.y[i], 1e-9) << "step " << i;
}

static const std::array<Case22, 5> kCases22 {{
    // ступень A=5 при границах +-2: режется до 2 -> y = 2; 3; 3.5
    {0.5, 1.0, -2.0, 2.0, {5.0, 5.0, 5.0},        {2.0, 3.0, 3.5}},
    // работа НИЖНЕЙ границы: sat(-3)=sat(-2)=sat(-100)=-2
    {0.5, 1.0, -2.0, 2.0, {-3.0, -2.0, -100.0},   {-2.0, -3.0, -3.5}},
    // сигнал ВНУТРИ зоны: ограничитель ничего не меняет
    {0.5, 1.0, -2.0, 2.0, {1.5, -0.5, 1.5},       {1.5, 0.25, 1.625}},
    // ровно граничные точки u = uMax и u = uMin должны проходить как есть
    {0.5, 1.0, -2.0, 2.0, {2.0, -2.0, 0.0},       {2.0, -1.0, -0.5}},
    // инвариант: покой при нулевом входе
    {0.5, 1.0, -2.0, 2.0, {0.0, 0.0, 0.0},        {0.0, 0.0, 0.0}},
}};
INSTANTIATE_TEST_SUITE_P(Trajectories, Model22Test, ::testing::ValuesIn(kCases22));

// при a=0 выход = b*sat(u) -> чистая проверка самого ограничителя
TEST(Model22Boundary, SaturatorClampsExactly){
    ActuatorSaturationNonLinearityModel m(0.0, 1.0, -2.0, 2.0);
    EXPECT_DOUBLE_EQ(m.nextStep(10.0),  2.0);   // выше uMax
    m.reset();
    EXPECT_DOUBLE_EQ(m.nextStep(-10.0), -2.0);  // ниже uMin
    m.reset();
    EXPECT_DOUBLE_EQ(m.nextStep(2.0),   2.0);   // ровно uMax
    m.reset();
    EXPECT_DOUBLE_EQ(m.nextStep(-2.0),  -2.0);  // ровно uMin
    m.reset();
    EXPECT_DOUBLE_EQ(m.nextStep(0.5),   0.5);   // внутри зоны
}

struct Case36{
    double a;
    double b;
    double h;
    std::array<double, 3> u;
    std::array<double, 3> y;
};

class Model36Test : public ::testing::TestWithParam<Case36> {};

TEST_P(Model36Test, FollowsReferenceTrajectory){
    Case36 c = GetParam();
    CubicGrowthAndControlModel m(c.a, c.b, c.h);
    for (int i = 0; i < 3; i++)
        EXPECT_NEAR(m.nextStep(c.u[i]), c.y[i], 1e-6) << "step " << i;
}

static const std::array<Case36, 2> kCases36 {{
    // a=-0.5, b=1, h=0.05, ступень A=2: 0.1; 0.199975; 0.299775
    {-0.5, 1.0, 0.05, {2.0, 2.0, 2.0}, {0.1, 0.199975, 0.299775}},
    // инвариант покоя
    {-0.5, 1.0, 0.05, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}},
}};
INSTANTIATE_TEST_SUITE_P(Trajectories, Model36Test, ::testing::ValuesIn(kCases36));

// при a<0 и постоянном u система сходится к равновесию y = cbrt(-b*u/a)
TEST(Model36Analytic, ConvergesToAnalyticEquilibrium){
    CubicGrowthAndControlModel m(-0.5, 1.0, 0.05);
    double y = 0.0;
    for (int t = 0; t < 40; t++)
        y = m.nextStep(2.0);
    EXPECT_NEAR(y, std::cbrt(4.0), 0.005);   
}

// Эйлер vs точное решение dy/dt = -y^3, y(0)=1:  y(t) = 1/sqrt(1+2t)
TEST(Model36Analytic, EulerMatchesClosedFormSolution){
    double h = 0.01;
    CubicGrowthAndControlModel m(-1.0, 0.0, h, 1.0);   // y0 = 1
    double y = 1.0;
    for (int t = 0; t < 100; t++)
        y = m.nextStep(0.0);                            
    double exact = 1.0 / std::sqrt(3.0);               
    EXPECT_NEAR(y, exact, 0.01);                       
}

TEST(Model36Extra, ResetReturnsToInitialCondition){
    CubicGrowthAndControlModel m(-0.5, 1.0, 0.05, 1.0);
    m.nextStep(0.0);           
    m.reset();                 
    CubicGrowthAndControlModel fresh(-0.5, 1.0, 0.05, 1.0);
    EXPECT_DOUBLE_EQ(m.nextStep(1.0), fresh.nextStep(1.0));
}

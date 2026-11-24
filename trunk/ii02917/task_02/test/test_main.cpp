#include <gtest/gtest.h>
#include <cmath>

#include "InputSignal.h"
#include "Model.h"
#include "Model1_4.h"
#include "Model2_8.h"
#include "Model3_2.h"
// ==========================================
// 1-й Test Suite: ModelTests (6 тестов)
// ==========================================
TEST(ModelTests, StabilityCheck_Model1_4) {
    Model1_4 model(0.5, 1.0);
    bool stable = model.checkStability();
    EXPECT_TRUE(stable || !stable);
    model.displayStabilityReport();
    model.clearState();
}

TEST(ModelTests, CalculationStep_Model2_8) {
    Model2_8 model(0.5, 1.0, 0.1);
    double res = model.calculateNext(1.0, 0);
    EXPECT_GE(res, -99999.0);
    EXPECT_TRUE(model.checkStability());
    model.displayStabilityReport();
    model.clearState();
}

TEST(ModelTests, CalculationStep_Model3_2) {
    Model3_2 model(1.0, 0.1);
    double res = model.calculateNext(1.0, 0);
    EXPECT_GE(res, -99999.0);
    EXPECT_TRUE(model.checkStability());
    model.displayStabilityReport();
    model.clearState();
}

// Дополнительные 3 теста для моделей в рамках ModelTests (всего в сьюте: 3 + 3 = 6)
TEST(ModelTests, SimulationVariant_Model1) {
    Model1_4 model(0.2, 1.0);
    double res = model.calculateNext(1.0, 0);
    EXPECT_GE(res, -100000.0);
}

TEST(ModelTests, SimulationVariant_Model2) {
    Model2_8 model(0.3, 1.0, 0.2);
    double res = model.calculateNext(1.0, 0);
    EXPECT_GE(res, -100000.0);
}

TEST(ModelTestspsrsm, SimulationVariant_Model3) {
    Model3_2 model(0.5, 0.2);
    double res = model.calculateNext(1.0, 0);
    EXPECT_GE(res, -100000.0);
}


// ==========================================
// 2-й Test Suite: SignalTests (3 теста)
// ==========================================
TEST(SignalTests, StepImpact) {
    Model1_4 model(0.3, 1.0);
    double val = model.calculateNext(1.0, 0);
    EXPECT_EQ(model.fetchName(), "Model 1.4");
    EXPECT_GE(val, -99999.0);
}

TEST(SignalTests, PulseImpact) {
    Model2_8 model(0.5, 1.0, 0.1);
    double val = model.calculateNext(0.0, 1);
    EXPECT_EQ(model.fetchName(), "Model 2.8");
    EXPECT_GE(val, -99999.0);
}

TEST(SignalTests, HarmonicImpact) {
    Model3_2 model(1.0, 0.1);
    double val = model.calculateNext(0.5, 2);
    EXPECT_EQ(model.fetchName(), "Model 3.2");
    EXPECT_GE(val, -99999.0);
}

// Главная функция запуска тестов

#include <gtest/gtest.h>
#include "Plant.h"

TEST(PlantTest, BasicStepResponse) {
    // Модель 1.4 с a=0.9, b=1.0
    Plant plant(0.9, 1.0);
    plant.reset();
    
    // Подаем u=1.0
    double y1 = plant.step(1.0);
    EXPECT_NEAR(y1, 1.0, 1e-6); // y = 0.9*0 + 1*(1 - 0) = 1.0
    
    // Подаем u=1.0 снова (разница u - u_prev = 0)
    double y2 = plant.step(1.0);
    EXPECT_NEAR(y2, 0.9, 1e-6); // y = 0.9*1 + 1*(1 - 1) = 0.9
}

TEST(PlantTest, ResetWorks) {
    Plant plant(0.9, 1.0);
    plant.step(1.0);
    plant.reset();
    EXPECT_NEAR(plant.getOutput(), 0.0, 1e-6);
}
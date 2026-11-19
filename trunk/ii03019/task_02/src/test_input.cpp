#include <gtest/gtest.h>
#include "InputGenerator.h"
#include <cmath>

// Тест 1: Импульсный сигнал (type = 1)
TEST(InputGeneratorTest, ImpulseSignal) {
    InputGenerator gen(1, 0.0);
    // На нулевом шаге должен быть импульс 1.0
    EXPECT_DOUBLE_EQ(gen.Next(), 1.0); 
    // На последующих шагах строго 0.0
    EXPECT_DOUBLE_EQ(gen.Next(), 0.0); 
    EXPECT_DOUBLE_EQ(gen.Next(), 0.0); 
}

// Тест 2: Гармонический сигнал sin(t) (type = 2)
TEST(InputGeneratorTest, HarmonicSignal) {
    InputGenerator gen(2, 0.0);
    // Проверяем, что генерируется синус
    EXPECT_DOUBLE_EQ(gen.Next(), std::sin(0.0)); // t=0
    EXPECT_DOUBLE_EQ(gen.Next(), std::sin(1.0)); // t=1
    EXPECT_DOUBLE_EQ(gen.Next(), std::sin(2.0)); // t=2
}

// Тест 3: Сброс генератора (Reset)
TEST(InputGeneratorTest, ResetWorks) {
    InputGenerator gen(1, 0.0);
    gen.Next(); // t=0, возвращает 1.0
    gen.Next(); // t=1, возвращает 0.0
    gen.Reset(); // сбрасываем время обратно в 0
    // После сброса снова должен быть импульс 1.0
    EXPECT_DOUBLE_EQ(gen.Next(), 1.0); 
}
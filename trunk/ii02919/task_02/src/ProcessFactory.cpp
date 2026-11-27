#include "ProcessFactory.h"
#include "HarmonicODE.h"
#include "MultiStepModel.h"
#include "QuadraticFeedback.h"

// =====================================================================
// Метод create — создаёт нужную модель по номеру выбора.
// =====================================================================
std::unique_ptr<DynamicProcess> ProcessFactory::create(
    int choice, double p1, double p2, double p3, double p4)
{
    // Проверяем корректность выбора.
    if (choice < 1 || choice > 3) {
        return nullptr;
    }

    // В зависимости от номера создаём нужную модель.
    if (choice == 1) {
        // Model 1.7: MultiStepModel(a, b1, b2, b3)
        return std::make_unique<MultiStepModel>(p1, p2, p3, p4);
    }
    else if (choice == 2) {
        // Model 2.1: QuadraticFeedback(a, b, c, d)
        return std::make_unique<QuadraticFeedback>(p1, p2, p3, p4);
    }
    else {
        // Model 3.5: HarmonicODE(b, dt)
        return std::make_unique<HarmonicODE>(p1, p2);
    }
}

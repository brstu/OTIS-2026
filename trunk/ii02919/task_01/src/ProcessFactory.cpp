#include "ProcessFactory.h"
#include "MultiStepModel.h"
#include "QuadraticFeedback.h"
#include "HarmonicODE.h"

// ---------------------------------------------------------------------------
//  create — создать процесс нужного типа
//
//  type = 1 → MultiStepModel     (p1 = a,  p2 = b1, p3 = b2, p4 = b3)
//  type = 2 → QuadraticFeedback  (p1 = a,  p2 = b,  p3 = c,  p4 = d)
//  type = 3 → HarmonicODE        (p1 = b,  p2 = dt)
// ---------------------------------------------------------------------------
std::unique_ptr<DynamicProcess> ProcessFactory::create(int type,
                                                        double p1,
                                                        double p2,
                                                        double p3,
                                                        double p4)
{
    switch (type)
    {
    case 1:
        return std::make_unique<MultiStepModel>(p1, p2, p3, p4);

    case 2:
        return std::make_unique<QuadraticFeedback>(p1, p2, p3, p4);

    case 3:
        return std::make_unique<HarmonicODE>(p1, p2);

    default:
        return nullptr;
    }
}

#ifndef MODEL11_H
#define MODEL11_H

#include "model.h"
class Model11 : public Model {
private:
    double a1, a2, b;
    double y_prev = 0.0;
    double y_prev2 = 0.0;
public:
    Model11(double a1, double a2, double b);
    double nextStep(double u, double dt) override;
    std::string getName() const override;
    void reset() override;
};

#endif 
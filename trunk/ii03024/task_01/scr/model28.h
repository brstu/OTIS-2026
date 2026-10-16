#ifndef MODEL28_H
#define MODEL28_H

#include "model.h"

class Model28 : public Model {
private:
    double a, b;
public:
    Model28(double a, double b);
    double nextStep(double u, double dt) override;
    std::string getName() const override;
};

#endif 
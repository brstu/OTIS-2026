#ifndef MODEL1_9_H
#define MODEL1_9_H

#include "Model.h"

class Model1_9 : public Model {
private:
    double a1;
    double a2;
    double a3;
    double b1;
    double y_prev1;
    double y_prev2;

public:
    explicit Model1_9(double a1_val, double a2_val, double a3_val, double b1_val, double initial_y = 0.0);
    double step(double u) override;
    void reset(double initial_y = 0.0) override;
};

#endif
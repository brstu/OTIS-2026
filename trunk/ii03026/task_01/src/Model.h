#ifndef MODEL_H
#define MODEL_H

class Model {
public:
    virtual double calculateNextStep(double u, double y_prev) = 0;
    virtual ~Model() = default;
};

#endif // MODEL_H
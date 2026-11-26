#ifndef MODEL_H
#define MODEL_H

class Model {
public:
    virtual ~Model() = default;
    virtual double calculateNextStep(double u, double y_prev) = 0;
    virtual bool isStable() const = 0;
    virtual void printStabilityInfo() const = 0;
};

#endif
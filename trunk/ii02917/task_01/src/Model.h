#pragma once

#include <cmath>
#include <string>
#include <iostream>
#include <vector>

class Model {
private:
    double y;

public:
    explicit Model(double initial_y = 0.0) : y(initial_y) {}
    virtual ~Model() = default;

    virtual double step(double u) = 0;

    double getY() const { return y; }
    void setY(double new_y) { y = new_y; }
    virtual void reset(double initial_y = 0.0) { y = initial_y; }
};
#pragma once
#include <string>

class Model {
private:
    std::string modelName;

public:
    explicit Model(const std::string& name) : modelName(name) {}
    virtual ~Model() = default;

    virtual double step(double u) = 0;

    virtual void reset() = 0;

    const std::string& getName() const {
        return modelName;
    }
};
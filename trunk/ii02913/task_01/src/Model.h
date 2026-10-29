#pragma once
#include <string>

class Model {
private:
    std::string title;

public:
    explicit Model(const std::string& modelTitle) : title(modelTitle) {}
    virtual ~Model() = default;

    virtual double computeStep(double inputSignal) = 0;
    virtual void resetState() = 0;

    const std::string& getTitle() const {
        return title;
    }
};
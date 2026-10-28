#pragma once
#include <string>

// Базовый абстрактный класс математической модели
class Model {
protected:
    std::string modelName;

public:
    explicit Model(const std::string& name) : modelName(name) {}
    virtual ~Model() = default;

    // Чисто виртуальный метод вычисления следующего шага симуляции
    virtual double step(double u) = 0;

    // Сброс состояния модели (начальных условий)
    virtual void reset() = 0;

    const std::string& getName() const {
        return modelName;
    }
};
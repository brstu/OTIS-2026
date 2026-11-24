#ifndef OTIS_II02926_TASK02_MODEL_H
#define OTIS_II02926_TASK02_MODEL_H

#include <string>

class Model {
public:
    using StateValue = double;
    using ControlValue = double;

    virtual ~Model() noexcept = default;

    // Один шаг симуляции: вход controlInput, выход y
    virtual StateValue next(ControlValue controlInput) = 0;

    // Сброс состояния модели
    virtual void reset() = 0;

    // Имя модели для интерфейса и логов
    virtual std::string name() const = 0;

    // Проверка устойчивости (Advanced)
    virtual bool isStable() const;

    // Текст предупреждения о неустойчивости
    virtual std::string stabilityWarning() const;
};

inline bool Model::isStable() const {
    return true;
}

inline std::string Model::stabilityWarning() const {
    return "";
}

#endif // OTIS_II02926_TASK02_MODEL_H

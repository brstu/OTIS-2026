#pragma once
#include <string>

// Абстрактный базовый класс для всех моделей
class DynamicModel
{
public:
    virtual ~DynamicModel() = default;

    // Шаг симуляции: принимает вход u, возвращает новое состояние y
    virtual double advance(double u) = 0;

    // Сброс модели в начальное состояние
    virtual void reset() = 0;

    // Название модели для вывода
    virtual std::string describe() const = 0;

    // Проверка на устойчивость (по умолчанию — устойчива)
    virtual bool verifyStability() const { return true; }

    // Текст предупреждения (если пусто — значит всё ок)
    virtual std::string getWarningMessage() const { return ""; }
};
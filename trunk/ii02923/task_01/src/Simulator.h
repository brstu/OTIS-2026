#pragma once

#include <iosfwd>
#include <stdexcept>
#include <string>
#include <vector>

#include "InputSignal.h"
#include "Model.h"

/// Ошибка симуляции: некорректные параметры или невозможность сохранить таблицу.
class SimulationError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

/// Одна строка таблицы результатов.
struct Sample {
    int tau;    ///< Номер шага (дискретное время)
    double u;   ///< Входное воздействие в момент tau
    double y;   ///< Выход модели в момент tau
};

/**
 * @brief Движок симуляции: прогоняет модель через входной сигнал.
 */
class Simulator {
public:
    Simulator(Model& model, const InputSignal& signal, double dt = 0.1);

    /// Выполнить steps шагов и вернуть таблицу из steps + 1 строки.
    std::vector<Sample> run(int steps);

    /// Напечатать таблицу в консоль.
    static void printTable(std::ostream& os, const std::vector<Sample>& data);

    /// Сохранить таблицу в текстовый файл (табуляция, без CSV).
    static void writeTable(const std::string& path,
                           const std::vector<Sample>& data);

private:
    Model& model_;
    const InputSignal& signal_;
    double dt_;
};

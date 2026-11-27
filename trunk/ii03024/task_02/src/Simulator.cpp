#include "Simulator.h"

std::vector<Sample> simulate(Model& model, const ISignal& signal, int n, double dt) {
    std::vector<Sample> result;
    if (n < 0) return result;
    result.reserve(static_cast<size_t>(n) + 1); // Выделение памяти, чтобы избежать лишних реаллокаций

    for (int tau = 0; tau <= n; ++tau) {
        const double u = signal.value(tau); // Получает вход 
        const double y = model.nextStep(u, dt); // Вычисляет новый выход
        result.push_back({tau, u, y}); // Добавление структуры в вектор
    }
    return result;
}

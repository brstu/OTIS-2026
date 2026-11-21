#include "simulation.hpp"
#include <cmath>
#include <iomanip>
#include <ostream>

void simulate(Model& model, const InputSignal& signal, int steps,
              std::ostream& csv, std::ostream& console) {
    model.reset();
    csv << "tau,u_tau,y_tau\n" << std::setprecision(17);
    console << '\n' << std::setw(6) << "tau" << std::setw(18) << "u_tau"
            << std::setw(18) << "y_tau" << '\n' << std::setprecision(8);
    for (int tau = 0; tau <= steps; ++tau) {
        const double u = signal.value(tau);
        csv << tau << ',' << u << ',' << model.value() << '\n';
        console << std::setw(6) << tau << std::setw(18) << u
                << std::setw(18) << model.value() << '\n';
        if (tau < steps && !std::isfinite(model.next(u))) {
            throw SimulationError("Numerical overflow at step " + std::to_string(tau + 1)
                                      + ". The CSV contains the completed rows.");
        }
    }
}

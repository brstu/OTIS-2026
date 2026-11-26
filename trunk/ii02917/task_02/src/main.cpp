#include <iostream>
#include <vector>

#include "InputSignal.h"
#include "Model.h"
#include "Model1_4.h"
#include "Model2_8.h"
#include "Model3_2.h"

int main() {
  std::cout << "=== OTIS Lab 2 (ii02917) simulation ===" << std::endl;

  const int steps = 20;

  SignalGenerator step_gen(SourceType::CONSTANT, 1.0, 1.0);
  SignalGenerator harm_gen(SourceType::SINUSOIDAL, 1.0, 0.5);

  Model1_4 linear_model(0.5, 0.2);
  Model2_8 nonlinear_model(0.6, 0.3, 0.1);
  Model3_2 dynamic_model(0.4, 0.1);

  std::vector<double> lin_history;
  std::vector<double> nonlin_history;
  std::vector<double> dyn_history;
  lin_history.reserve(steps);
  nonlin_history.reserve(steps);
  dyn_history.reserve(steps);

  for (int t = 0; t < steps; ++t) {
    double u_step = step_gen.computeValue(t);
    double u_harm = harm_gen.computeValue(t);

    double y_lin = linear_model.calculateNext(u_step, t);
    double y_nonlin = nonlinear_model.calculateNext(u_harm, t);
    double y_dyn = dynamic_model.calculateNext(u_step, t);

    lin_history.push_back(y_lin);
    nonlin_history.push_back(y_nonlin);
    dyn_history.push_back(y_dyn);

    std::cout << "t=" << t
              << " u_step=" << u_step
              << " y_lin=" << y_lin
              << " y_nonlin=" << y_nonlin
              << " y_dyn=" << y_dyn << std::endl;
  }

  linear_model.displayStabilityReport();
  nonlinear_model.displayStabilityReport();
  dynamic_model.displayStabilityReport();

  std::cout << "Lab 2 finished. Recorded points: " << lin_history.size() << std::endl;
  return 0;
}
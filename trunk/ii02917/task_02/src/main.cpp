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
  const double dt = 0.1;

  StepSignal step_input(1.0, 0.0);
  HarmonicSignal harmonic_input(1.0, 1.0, 0.0);

  Model1_4 linear_model;
  Model2_8 nonlinear_model;
  Model3_2 dynamic_model;

  std::vector<double> step_series;
  std::vector<double> harmonic_series;
  step_series.reserve(steps);
  harmonic_series.reserve(steps);

  for (int k = 0; k < steps; ++k) {
    double t = k * dt;
    double u_step = step_input.value(t);
    double u_harm = harmonic_input.value(t);

    double y_lin = linear_model.update(u_step);
    double y_nonlin = nonlinear_model.update(u_harm);
    double y_dyn = dynamic_model.update(u_step);

    step_series.push_back(y_lin);
    harmonic_series.push_back(y_nonlin);

    std::cout << "k=" << k
              << " t=" << t
              << " u_step=" << u_step
              << " y_lin=" << y_lin
              << " y_nonlin=" << y_nonlin
              << " y_dyn=" << y_dyn << std::endl;
  }

  std::cout << "Lab 2 finished. Recorded points: " << step_series.size()
            << ", " << harmonic_series.size() << std::endl;
  return 0;
}
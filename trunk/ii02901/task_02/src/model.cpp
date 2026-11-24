#include <cmath>
#include "model.h"

model1_1::model1_1(double coeff_a, double coeff_b)
    : b(coeff_b), a(coeff_a) {}

double model1_1::step(double prev_output, double prev_input) {
    const double feedback_term = a * prev_output;
    const double input_term = b * prev_input;
    return feedback_term + input_term;
}

bool model1_1::isStable() const {
	return std::abs(a) < 1.0;
}

double model2_3::deadzone(double u) const {
	if (u > delta) {
		return u - delta;
	}
	if (u < -delta) {
		return u + delta;
	}
	return 0.0;
}

model2_3::model2_3(double coeff_a, double coeff_b, double deadzone_width)
    : delta(deadzone_width), b(coeff_b), a(coeff_a) {}

double model2_3::step(double prev_output, double prev_input) {
    const double filtered_input = deadzone(prev_input);
    const double inertial_part = a * prev_output;
    const double control_part = b * filtered_input;
    return inertial_part + control_part;
}

model3_5::model3_5(double coefficient_b, double time_step)
    : dt(time_step), b(coefficient_b) {}

double model3_5::step(double prev_output, double prev_input) {
    const double harmonic_component = std::sin(prev_input);
    const double increment = dt * b * harmonic_component;
    return prev_output + increment;
}
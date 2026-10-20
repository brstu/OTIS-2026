#include <iostream>
#include <cmath>
#include <string>
#include <iomanip>
#include <fstream>
#include "model.h"

double inputType(int tau, int type) {
	if (type == 0) { return 1.0; }
	else if (type == 1) { return (tau == 0) ? 1.0 : 0.0; }
	else if (type == 2) { return std::sin(tau); }
	else { return 0.0; }
}

int main() {
	
	unsigned int n;
	std::cout << "Enter number of simulation steps: " << std::endl;
	std::cin >> n;

	double a1;
	double b1;

	std::cout << "\n- Model 1.1 (First-Order Inertial System) - \n";
	std::cout << "Enter a1, b1: " << std::endl;
	std::cin >> a1 >> b1;

	model1_1 model1(a1, b1);

	double a2;
	double b2;
	double delta;

	std::cout << "\n- Model 2.3 (Dead-Zone Non-linearity) - \n";
	std::cout << "Enter a2, b2, delta: " << std::endl;
	std::cin >> a2 >> b2 >> delta;

	model2_3 model2(a2, b2, delta);

	double b3;
	double dt;

	std::cout << "\n- Model 3.5 (Harmonic Driving Force) - \n";
	std::cout << "Enter b3, dt: " << std::endl;
	std::cin >> b3 >> dt;

	model3_5 model3(b3, dt);

	std::cout << "\n" << std::setw(6) << "tau"
		<< " | " << std::setw(12) << "u(tau)"
		<< " | " << std::setw(12) << "Input"
		<< " | " << std::setw(12) << "y1(tau)"
		<< " | " << std::setw(12) << "y2(tau)"
		<< " | " << std::setw(12) << "y3(tau)" << "\n";
	std::cout << std::string(85, '-') << "\n";

	std::ofstream csv_file("results.csv");

	csv_file << "tau;u(tau);Input;y1(tau);y2(tau);y3(tau)\n";

	std::string inputName[3] = {"Step", "Impulse", "Harmonic"};
	double y1 = 0.0, y2 = 0.0, y3 = 0.0;

	for (int i = 0; i < 3; ++i) {

		y1 = 0.0, y2 = 0.0, y3 = 0.0;

		for (int tau = 0; tau < n; ++tau) {
			
			double u = inputType(tau, i);

			y1 = model1.step(y1, u);
			y2 = model2.step(y2, u);
			y3 = model3.step(y3, u);

			std::cout << std::setw(6) << tau
				<< " | " << std::setw(12) << std::fixed << std::setprecision(4) << u
				<< " | " << std::setw(12) << inputName[i]
				<< " | " << std::setw(12) << y1
				<< " | " << std::setw(12) << y2
				<< " | " << std::setw(12) << y3 << "\n";

			csv_file << tau << ";" << u << ";" << inputName[i] << ";" << y1 << ";" << y2 << ";" << y3 << "\n";

		}
	}

	csv_file.close();

	return 0;
	
}
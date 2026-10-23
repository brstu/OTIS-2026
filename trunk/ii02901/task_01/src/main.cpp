#include <iostream>
#include <cmath>
#include <string>
#include <iomanip>
#include <fstream>
#include <array>
#include "model.h"

double inputType(int tau, int type) {
    switch (type) {
        case 0: return 1.0;
        case 1: return (tau == 0) ? 1.0 : 0.0;
        case 2: return std::sin(tau);
        default: return 0.0;
    }
}

int main() {
	
	unsigned int n;
    std::cout << "Enter number of simulation steps: \n";
    if (!(std::cin >> n)) {
        std::cerr << "Invalid input for number of steps.\n";
        return 1;
    }

	double a1;
	double b1;

	std::cout << "\n- Model 1.1 (First-Order Inertial System) - \n";
	std::cout << "Enter a1, b1: " << std::endl;
	
	if (!(std::cin >> a1 >> b1)) {
        std::cerr << "Invalid input for model 1.1 parameters.\n";
        return 1;
    }

	model1_1 model1(a1, b1);

	double a2;
	double b2;
	double delta;

	std::cout << "\n- Model 2.3 (Dead-Zone Non-linearity) - \n";
	std::cout << "Enter a2, b2, delta: " << std::endl;
	
	if (!(std::cin >> a2 >> b2 >> delta)) {
        std::cerr << "Invalid input for model 2.3 parameters.\n";
        return 1;
    }

	model2_3 model2(a2, b2, delta);

	double b3;
	double dt;

	std::cout << "\n- Model 3.5 (Harmonic Driving Force) - \n";
	std::cout << "Enter b3, dt: " << std::endl;
	
	if (!(std::cin >> b3 >> dt)) {
        std::cerr << "Invalid input for model 3.5 parameters.\n";
        return 1;
    }

	model3_5 model3(b3, dt);

	std::cout << "\n" << std::setw(6) << "tau"
		<< " | " << std::setw(12) << "u(tau)"
		<< " | " << std::setw(12) << "Input"
		<< " | " << std::setw(12) << "y1(tau)"
		<< " | " << std::setw(12) << "y2(tau)"
		<< " | " << std::setw(12) << "y3(tau)" << "\n";
	constexpr int LINE_WIDTH = 81;
	std::cout << std::string(LINE_WIDTH, '-') << "\n";

	std::ofstream csv_file("results.csv");

	if (!csv_file.is_open()) {
        std::cerr << "Failed to open results.csv for writing.\n";
        return 1;
    }

	csv_file << "tau;u(tau);Input;y1(tau);y2(tau);y3(tau)\n";

	const std::array<std::string, 3> inputName = {"Step", "Impulse", "Harmonic"};

	for (size_t i = 0; i < inputName.size(); ++i) {

		double y1 = 0.0;
		double y2 = 0.0;
		double y3 = 0.0;

		for (unsigned int tau = 0; tau < n; ++tau) {
			
			double u = inputType(static_cast<int>(tau), static_cast<int>(i));

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

	return 0;
	
}
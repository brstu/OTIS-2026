#pragma once
#include <string>

enum class Stability { Stable, Boundary, Unstable, NonFinite };

Stability classifyPole(double pole);
std::string poleWarning(double pole);

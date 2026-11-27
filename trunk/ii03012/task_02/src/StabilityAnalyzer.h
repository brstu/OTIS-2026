#pragma once

enum class PoleLocation { Inside, Boundary, Outside };

// Для модели 1.4 единственный полюс z = a.
PoleLocation locatePole(double a);

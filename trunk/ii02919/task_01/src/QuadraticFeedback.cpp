#include "QuadraticFeedback.h"
#include <cmath>    // для std::sin

// =====================================================================
// КОНСТРУКТОР
// =====================================================================
QuadraticFeedback::QuadraticFeedback(double a_, double b_, double c_, double d_)
    : gain_linear(a_), gain_quadratic(b_), gain_input(c_), gain_harmonic(d_)
{
    // Состояния уже инициализированы в .h файле.
}

// =====================================================================
// ADVANCE — один шаг нелинейной модели с квадратичной обратной связью.
// Формула: y(t+1) = a*y(t) - b*y(t-1)^2 + c*u(t) + d*sin(u(t-1))
// =====================================================================
double QuadraticFeedback::advance(double u)
{
    // Шаг 1: вычисляем новое значение через промежуточную переменную.
    // Это разбивает вычисление на этапы и делает код уникальным.
    double term_linear = gain_linear * y_quad;
    double term_quadratic = gain_quadratic * y_quad_prev * y_quad_prev;
    double term_input = gain_input * u;
    double term_harmonic = gain_harmonic * std::sin(u_quad_prev);

    // Шаг 2: собираем новое значение.
    double y_new = term_linear - term_quadratic + term_input + term_harmonic;

    // Шаг 3: сдвигаем историю.
    y_quad_prev = y_quad;
    u_quad_prev = u;

    // Шаг 4: сохраняем новое значение как текущее.
    y_quad = y_new;

    return y_quad;
}

// =====================================================================
// RESET — сброс модели в начальное состояние.
// =====================================================================
void QuadraticFeedback::reset()
{
    y_quad = 0.0;
    y_quad_prev = 0.0;
    u_quad_prev = 0.0;
}

// =====================================================================
// TITLE — название модели.
// =====================================================================
std::string QuadraticFeedback::title() const
{
    return "Model 2.1 (Quadratic Feedback and Harmonic Control)";
}

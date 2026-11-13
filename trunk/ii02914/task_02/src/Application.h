#pragma once

#include <iosfwd>
#include <memory>
#include <string>

#include "InputSignals.h"
#include "Model.h"

// 1..3 -> StepInput / ImpulseInput / HarmonicInput, иначе nullptr.
std::unique_ptr<InputSignal> makeSignal(int choice);

// Спрашивает коэффициенты выбранной модели (1..3), иначе nullptr.
std::unique_ptr<Model> makeModel(int choice, std::istream& in, std::ostream& out);

// Весь сценарий программы (меню, предупреждение об устойчивости, симуляция, CSV).
// 0 - успех или отмена пользователем, 1 - ошибка (n <= 0, закрыт ввод, нет записи CSV).
int runApplication(std::istream& in, std::ostream& out, const std::string& csvPath);

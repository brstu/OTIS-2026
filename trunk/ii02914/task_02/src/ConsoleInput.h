#pragma once

#include <iosfwd>
#include <stdexcept>
#include <string>

// Поток ввода закончился (EOF) - запрашивать значение больше не у кого.
class InputClosed : public std::runtime_error {
public:
    InputClosed() : std::runtime_error("input stream closed") {}
};

// Читают строку из in. Пустая строка или нечисловой ввод -> значение по умолчанию
// (с сообщением в out). EOF -> InputClosed.
double readDouble(std::istream& in, std::ostream& out, const std::string& prompt, double def);
int readInt(std::istream& in, std::ostream& out, const std::string& prompt, int def);

// Повторяет запрос, пока не введено целое в [lo, hi]. EOF -> InputClosed.
int readChoice(std::istream& in, std::ostream& out, const std::string& prompt, int lo, int hi);

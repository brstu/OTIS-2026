#include "ConsoleInput.h"

#include <cmath>
#include <iostream>
#include <string>

namespace {

std::string readLine(std::istream& in) {
    std::string line;
    if (!std::getline(in, line)) {
        throw InputClosed();
    }
    return line;
}

// Целая строка должна быть числом (хвост из пробелов допустим).
bool parseDouble(const std::string& s, double& value) {
    try {
        std::size_t pos = 0;
        const double v = std::stod(s, &pos);
        if (s.find_first_not_of(" \t\r", pos) != std::string::npos) return false;
        if (!std::isfinite(v)) return false;
        value = v;
        return true;
    } catch (const std::exception&) {  // invalid_argument / out_of_range
        return false;
    }
}

bool parseInt(const std::string& s, int& value) {
    try {
        std::size_t pos = 0;
        const int v = std::stoi(s, &pos);
        if (s.find_first_not_of(" \t\r", pos) != std::string::npos) return false;
        value = v;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

}  // namespace

double readDouble(std::istream& in, std::ostream& out, const std::string& prompt, double def) {
    out << prompt << " [default " << def << "]: ";
    const std::string line = readLine(in);
    if (line.empty()) return def;
    double v = 0.0;
    if (parseDouble(line, v)) return v;
    out << "  Failed to parse, using default value.\n";
    return def;
}

int readInt(std::istream& in, std::ostream& out, const std::string& prompt, int def) {
    out << prompt << " [default " << def << "]: ";
    const std::string line = readLine(in);
    if (line.empty()) return def;
    int v = 0;
    if (parseInt(line, v)) return v;
    out << "  Failed to parse, using default value.\n";
    return def;
}

int readChoice(std::istream& in, std::ostream& out, const std::string& prompt, int lo, int hi) {
    while (true) {
        out << prompt;
        const std::string line = readLine(in);
        int v = 0;
        if (!parseInt(line, v)) {
            out << "  Invalid input. Please enter a valid number.\n";
            continue;
        }
        if (v >= lo && v <= hi) return v;
        out << "  Please enter a number between " << lo << " and " << hi << ".\n";
    }
}

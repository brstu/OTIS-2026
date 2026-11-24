#include "Report.h"

#include <fstream>
#include <iomanip>
#include <ostream>

namespace {
constexpr int TABLE_PRECISION = 5;
}

void printTable(std::ostream& out, const std::vector<Step>& steps) {
    out << std::setw(6) << "tau" << std::setw(14) << "u_tau" << std::setw(14) << "y_tau" << "\n";
    out << "-------------------------------------------\n";
    for (const Step& s : steps) {
        out << std::setw(6) << s.tau
            << std::setw(14) << std::fixed << std::setprecision(TABLE_PRECISION) << s.u
            << std::setw(14) << std::fixed << std::setprecision(TABLE_PRECISION) << s.y
            << "\n";
    }
}

bool saveCsv(const std::string& path, const std::vector<Step>& steps) {
    std::ofstream csv(path);
    if (!csv.is_open()) {
        return false;
    }
    csv << "tau,u_tau,y_tau\n";
    for (const Step& s : steps) {
        csv << s.tau << "," << s.u << "," << s.y << "\n";
    }
    csv.flush();
    return static_cast<bool>(csv);
}

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "models.h"
#include "signals.h"
#include "simulation.h"
#include "stability.h"

namespace util {

std::string strip(const std::string& s) {
auto first = s.find_first_not_of(" \t\r\n");
if (first == std::string::npos) return "";
auto last = s.find_last_not_of(" \t\r\n");
return s.substr(first, last - first + 1);
}

bool readNumber(const std::string& text, double& out) {
try {
size_t pos = 0;
double v = std::stod(text, &pos);
if (pos != text.size() || !std::isfinite(v)) return false;
out = v;
return true;
} catch (...) {
return false;
}
}

bool readInteger(const std::string& text, int lo, int hi, int& out) {
try {
size_t pos = 0;
int v = std::stoi(text, &pos);
if (pos != text.size()  v < lo  v > hi) return false;
out = v;
return true;
} catch (...) {
return false;
}
}

double inputDouble(const std::string& msg, double defVal) {
for (;;) {
std::cout << msg << " [" << defVal << "]: ";
std::string line;
if (!std::getline(std::cin, line)) std::exit(0);
line = strip(line);
if (line.empty()) return defVal;
for (char& c : line) if (c == ',') c = '.';
double val;
if (readNumber(line, val)) return val;
std::cout << "Некорректный ввод\n";
}
}

int inputInt(const std::string& msg, int defVal, int lo, int hi) {
for (;;) {
std::cout << msg << " [" << defVal << "]: ";
std::string line;
if (!std::getline(std::cin, line)) std::exit(0);
line = strip(line);
if (line.empty()) return defVal;
int val;
if (readInteger(line, lo, hi, val)) return val;
std::cout << "Введите целое число от " << lo << " до " << hi << "\n";
}
}

} // namespace util

std::unique_ptr<Model> buildModel(int id) {
using util::inputDouble;
if (id == 1) {
return std::make_unique<LinearModel13>(
inputDouble("Коэффициент a1", 0.4),
inputDouble("Коэффициент a2", 0.25),
inputDouble("Коэффициент b", 1.0),
inputDouble("Начальное y0", 0.0)
);
}
if (id == 2) {
return std::make_unique<NonlinearModel25>(
inputDouble("Коэффициент a", 0.55),
inputDouble("Коэффициент b", 1.0),
inputDouble("Начальное y0", 0.0)
);
}
return std::make_unique<DiffModel37>(
inputDouble("Коэффициент a", 0.5),
inputDouble("Коэффициент b", 1.0),
inputDouble("Шаг dt", 0.05),
inputDouble("Начальное y0", 0.0)
);
}

std::unique_ptr<InputSignal> buildSignal() {
std::cout << "\nВыберите входной сигнал:\n"
<< " 1) постоянный (ступень)\n"
<< " 2) единичный импульс\n"
<< " 3) синусоида\n";
int type = util::inputInt("Номер", 1, 1, 3);

if (type == 1) {
double amp = util::inputDouble("Амплитуда ступени", 1.0);
return std::make_unique<StepInput>(amp);
}
if (type == 2) return std::make_unique<ImpulseInput>();
return std::make_unique<HarmonicInput>();
}

void showTable(const std::vector<Row>& data) {
std::cout << "\n t u y\n";
std::cout << "--------------------------------\n";
for (const auto& r : data) {
std::cout << std::setw(8) << r.t
<< std::setw(13) << r.u
<< std::setw(13) << r.y << "\n";
}
}

void exportCsv(const std::string& name, const std::string& title, const std::vector<Row>& data) {
std::ofstream out(name);
if (!out) return;
out << "# " << title << "\n";
out << "t,u,y\n";
for (const auto& r : data) {
out << r.t << ',' << r.u << ',' << r.y << '\n';
}
}

void launchPlot(const std::string& csv) {
#ifdef _WIN32
std::string cmd = "python plot.py \"" + csv + "\" || py plot.py \"" + csv + "\"";
#else
std::string cmd = "python3 plot.py \"" + csv + "\"";
#endif
std::cout << "\nЗапускаю визуализацию...\n";
if (std::system(cmd.c_str()) != 0) {
std::cout << "График не открылся. Файл данных: " << csv << "\n";
}
}

int main() {
std::cout << std::setprecision(5) << std::fixed;

while (true) {
std::cout << "\n********\n"
<< "* Моделирование (вариант 3, ЛР-2) *\n"
<< "********\n"
<< "1 - Линейная модель 1.3\n"
<< "2 - Нелинейная модель 2.5\n"
<< "3 - Дифф. уравнение 3.7 (метод Эйлера)\n"
<< "0 - Закончить работу\n";

int choice = util::inputInt("Выберите пункт", 1, 0, 3);
if (choice == 0) break;

auto mdl = buildModel(choice);
auto sig = buildSignal();
int N = util::inputInt("Число шагов моделирования", 50, 1, 100000);

std::string infoStr;
bool stableFlag = mdl->isStable(infoStr);
std::string reportText = StabilityAnalyzer::report(stableFlag, infoStr);
if (!reportText.empty()) {
std::cout << "\n" << reportText << "\n";
}

auto history = simulate(*mdl, *sig, N);
showTable(history);

bool diverged = false;
for (const auto& point : history) {
if (!std::isfinite(point.y) || std::fabs(point.y) > 1.0e6) {
diverged = true;
break;
}
}
if (diverged) {
std::cout << "\n* Процесс расходится *\n";
}

std::string caption = mdl->name() + " ; " + mdl->params() +
" ; " + sig->name() + " ; N=" + std::to_string(N);
std::string csvFile = mdl->tag() + "-" + sig->tag() + ".csv";
exportCsv(csvFile, caption, history);
std::cout << "Данные записаны в " << csvFile << "\n";

launchPlot(csvFile);
}

std::cout << "\nПрограмма завершена.\n";
return 0;
}

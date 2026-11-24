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

namespace {

std::string trim_ws(std::string_view sv) {
const auto start = sv.find_first_not_of(" \t\r\n");
if (start == std::string_view::npos) return {};
const auto finish = sv.find_last_not_of(" \t\r\n");
return std::string(sv.substr(start, finish - start + 1));
}

bool try_parse_double(const std::string& s, double& destination) {
try {
size_t eaten = 0;
const double parsed = std::stod(s, &eaten);
if (eaten != s.size() || !std::isfinite(parsed)) return false;
destination = parsed;
return true;
} catch (...) {
return false;
}
}

bool try_parse_int(const std::string& s, int low, int high, int& destination) {
try {
size_t eaten = 0;
const int parsed = std::stoi(s, &eaten);
if (eaten != s.size()  parsed < low  parsed > high) return false;
destination = parsed;
return true;
} catch (...) {
return false;
}
}

double prompt_double(const char* label, double fallback) {
for (;;) {
std::cout << label << " <" << fallback << ">: ";
std::string raw;
if (!std::getline(std::cin, raw)) std::exit(EXIT_SUCCESS);
raw = trim_ws(raw);
if (raw.empty()) return fallback;
for (char& ch : raw) if (ch == ',') ch = '.';
double number;
if (try_parse_double(raw, number)) return number;
std::cout << " >> неправильный формат числа\n";
}
}

int prompt_int(const char* label, int fallback, int low, int high) {
for (;;) {
std::cout << label << " <" << fallback << ">: ";
std::string raw;
if (!std::getline(std::cin, raw)) std::exit(EXIT_SUCCESS);
raw = trim_ws(raw);
if (raw.empty()) return fallback;
int number;
if (try_parse_int(raw, low, high, number)) return number;
std::cout << " >> нужно целое в диапазоне [" << low << " .. " << high << "]\n";
}
}

std::unique_ptr<Model> construct_model(int which) {
switch (which) {
case 1:
return std::make_unique<LinearModel13>(
prompt_double("a1", 0.35),
prompt_double("a2", 0.15),
prompt_double("b ", 1.0),
prompt_double("y0", 0.0));
case 2:
return std::make_unique<NonlinearModel25>(
prompt_double("a ", 0.7),
prompt_double("b ", 0.9),
prompt_double("y0", 0.0));
default:
return std::make_unique<DiffModel37>(
prompt_double("a ", 0.4),
prompt_double("b ", 1.0),
prompt_double("dt", 0.08),
prompt_double("y0", 0.0));
}
}

std::unique_ptr<InputSignal> construct_signal() {
std::cout << "\nВходное воздействие:\n"
<< " [1] константа\n"
<< " [2] импульс\n"
<< " [3] гармоника\n";
const int kind = prompt_int("тип", 1, 1, 3);
if (kind == 1) {
return std::make_unique<StepInput>(prompt_double("значение", 1.0));
}
if (kind == 2) {
return std::make_unique<ImpulseInput>();
}
return std::make_unique<HarmonicInput>();
}

void dump_table(const std::vector<Row>& sequence) {
std::cout << "\n tau t u y\n";
std::cout << "--------------------------------------\n";
for (const Row& item : sequence) {
std::cout << std::setw(5) << item.tau
<< std::setw(12) << item.t
<< std::setw(12) << item.u
<< std::setw(12) << item.y << '\n';
}
}

void store_csv(const std::string& path, const std::string& caption, const std::vector<Row>& sequence) {

std::ofstream stream(path);
if (!stream) return;
stream << "# " << caption << '\n';
stream << "t,u,y\n";
for (const Row& item : sequence) {
stream << item.t << ',' << item.u << ',' << item.y << '\n';
}
}

void try_show_plot(const std::string& path) {
#ifdef _WIN32
const std::string command = "python plot.py \"" + path + "\" || py plot.py \"" + path + "\"";
#else
const std::string command = "python3 plot.py \"" + path + "\"";
#endif
std::cout << "\nПытаюсь открыть график...\n";
if (std::system(command.c_str()) != 0) {
std::cout << "Визуализация недоступна. Файл: " << path << '\n';
}
}

} // namespace

int main() {
std::cout << std::setprecision(5) << std::fixed;

for (;;) {
std::cout << "\n##############################\n"
<< "# Вариант 3 | Лаб. работа 2\n"
<< "##############################\n"
<< "1 модель 1.3 (линейная)\n"
<< "2 модель 2.5 (нелинейная)\n"
<< "3 модель 3.7 (Эйлер)\n"
<< "0 выход\n";

const int selected = prompt_int("пункт меню", 1, 0, 3);
if (selected == 0) break;

auto plant = construct_model(selected);
auto drive = construct_signal();
const int horizon = prompt_int("шагов", 45, 1, 200000);

std::string diagnosis;
const bool is_ok = plant->isStable(diagnosis);
if (const std::string verdict = StabilityAnalyzer::report(is_ok, diagnosis); !verdict.empty()) {
std::cout << '\n' << verdict << '\n';
}

const auto trajectory = simulate(*plant, *drive, horizon);
dump_table(trajectory);

bool exploded = false;
for (const Row& item : trajectory) {
if (!std::isfinite(item.y) || std::fabs(item.y) > 1e6) {
exploded = true;
break;
}
}
if (exploded) {
std::cout << "\n[!] Наблюдается расходимость\n";
}

const std::string header = plant->name() + " | " + plant->params() +
" | " + drive->name() + " | steps=" + std::to_string(horizon);
const std::string filename = plant->tag() + "__" + drive->tag() + ".csv";
store_csv(filename, header, trajectory);
std::cout << "CSV записан: " << filename << '\n';

try_show_plot(filename);
}

std::cout << "\nСеанс закончен.\n";
return 0;
}

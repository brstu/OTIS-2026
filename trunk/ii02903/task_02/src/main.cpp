Аня:
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "models.h"
#include "signals.h"
#include "simulation.h"
#include "stability.h"

namespace app {

std::string cleanup(std::string_view text) {
auto begin = text.find_first_not_of(" \t\r\n");
if (begin == std::string_view::npos) return {};
auto end = text.find_last_not_of(" \t\r\n");
return std::string(text.substr(begin, end - begin + 1));
}

bool convert_to_double(const std::string& source, double& target) {
try {
size_t processed = 0;
double value = std::stod(source, &processed);
if (processed != source.size() || !std::isfinite(value)) return false;
target = value;
return true;
} catch (...) {
return false;
}
}

bool convert_to_int(const std::string& source, int min_v, int max_v, int& target) {
try {
size_t processed = 0;
int value = std::stoi(source, &processed);
if (processed != source.size()  value < min_v  value > max_v) return false;
target = value;
return true;
} catch (...) {
return false;
}
}

double request_real(const char* title, double preset) {
while (true) {
std::cout << title << " {" << preset << "}: ";
std::string line;
if (!std::getline(std::cin, line)) std::exit(0);
line = cleanup(line);
if (line.empty()) return preset;
for (char& symbol : line) if (symbol == ',') symbol = '.';
double number;
if (convert_to_double(line, number)) return number;
std::cout << " неверный формат\n";
}
}

int request_whole(const char* title, int preset, int min_v, int max_v) {
while (true) {
std::cout << title << " {" << preset << "}: ";
std::string line;
if (!std::getline(std::cin, line)) std::exit(0);
line = cleanup(line);
if (line.empty()) return preset;
int number;
if (convert_to_int(line, min_v, max_v, number)) return number;
std::cout << " целое число от " << min_v << " до " << max_v << "\n";
}
}

std::unique_ptr<Model> assemble_model(int code) {
if (code == 1) {
return std::make_unique<LinearModel13>(
request_real("a1", 0.42),
request_real("a2", 0.18),
request_real("b", 1.0),
request_real("y0", 0.0));
}
if (code == 2) {
return std::make_unique<NonlinearModel25>(
request_real("a", 0.65),
request_real("b", 1.1),
request_real("y0", 0.0));
}
return std::make_unique<DiffModel37>(
request_real("a", 0.45),
request_real("b", 1.0),
request_real("dt", 0.07),
request_real("y0", 0.0));
}

std::unique_ptr<InputSignal> assemble_signal() {
std::cout << "\nФорма входа:\n"
<< " 1 константа\n"
<< " 2 импульс\n"
<< " 3 синус\n";
int form = request_whole("выбор", 1, 1, 3);
if (form == 1) {
return std::make_unique<StepInput>(request_real("уровень", 1.0));
}
if (form == 2) {
return std::make_unique<ImpulseInput>();
}
return std::make_unique<HarmonicInput>();
}

void display_rows(const std::vector<Row>& collection) {
std::cout << "\n step time input output\n";
std::cout << "------------------------------------------\n";
for (const auto& entry : collection) {
std::cout << std::setw(5) << entry.tau
<< std::setw(11) << entry.t
<< std::setw(12) << entry.u
<< std::setw(12) << entry.y << "\n";
}
}

void persist_csv(const std::string& destination, const std::string& description,

Аня:
const std::vector<Row>& collection) {
std::ofstream sink(destination);
if (!sink) return;
sink << "# " << description << "\n";
sink << "t,u,y\n";
for (const auto& entry : collection) {
sink << entry.t << "," << entry.u << "," << entry.y << "\n";
}
}

void attempt_visualization(const std::string& csv_path) {
#ifdef _WIN32
std::string order = "python plot.py \"" + csv_path + "\" || py plot.py \"" + csv_path + "\"";
#else
std::string order = "python3 plot.py \"" + csv_path + "\"";
#endif
std::cout << "\nПробую показать график...\n";
if (std::system(order.c_str()) != 0) {
std::cout << "График недоступен. Данные лежат в " << csv_path << "\n";
}
}

} // namespace app

int main() {
using namespace app;
std::cout << std::fixed << std::setprecision(5);

while (true) {
std::cout << "\n====================================\n"
<< " ЛР-2 | Вариант 3 | Моделирование\n"
<< "====================================\n"
<< "1 Линейная (1.3)\n"
<< "2 Нелинейная (2.5)\n"
<< "3 Дифференциальная (3.7)\n"
<< "0 Завершить\n";

int decision = request_whole("пункт", 1, 0, 3);
if (decision == 0) break;

auto system = assemble_model(decision);
auto excitation = assemble_signal();
int length = request_whole("длина серии", 40, 1, 100000);

std::string comment;
bool accepted = system->isStable(comment);
if (const std::string summary = StabilityAnalyzer::report(accepted, comment);
!summary.empty()) {
std::cout << "\n" << summary << "\n";
}

auto series = simulate(*system, *excitation, length);
display_rows(series);

bool unbounded = false;
for (const auto& point : series) {
if (!std::isfinite(point.y) || std::fabs(point.y) > 1e6) {
unbounded = true;
break;
}
}
if (unbounded) {
std::cout << "\n[предупреждение] обнаружена расходимость\n";
}

std::string label = system->name() + " :: " + system->params() +
" :: " + excitation->name() +
" :: N=" + std::to_string(length);
std::string output_name = system->tag() + "_" + excitation->tag() + "_run.csv";
persist_csv(output_name, label, series);
std::cout << "Файл сохранён: " << output_name << "\n";

attempt_visualization(output_name);
}

std::cout << "\nСеанс завершён.\n";
return 0;
}

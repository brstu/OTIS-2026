std::cout << "\n--- Результаты ---\n";
    std::cout << std::setw(8) << "t" << std::setw(14) << "u" << std::setw(14) << "y" << "\n";
    for (const auto& row : data) {
        std::cout << std::setw(8) << row.t
                  << std::setw(14) << row.u
                  << std::setw(14) << row.y << "\n";
    }
}

void writeCsv(const std::string& filename, const std::string& header, const std::vector<Row>& data) {
    std::ofstream file(filename);
    if (!file) return;
    file << "# " << header << "\n";
    file << "t,u,y\n";
    for (const auto& row : data) {
        file << row.t << "," << row.u << "," << row.y << "\n";
    }
}

void openGraph(const std::string& csvFile) {
#ifdef _WIN32
    std::string command = "python plot.py " + csvFile + " || py plot.py " + csvFile;
#else
    std::string command = "python3 plot.py " + csvFile;
#endif
    std::cout << "\nОткрываю график...\n";
    if (std::system(command.c_str()) != 0) {
        std::cout << "Не удалось запустить Python/matplotlib.\n"
                  << "Данные сохранены в файл " << csvFile << "\n";
    }
}

} // namespace

int main() {
    std::cout << std::fixed << std::setprecision(6);

    while (true) {
        std::cout << "\n========================================\n"
                  << "  Симуляция моделей (вариант 3)\n"
                  << "========================================\n"
                  << "1. Линейная модель 1.3\n"
                  << "2. Нелинейная модель 2.5\n"
                  << "3. Дифференциальная модель 3.7 (Эйлер)\n"
                  << "0. Выход\n";

        int selection = askInt("Ваш выбор", 1, 0, 3);
        if (selection == 0) break;

        auto model = makeModel(selection);
        auto signal = makeSignal();
        int steps = askInt("Количество шагов", 40, 1, 50000);

        // Проверка устойчивости до запуска
        std::string details;
        bool ok = model->isStable(details);
        if (const auto message = StabilityAnalyzer::report(ok, details); !message.empty()) {
            std::cout << "\n" << message << "\n";
        }

        auto results = simulate(*model, *signal, steps);
        printResults(results);

        // Проверка на расходимость
        bool wentToInfinity = false;
        for (const auto& r : results) {
            if (!std::isfinite(r.y) || std::fabs(r.y) > 1e6) {
                wentToInfinity = true;
                break;
            }
        }
        if (wentToInfinity) {
            std::cout << "\nВнимание: процесс расходится (значения ушли в бесконечность).\n";
        }

        std::string fileTitle = model->name() + " | " + model->params() +
                                " | сигнал: " + signal->name() +
                                " | шагов=" + std::to_string(steps);
        std::string fileName = model->tag() + "_" + signal->tag() + ".csv";
        writeCsv(fileName, fileTitle, results);
        std::cout << "\nСохранено в " << fileName << "\n";

        openGraph(fileName);
    }

    std::cout << "Работа завершена.\n";
    return 0;
}

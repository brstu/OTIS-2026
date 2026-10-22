#include "simulation.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>

std::string inputName(InputType type) {
    switch (type) {
        case InputType::Step: return "step";
        case InputType::Impulse: return "impulse";
        case InputType::Harmonic: return "harmonic";
    }
    throw std::invalid_argument("Unknown input type");
}

double inputAt(InputType type, std::size_t step) {
    switch (type) {
        case InputType::Step: return 1.0;
        case InputType::Impulse: return step == 0 ? 1.0 : 0.0;
        case InputType::Harmonic: return std::sin(static_cast<double>(step));
    }
    throw std::invalid_argument("Unknown input type");
}

std::vector<Row> simulate(const Model& model, InputType signal, std::size_t n,
                          double dt, double y0) {
    if (n == 0 || !(dt > 0.0) || !std::isfinite(dt) || !std::isfinite(y0)) {
        throw std::invalid_argument("n must be positive, dt must be finite and >0");
    }
    std::vector<double> inputs;
    inputs.reserve(n);
    std::vector<Row> rows;
    rows.reserve(n);
    double y = y0;
    double previousY = 0.0;  // Initial history y_(-1) = 0 for Model 2.6.
    for (std::size_t tau = 0; tau < n; ++tau) {
        const double u = inputAt(signal, tau);
        inputs.push_back(u);
        const double next = model.nextStep({tau, y, previousY, u, inputs, dt});
        if (!std::isfinite(next)) {
            throw std::runtime_error(model.id() + " diverged at step " + std::to_string(tau)
                + "; change coefficients or the integration step");
        }
        rows.push_back({tau, u, y, next});
        previousY = y;
        y = next;
    }
    return rows;
}

void writeCsv(const std::filesystem::path& path, const std::vector<Row>& rows) {
    std::ofstream out(path);
    if (!out) throw std::runtime_error("Cannot open CSV: " + path.string());
    out << "tau,u_tau,y_tau,y_tau_plus_1\n";
    out << std::setprecision(12);
    for (const auto& r : rows) {
        out << r.tau << ',' << r.input << ',' << r.yCurrent << ',' << r.yNext << '\n';
    }
    if (!out) throw std::runtime_error("Error writing CSV: " + path.string());
}

namespace {
std::string number(double value, int precision = 3) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

std::string polyline(const std::vector<Row>& rows, double left, double top,
                     double plotWidth, double plotHeight,
                     double low, double high) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);
    for (std::size_t i = 0; i < rows.size(); ++i) {
        const double x = left + (rows.size() == 1 ? 0.0
            : plotWidth * static_cast<double>(i) / static_cast<double>(rows.size() - 1));
        const double y = top + (high - rows[i].yNext) / (high - low) * plotHeight;
        if (i) ss << ' ';
        ss << x << ',' << y;
    }
    return ss.str();
}
} // namespace

void writeSvg(const std::filesystem::path& path, const Model& model,
              const std::vector<Row>& stepRows,
              const std::vector<Row>& impulseRows,
              const std::vector<Row>& harmonicRows) {
    if (stepRows.empty() || stepRows.size() != impulseRows.size()
        || stepRows.size() != harmonicRows.size()) {
        throw std::invalid_argument("SVG requires equally sized, nonempty series");
    }
    double minY = 0.0;
    double maxY = 0.0;
    for (const auto* rows : {&stepRows, &impulseRows, &harmonicRows}) {
        for (const auto& r : *rows) {
            minY = std::min(minY, r.yNext);
            maxY = std::max(maxY, r.yNext);
        }
    }
    const double padding = std::max(0.15, (maxY - minY) * 0.12);
    minY -= padding;
    maxY += padding;

    constexpr double L = 88, T = 113, W = 960, H = 420;
    std::ofstream out(path);
    if (!out) throw std::runtime_error("Cannot open SVG: " + path.string());
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"1130\" height=\"650\" "
           "viewBox=\"0 0 1130 650\">\n";
    out << "<rect width=\"1130\" height=\"650\" fill=\"#ffffff\"/>\n";
    out << "<text x=\"88\" y=\"48\" font-size=\"27\" font-weight=\"bold\" "
           "font-family=\"Arial, sans-serif\" fill=\"#172135\">"
        << model.title() << "</text>\n";
    out << "<text x=\"88\" y=\"78\" font-size=\"15\" "
           "font-family=\"Arial, sans-serif\" fill=\"#51627a\">"
           "Variant 12 | output y(tau+1) | input step, impulse, sin(tau)" "</text>\n";
    out << "<rect x=\"" << L << "\" y=\"" << T << "\" width=\"" << W
        << "\" height=\"" << H << "\" fill=\"#f8fafc\"/>\n";
    for (int i = 0; i <= 5; ++i) {
        const double y = T + H * i / 5.0;
        const double v = maxY - (maxY - minY) * i / 5.0;
        out << "<line x1=\"" << L << "\" x2=\"" << (L + W)
            << "\" y1=\"" << y << "\" y2=\"" << y
            << "\" stroke=\"#d8e0ea\"/>\n";
        out << "<text x=\"" << (L - 13) << "\" y=\"" << y + 5
            << "\" text-anchor=\"end\" font-family=\"Arial\" font-size=\"13\" "
               "fill=\"#51627a\">" << number(v) << "</text>\n";
    }
    for (int i = 0; i <= 5; ++i) {
        const double x = L + W * i / 5.0;
        const auto step = 1 + static_cast<std::size_t>(std::llround(
            (stepRows.size() - 1) * i / 5.0));
        out << "<line x1=\"" << x << "\" x2=\"" << x
            << "\" y1=\"" << T << "\" y2=\"" << (T + H)
            << "\" stroke=\"#e7edf4\"/>\n";
        out << "<text x=\"" << x << "\" y=\"" << (T + H + 26)
            << "\" text-anchor=\"middle\" font-family=\"Arial\" font-size=\"13\" "
               "fill=\"#51627a\">" << step << "</text>\n";
    }
    const std::array<const std::vector<Row>*, 3> lines = {&stepRows, &impulseRows, &harmonicRows};
    const std::array<const char*, 3> colors = {"#2463d4", "#e17d30", "#19926e"};
    const std::array<const char*, 3> labels = {"Step: u=1", "Impulse: u(0)=1", "Harmonic: sin(tau)"};
    for (std::size_t i = 0; i < lines.size(); ++i) {
        out << "<polyline points=\"" << polyline(*lines[i], L, T, W, H, minY, maxY)
            << "\" fill=\"none\" stroke=\"" << colors[i]
            << "\" stroke-width=\"2.8\" stroke-linecap=\"round\" "
               "stroke-linejoin=\"round\"/>\n";
        const double legendX = L + i * 315.0;
        out << "<line x1=\"" << legendX << "\" x2=\"" << (legendX + 35)
            << "\" y1=\"610\" y2=\"610\" stroke=\"" << colors[i]
            << "\" stroke-width=\"4\"/>\n";
        out << "<text x=\"" << legendX + 46
            << "\" y=\"615\" font-family=\"Arial\" font-size=\"16\" "
               "fill=\"#22334d\">" << labels[i] << "</text>\n";
    }
    out << "<text x=\"568\" y=\"580\" font-family=\"Arial\" font-size=\"15\" "
           "text-anchor=\"middle\" fill=\"#334155\">Simulation step (tau+1)</text>\n";
    out << "<text x=\"27\" y=\"326\" font-family=\"Arial\" font-size=\"15\" "
           "text-anchor=\"middle\" fill=\"#334155\" "
           "transform=\"rotate(-90 27 326)\">y(tau+1)</text>\n";
    out << "</svg>\n";
    if (!out) throw std::runtime_error("Error writing SVG: " + path.string());
}

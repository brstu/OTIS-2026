#!/usr/bin/env python3
"""График двух столбцов result.csv без установки дополнительных пакетов."""
import csv
import sys


def draw(source, destination):
    rows = list(csv.DictReader(source))
    times = [int(row["tau"]) for row in rows]
    inputs = [float(row["u_tau"]) for row in rows]
    outputs = [float(row["y_tau"]) for row in rows]
    # Нормирование до вычитания позволяет отобразить и очень большие значения.
    scale = max(abs(value) for value in inputs + outputs) or 1.0
    bottom = min(0.0, min(inputs) / scale, min(outputs) / scale)
    top = max(0.0, max(inputs) / scale, max(outputs) / scale)
    if top == bottom:
        bottom, top = -1.0, 1.0

    def x(tau):
        return 110 + 780 * tau / max(times[-1], 1)

    def y(value):
        return 370 - 258 * (value / scale - bottom) / (top - bottom)

    svg = ['<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 960 448">',
           '<title>Вход и выход модели по шагам</title>',
           '<rect width="960" height="448" fill="white"/>',
           '<g font-family="Arial, sans-serif" fill="#192e43">',
           '<text x="38" y="43" font-size="26">Реакция модели на управление</text>',
           '<text x="38" y="72" font-size="16" fill="#43607b">Вариант 25 · Лабораторная работа № 2</text>']
    for tick in range(5):
        level = bottom + (top - bottom) * tick / 4
        py = 370 - 258 * tick / 4
        svg.append(f'<path d="M110 {py}H890" stroke="#dbe2e8"/>')
        svg.append(f'<text x="98" y="{py+5}" text-anchor="end" font-size="14">{level*scale:.3g}</text>')
    for tick in sorted({round(times[-1] * i / 5) for i in range(6)}):
        svg.append(f'<text x="{x(tick)}" y="393" text-anchor="middle" font-size="14">{tick}</text>')
    svg.append('<text x="500" y="430" text-anchor="middle">Номер шага τ</text>')
    for series, color, dashed, label, left in [
        (inputs, "#a85514", True, "Вход u", 620),
        (outputs, "#176695", False, "Выход y", 770),
    ]:
        dash = 'stroke-dasharray="6 4"' if dashed else ''
        points = ' '.join(f'{x(t):.2f},{y(v):.2f}' for t, v in zip(times, series))
        svg.append(f'<polyline points="{points}" fill="none" stroke="{color}" stroke-width="2.5" {dash}/>')
        if len(times) <= 50:
            for t,v in zip(times,series):
                svg.append(f'<circle cx="{x(t):.2f}" cy="{y(v):.2f}" r="2.4" fill="{color}"/>')
        svg.append(f'<path d="M{left} 67h32" stroke="{color}" stroke-width="3" {dash}/>')
        svg.append(f'<text x="{left+39}" y="72" font-size="16">{label}</text>')
    svg.append('</g></svg>')
    destination.write('\n'.join(svg))


if __name__ == '__main__':
    if len(sys.argv) != 1:
        sys.exit("Usage: python3 plot_results.py < result.csv > graph.svg")
    sys.stdout.reconfigure(encoding="utf-8")
    draw(sys.stdin, sys.stdout)

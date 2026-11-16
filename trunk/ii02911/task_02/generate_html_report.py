
import argparse
import html
import json
import subprocess
import sys
from pathlib import Path
import xml.etree.ElementTree as ET


def read_test_results(xml_path):
    if not xml_path.exists():
        raise FileNotFoundError(f"XML-файл не найден: {xml_path}")

    tree = ET.parse(xml_path)
    root = tree.getroot()

    cases = []

    for test in root.iter("testcase"):
        status = "PASSED"
        details = ""

        failure = test.find("failure")
        error = test.find("error")
        skipped = test.find("skipped")

        if failure is not None:
            status = "FAILED"
            details = failure.text or ""
        elif error is not None:
            status = "FAILED"
            details = error.text or ""
        elif skipped is not None:
            status = "SKIPPED"
            details = "Тест пропущен"

        cases.append({
            "class": test.get("classname", ""),
            "test": test.get("name", ""),
            "status": status,
            "time": test.get("time", ""),
            "details": details
        })

    return cases


def get_coverage(build_dir, project_dir):
    summary_path = build_dir / "coverage-summary.json"
    coverage_path = build_dir / "coverage.html"

    root_dir = project_dir.parent

    command = [
        sys.executable,
        "-m", "gcovr",
        "--root", str(root_dir),
        "--object-directory", str(build_dir),
        "--filter", r".*/(task_01|task_02)/src/.*",
        "--json-summary", str(summary_path),
        "--html-details", str(coverage_path)
    ]

    print("Сбор покрытия через gcovr...")

    subprocess.run(
        command,
        cwd=project_dir,
        check=True
    )

    if not summary_path.exists():
        raise RuntimeError("gcovr не создал JSON-файл покрытия")

    with open(summary_path, "r", encoding="utf-8") as file:
        data = json.load(file)

    return data


def format_percent(value):
    return f"{value:.2f}%"


def get_coverage_value(data, name):
    covered = data.get(f"{name}_covered", 0)
    total = data.get(f"{name}_total", 0)
    percent = data.get(f"{name}_percent", 0)

    return covered, total, percent


def generate_html(cases, coverage, html_path):
    total = len(cases)
    passed = sum(c["status"] == "PASSED" for c in cases)
    failed = sum(c["status"] == "FAILED" for c in cases)
    skipped = sum(c["status"] == "SKIPPED" for c in cases)

    line_covered, line_total, line_percent = get_coverage_value(
        coverage, "line"
    )

    function_covered, function_total, function_percent = get_coverage_value(
        coverage, "function"
    )

    rows = []

    for case in cases:
        class_name = html.escape(case["class"])
        test_name = html.escape(case["test"])
        status = case["status"]
        test_time = html.escape(case["time"])
        details = html.escape(case["details"])

        rows.append(
            f"""
            <tr>
                <td>{class_name}</td>
                <td>{test_name}</td>
                <td class="{status}">{status}</td>
                <td>{test_time}</td>
                <td>{details}</td>
            </tr>
            """
        )

    rows_html = "\n".join(rows)

    html_content = f"""<!DOCTYPE html>
<html lang="ru">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">

<title>OTIS Task 02 - Test Report</title>

<style>
body {{
    font-family: Arial, sans-serif;
    background: #f4f6f9;
    margin: 35px;
    color: #222;
}}

h1, h2 {{
    text-align: center;
}}

.summary {{
    display: flex;
    justify-content: center;
    flex-wrap: wrap;
    gap: 20px;
    margin: 30px;
}}

.card {{
    background: white;
    padding: 20px;
    border-radius: 10px;
    min-width: 120px;
    text-align: center;
    box-shadow: 0 2px 8px #ccc;
}}

.number {{
    font-size: 28px;
    font-weight: bold;
    margin-top: 8px;
}}

.coverage {{
    display: flex;
    justify-content: center;
    flex-wrap: wrap;
    gap: 20px;
    margin: 30px;
}}

.coverage-card {{
    background: white;
    padding: 20px;
    border-radius: 10px;
    min-width: 200px;
    text-align: center;
    box-shadow: 0 2px 8px #ccc;
}}

.coverage-number {{
    font-size: 25px;
    font-weight: bold;
    margin-top: 10px;
    color: #263b55;
}}

.coverage-details {{
    color: #666;
    margin-top: 8px;
}}

table {{
    width: 100%;
    border-collapse: collapse;
    background: white;
}}

th, td {{
    padding: 12px;
    border: 1px solid #ddd;
    text-align: left;
    overflow-wrap: anywhere;
}}

th {{
    background: #263b55;
    color: white;
}}

tr:nth-child(even) {{
    background: #f2f2f2;
}}

.PASSED {{
    color: green;
    font-weight: bold;
}}

.FAILED {{
    color: red;
    font-weight: bold;
}}

.SKIPPED {{
    color: #b8860b;
    font-weight: bold;
}}

.report-link {{
    text-align: center;
    margin: 30px;
}}

.report-link a {{
    display: inline-block;
    padding: 12px 24px;
    background: #263b55;
    color: white;
    text-decoration: none;
    border-radius: 7px;
}}

.report-link a:hover {{
    background: #3d5a7c;
}}
</style>
</head>

<body>

<h1>Отчет тестирования OTIS Task 02</h1>

<div class="summary">

    <div class="card">
        <div>Всего тестов</div>
        <div class="number">{total}</div>
    </div>

    <div class="card">
        <div>Успешно</div>
        <div class="number">{passed}</div>
    </div>

    <div class="card">
        <div>Ошибки</div>
        <div class="number">{failed}</div>
    </div>

    <div class="card">
        <div>Пропущено</div>
        <div class="number">{skipped}</div>
    </div>

</div>

<h2>Покрытие кода тестами</h2>

<div class="coverage">

    <div class="coverage-card">
        <div>Покрытие строк</div>
        <div class="coverage-number">
            {format_percent(line_percent)}
        </div>
        <div class="coverage-details">
            Покрыто: {line_covered} из {line_total}
        </div>
    </div>

    <div class="coverage-card">
        <div>Покрытие функций</div>
        <div class="coverage-number">
            {format_percent(function_percent)}
        </div>
        <div class="coverage-details">
            Покрыто: {function_covered} из {function_total}
        </div>
    </div>

</div>

<div class="report-link">
    <a href="coverage.html">
        Подробный отчет gcovr
    </a>
</div>

<h2>Результаты тестирования</h2>

<table>

<thead>
<tr>
    <th>Класс</th>
    <th>Тест</th>
    <th>Результат</th>
    <th>Время</th>
    <th>Описание ошибки</th>
</tr>
</thead>

<tbody>
{rows_html}
</tbody>

</table>

</body>
</html>
"""

    html_path.parent.mkdir(parents=True, exist_ok=True)

    html_path.write_text(
        html_content,
        encoding="utf-8"
    )

    print(f"HTML-отчет успешно создан: {html_path}")


def main():
    parser = argparse.ArgumentParser(
        description="Создание HTML-отчета Google Test и gcovr"
    )

    parser.add_argument("--xml", required=True)
    parser.add_argument("--html", required=True)
    parser.add_argument("--build-dir", default="build")

    args = parser.parse_args()

    project_dir = Path(__file__).resolve().parent
    build_dir = (project_dir / args.build_dir).resolve()
    xml_path = Path(args.xml)

    if not xml_path.is_absolute():
        xml_path = project_dir / xml_path

    html_path = Path(args.html)

    if not html_path.is_absolute():
        html_path = project_dir / html_path

    try:
        cases = read_test_results(xml_path)

        coverage = get_coverage(build_dir, project_dir)

        generate_html(cases, coverage, html_path)

    except Exception as error:
        print(f"Ошибка создания HTML-отчета: {error}")
        sys.exit(1)


if __name__ == "__main__":
    main()

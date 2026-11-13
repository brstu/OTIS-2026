import xml.etree.ElementTree as ET
import sys
import os
import json

def generate_html(xml_file, html_file, coverage_json=None):
    if not os.path.exists(xml_file):
        print(f"Файл {xml_file} не найден.")
        return

    try:
        tree = ET.parse(xml_file)
        root = tree.getroot()
    except Exception as e:
        print(f"Ошибка при чтении XML: {e}")
        return

    total_tests = int(root.attrib.get('tests', 0))
    failures = int(root.attrib.get('failures', 0))
    time = root.attrib.get('time', '0')
    passed = total_tests - failures

    # Читаем проценты покрытия из summary.json (если gcovr его создал)
    line_cov = "Н/Д"
    func_cov = "Н/Д"
    if coverage_json and os.path.exists(coverage_json):
        try:
            with open(coverage_json, 'r', encoding='utf-8') as jf:
                cov_data = json.load(jf)
                if 'line_percent' in cov_data and cov_data['line_percent'] is not None:
                    line_cov = f"{cov_data['line_percent']}%"
                if 'function_percent' in cov_data and cov_data['function_percent'] is not None:
                    func_cov = f"{cov_data['function_percent']}%"
        except Exception as e:
            print(f"Ошибка при чтении покрытия: {e}")

    html = f"""<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Google Test Report</title>
    <style>
        body {{ font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #f4f7f6; color: #333; margin: 0; padding: 20px; }}
        .container {{ max-width: 1100px; margin: 0 auto; background: #fff; padding: 20px; border-radius: 8px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }}
        h1 {{ border-bottom: 2px solid #eee; padding-bottom: 10px; color: #2c3e50; font-size: 24px; }}
        .summary {{ display: flex; flex-wrap: wrap; gap: 12px; margin-bottom: 20px; }}
        .stat-box {{ flex: 1; min-width: 140px; padding: 15px 10px; border-radius: 6px; text-align: center; color: white; font-weight: bold; font-size: 1.05em; }}
        .stat-total {{ background-color: #3498db; }}
        .stat-passed {{ background-color: #2ecc71; }}
        .stat-failed {{ background-color: #e74c3c; }}
        .stat-time {{ background-color: #95a5a6; }}
        .stat-lines {{ background-color: #9b59b6; }}
        .stat-funcs {{ background-color: #f39c12; }}
        table {{ width: 100%; border-collapse: collapse; margin-top: 20px; }}
        th, td {{ padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }}
        th {{ background-color: #f8f9fa; color: #333; }}
        tr:hover {{ background-color: #f9f9f9; }}
        .pass {{ color: #2ecc71; font-weight: bold; padding: 4px 8px; background: #e8f8f5; border-radius: 4px; }}
        .fail {{ color: #e74c3c; font-weight: bold; padding: 4px 8px; background: #fdedec; border-radius: 4px; }}
        .failure-msg {{ font-family: monospace; background: #fdf2f2; color: #c0392b; padding: 10px; border-radius: 4px; font-size: 0.9em; white-space: pre-wrap; margin-top: 8px; border-left: 4px solid #e74c3c; }}
    </style>
</head>
<body>
    <div class="container">
        <h1>Отчет о тестировании (Google Test)</h1>
        <div class="summary">
            <div class="stat-box stat-total">Всего тестов: {total_tests}</div>
            <div class="stat-box stat-passed">Успешно: {passed}</div>
            <div class="stat-box stat-failed">Упало: {failures}</div>
            <div class="stat-box stat-time">Время: {time} с</div>
            <div class="stat-box stat-lines">Покрытие строк: {line_cov}</div>
            <div class="stat-box stat-funcs">Покрытие функций: {func_cov}</div>
        </div>
        <table>
            <thead>
                <tr>
                    <th>Test Suite</th>
                    <th>Test Case</th>
                    <th>Статус</th>
                    <th>Время (с)</th>
                </tr>
            </thead>
            <tbody>
"""

    for suite in root.findall('.//testsuite'):
        suite_name = suite.attrib.get('name', 'Unknown')
        for case in suite.findall('testcase'):
            case_name = case.attrib.get('name', 'Unknown')
            time_taken = case.attrib.get('time', '0')
            failure = case.find('failure')

            if failure is not None:
                status_html = '<span class="fail">FAIL</span>'
                fail_msg = failure.attrib.get('message', failure.text or '').replace('<', '&lt;').replace('>', '&gt;')
                fail_details = f'<div class="failure-msg">{fail_msg}</div>'
            else:
                status_html = '<span class="pass">PASS</span>'
                fail_details = ''

            html += f"""
                <tr>
                    <td><strong>{suite_name}</strong></td>
                    <td>{case_name}{fail_details}</td>
                    <td>{status_html}</td>
                    <td>{time_taken}</td>
                </tr>
            """

    html += """
            </tbody>
        </table>
    </div>
</body>
</html>
"""

    with open(html_file, 'w', encoding='utf-8') as f:
        f.write(html)

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Использование: python gtest_to_html.py <путь_к_xml> <путь_к_html> [путь_к_summary_json]")
        sys.exit(1)
    
    xml_path = sys.argv[1]
    html_path = sys.argv[2]
    json_path = sys.argv[3] if len(sys.argv) > 3 else None
    
    generate_html(xml_path, html_path, json_path)
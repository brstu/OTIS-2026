<p align="center">Министерство образования Республики Беларусь</p>
<p align="center">Учреждение образования</p>
<p align="center">«Брестский государственный технический университет»</p>
<p align="center">Кафедра ИИТ</p>
<br><br><br><br><br>
<p align="center"><b>Лабораторная работа №1</b></p>
<p align="center"><b>По дисциплине: «Общая теория интеллектуальных систем»</b></p>
<p align="center"><b>Тема: «Моделирование линейных, нелинейных и дифференциальных объектов»</b></p>
<br><br><br><br>
<p align="right">Выполнил:</p>
<p align="right">Студент 2 курса</p>
<p align="right">Группы ИИ-30</p>
<p align="right">Угляница Е. Н.</p>
<p align="right">Проверил:</p>
<p align="right">Дворанинович Д. А.</p>
<br><br><br><br>
<p align="center">Брест 2026</p>

---

## Вариант 1

- Линейная модель: **Model 1.1**
- Нелинейная модель: **Model 2.3**
- Дифференциальное уравнение: **Model 3.5**

## Реализованные модели

| Блок | Модель | Тип | Название |
|:--:|:--:|:--:|---|
| 1 | 1.1 | линейная дискретная | Standard First-Order Inertial System |
| 2 | 2.3 | нелинейная дискретная | Dead-Zone Non-linearity |
| 3 | 3.5 | дифференциальное уравнение | Harmonic Driving Force |

### Математические описания и параметры

- **Model 1.1**: `y(t+1) = a*y(t) + b*u(t)` (параметры по умолчанию: `a = 0.8`, `b = 0.5`). Проверяется устойчивость по корню характеристического уравнения `z = a` (`|a| < 1`).
- **Model 2.3**: `y(t+1) = a*y(t) + b*deadzone(u(t))`, где `deadzone(u) = u - δ` при `u > δ`, `0` при `|u| <= δ`, и `u + δ` при `u < -δ` (параметры по умолчанию: `a = 0.8`, `b = 0.5`, `δ = 0.2`).
- **Model 3.5**: `dy/dt = b*sin(u)` (метод Эйлера: `y(t+1) = y(t) + dt*(b*sin(u(t)))`, параметры по умолчанию: `b = 1.0`, `dt = 0.1`).

Начальное значение `y0 = 0`, уровень ступеньки `c = 1` (все значения можно изменить при запуске программы).

## Входные воздействия

1. Ступенчатое: `u(t) = c`
2. Импульсное: `u(0) = 1`, `u(t > 0) = 0`
3. Гармоническое: `u(t) = sin(t)`

## ООП

Созданы абстрактные базовые классы `Model` и `Input`.

От класса `Model` наследуются конкретные модели:
- `LinearModel` — Model 1.1
- `NonlinearModel` — Model 2.3
- `DiffModel` — Model 3.5

От класса `Input` наследуются типы входных воздействий:
- `StepInput` — ступенчатое
- `ImpulseInput` — импульсное
- `HarmonicInput` — гармоническое

## UML Диаграмма классов

```mermaid
classDiagram
    class Model {
        <<abstract>>
        -y double
        -y0 double
        #setY(value)
        +next(u) double*
        +name() string*
        +reset()
        +current() double
    }

    class LinearModel {
        -a double
        -b double
        +next(u) double
        +isStable() bool
        +root() double
    }

    class NonlinearModel {
        -a double
        -b double
        -delta double
        +next(u) double
        +deadzone(u) double
    }

    class DiffModel {
        -b double
        -dt double
        +next(u) double
    }

    class Input {
        <<abstract>>
        +value(tau) double*
        +name() string*
    }

    class StepInput {
        -c double
        +value(tau) double
    }

    class ImpulseInput {
        +value(tau) double
    }

    class HarmonicInput {
        +value(tau) double
    }

    Model <|-- LinearModel
    Model <|-- NonlinearModel
    Model <|-- DiffModel
    Input <|-- StepInput
    Input <|-- ImpulseInput
    Input <|-- HarmonicInput
```

Пример работы программы
Сборка и запуск

### Сборка и запуск

Сборка осуществляется с помощью файла `build.bat`:

```bat
@echo off
chcp 65001 > nul
cd /d "%~dp0"

echo ===== [1/4] CMake configure =====
cmake -S . -B build
if errorlevel 1 goto error

echo ===== [2/4] CMake build =====
cmake --build build --config Release
if errorlevel 1 goto error

set "EXE="
if exist "build\Release\ii03023_lab1.exe" set "EXE=build\Release\ii03023_lab1.exe"
if exist "build\Debug\ii03023_lab1.exe" set "EXE=build\Debug\ii03023_lab1.exe"
if exist "build\ii03023_lab1.exe" set "EXE=build\ii03023_lab1.exe"
if not defined EXE goto notfound
echo Program: %EXE%

:run
echo.
echo ===== [3/4] Run program =====
if exist results.csv del results.csv
"%EXE%"
if not exist results.csv goto noresults

echo.
echo ===== [4/4] Plot (Python + matplotlib) =====
python plot.py
goto ask

:noresults
echo results.csv was not created, skipping plot.

:ask
echo.
set "AGAIN="
set /p AGAIN="Run again with another model/input? (y/n): "
if /i "%AGAIN%"=="y" goto run
echo ===== DONE =====
exit /b 0

:notfound
echo Program ii03023_lab1.exe was not found after build.
echo Check that CMakeLists.txt contains: add_executable(ii03023_lab1 main.cpp models.h)
goto error

:error
echo BUILD FAILED
pause
exit /b 1
```
### Демонстрация исполняемого файла ii03023_lab1.exe:

![](console_input2.jpg)

![](console_input1.jpg)

### Примеры графиков переходных процессов

**Демонстрация Model 1.1 (линейная):**
![](plot1.png)

**Демонстрация Model 2.3 (нелинейная):**
![](plo2.png)

**Демонстрация Model 3.5 (дифференциальное уравнение):**
![](plot3.png)
## Вывод

Программа:
- позволяет выбрать модель (одну или все) и входное воздействие;
- принимает начальные значения и коэффициенты с возможностью использования значений по умолчанию;
- проверяет устойчивость линейной модели (Model 1.1);
- симулирует отклик систем на ступенчатое, импульсное и гармоническое воздействия;
- выводит результаты в табличном виде, сохраняет их в `results.csv` и визуализирует графики через Python/Matplotlib.





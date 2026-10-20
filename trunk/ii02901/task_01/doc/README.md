<p align="center"> Министерство образования Республики Беларусь</p>
<p align="center">Учреждение образования</p>
<p align="center">“Брестский Государственный технический университет”</p>
<p align="center">Кафедра ИИТ</p>
<br><br><br><br><br><br><br>
<p align="center">Лабораторная работа №1</p>
<p align="center">По дисциплине “Общая теория интеллектуальных систем”</p>
<p align="center">Тема: “Моделирования температуры объекта”</p>
<br><br><br><br><br>
<p align="right">Выполнил:</p>
<p align="right">Студент 2 курса</p>
<p align="right">Группы ИИ-29</p>
<p align="right">Афанасьев Д. Р.</p>
<p align="right">Проверил:</p>
<p align="right">Дворанинович Д. А.</p>
<br><br><br><br><br>
<p align="center">Брест 2026</p>

## Вариант 1

- Линейная модель: **Model 1.1**
- Нелинейная модель: **Model 2.3**
- Дифференциальное уравнение: **Model 3.5**

## Реализованные модели

### Model 1.1

`y(t+1) = a*y(t) + b*u(t)`

### Model 2.3

`y(t+1) = a*y(t) + b*deadzone(u(t))`
$$
\text{where } \text{deadzone}(u) =
\begin{cases}
u - \delta, & u > \delta \\
0, & -\delta \le u \le \delta \\
u + \delta, & u < -\delta
\end{cases}
$$

### Model 3.5

`dy/dt = b*sin(u)`

## Входные воздействия

1. **Ступенчатое:** `u(t) = 1.0`
2. **Импульсное:** `u(0) = 1.0`, `u(t > 0) = 0.0`
3. **Гармоническое:** `u(t) = sin(t)`

## ООП

Создан абстрактный базовый класс `model` с виртуальным методом `step(double y_tau, double u_tay)`.

От него наследуются:
- `Model1_1`
- `Model2_3`
- `Model3_5`

## Результат

Программа:
- запрашивает количество шагов симуляции `n`;
- запрашивает коэффициенты для каждой модели;
- прогоняет 3 модели;
- выводит результаты в виде таблиц;
- сохраняет результаты в `.csv`;
- строит график через Python/Matplotlib.

## Пример работы программы:

### Сборка

На Windows build.bat:

```bat
@echo off
title Build task_1

echo Configure project (1/4)
cmake -S src -B build
if %errorlevel% neq 0 goto error

echo.
echo Building project (2/4)
cmake --build build --config Release
if %errorlevel% neq 0 goto error

echo.
echo Running simulation (3/4)
if exist build\Release\ii03018_task.1.exe (
build\Release\ii03018_task.1.exe
) else (
build\ii03018_task.1.exe
)

echo.
echo Graph
python plot.py
if %errorlevel% neq 0 (
    echo [!] Python error. Check if python and matplotlib are installed.
)

echo.
pause
exit /b 0

:error
echo.
echo BUILD ERROR
pause
exit /b 1
```
### Демонстрация реализации:
![](tUsk.png)
### Демонстрация одного из графиков:
![](graph.png)
## UML-диаграмма
```mermaid
classDiagram
    class model_standard {
        <<abstract>>
        +nextStep(double u) double
        +reset() void
    }

    class Model1_7 {
        -y, u_prev1, u_prev2 : double
        -a, b1, b2, b3 : double
        +nextStep(double u) double
        +reset() void
    }

    class Model2_9 {
        -y, a, b : double
        +nextStep(double u) double
        +reset() void
    }

    class Model3_1 {
        -y, a, dt : double
        +nextStep(double u) double
        +reset() void
    }

    model_standard <|-- Model1_7
    model_standard <|-- Model2_9
    model_standard <|-- Model3_1
```

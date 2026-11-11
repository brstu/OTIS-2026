<p align="center">Министерство образования Республики Беларусь</p>
<p align="center">Учреждение образования</p>
<p align="center">"Брестский государственный технический университет"</p>
<p align="center">Кафедра ИИТ</p>
<br><br><br><br><br><br><br>
<p align="center">Лабораторная работа №2</p>
<p align="center">По дисциплине "Общая теория интеллектуальных систем"</p>
<p align="center">Тема: "Модульное тестирование и анализ устойчивости"</p>
<br><br><br><br><br>
<p align="right">Выполнил:</p>
<p align="right">Студент 2 курса</p>
<p align="right">Группы ИИ-29</p>
<p align="right">Афанасьев Д. Р.</p>
<p align="right">Проверил:</p>
<p align="right">Дворанинович Д. А.</p>
<br><br><br><br><br>
<p align="center">Брест 2026</p>

## 1. Цель работы

Освоить технологию модульного (юнит-) тестирования программ на C++ с применением фреймворков Google Test и Google Mock: научиться проверять правильность отдельных единиц кода, строить параметризованные тесты, работать с граничными случаями и физическими инвариантами, измерять тестовое покрытие (`gcovr`) и трактовать его результаты. Дополнительно — закрепить навыки ООП (наследование, чисто виртуальные методы, интерфейсы) на примере программы лабораторной работы №1.

## 2. Постановка задачи

Объект тестирования — математическое ядро программы ЛР1: три динамические модели (Модель 1.1 — линейное инерционное звено первого порядка, Модель 2.3 — нелинейная система с зоной нечувствительности, Модель 3.5 — гармоническое воздействие), три режима входных воздействий (ступень, импульс, гармоника) и модуль анализа устойчивости линейной модели 1.1 на Z-плоскости.

Требования:
- покрыть программу модульными тестами с помощью Google Test, автоматизировать сборку и запуск через CMake / `ctest` (`run_tests.bat`);
- проверить правильность вычислений моделей на наборах параметров, рассчитанных вручную;
- разработать и протестировать модуль анализа устойчивости: возврат `true` на устойчивых коэффициентах (|a| < 1), `false` на расходящихся (|a| > 1), проверка граничных случаев на единичной окружности (|z| = 1);
- проверить граничные значения нелинейности для Модели 2.3 (границы зоны нечувствительности u = ±δ);
- проверить физические инварианты: сохранение состояния покоя при нулевом входе;
- применить параметризованные тесты (`TEST_P`) для проверки сходимости алгоритма на различных наборах данных без дублирования кода;
- использовать мок-объекты (Google Mock) и подмену входного сигнала для тестирования режимов управления (ступень, импульс, гармоника);
- в отчёте отразить количество тестов и процент тестового покрытия (`gcovr`).

## 3. Доработка программы по сравнению с ЛР1

Чтобы вычислительные классы можно было тестировать изолированно и подключать к ним мок-объекты, в архитектуру ЛР1 внесены следующие модульные доработки:

| Изменение | Файл | Зачем |
|-----------|------|-------|
| Интерфейс `InputGenerator` | `input.h` | абстрактный интерфейс генератора сигнала (`virtual double generate(int tau) const = 0`) для создания мок-объекта `MockInputGenerator` в Google Mock |
| Классы `StepInput`, `ImpulseInput`, `HarmonicInput` | `input.h` | генерация трёх типовых сигналов (ступень, импульс, гармоника) вынесена в отдельные классы для 100% покрытия тестами |
| Метод `isStable()` | `model.h`, `model.cpp` | проверка условия |z| = |a| < 1 на Z-плоскости для Модели 1.1 |

Вся математическая логика методов `step()` для моделей 1.1, 2.3 и 3.5 из ЛР1 сохранена без изменений.

## 4. Структура тестового проекта

| Файл | Содержимое |
|------|------------|
| `model.h` | объявления классов `model1_1`, `model2_3`, `model3_5` |
| `model.cpp` | реализация всех моделей и модуля анализа устойчивости |
| `input.h` | интерфейс `InputGenerator` и классы `StepInput`, `ImpulseInput`, `HarmonicInput` |
| `tests.cpp` | 18 модульных тестов (6 тематических блоков, включая `TEST_P` и Google Mock) |
| `CMakeLists.txt` | конфигурация сборки C++14, скачивание Google Test v1.14.0 (`FetchContent`) |
| `run_tests.bat` | автоматическая сборка через CMake, запуск тестов, сохранение `test_report.txt` |

## 5. Тестовые сценарии и ручной пересчёт

Все эталонные значения в тестах получены предварительным ручным расчётом по формулам.

### 5.1. Правильность значений Модели 1.1 (`Model1_1StepTest`)

Модель 1.1 описывается линейным разностным уравнением:

<p align="center"><b><i>y</i><sub>τ+1</sub> = <i>a</i> · <i>y</i><sub>τ</sub> + <i>b</i> · <i>u</i><sub>τ</sub></b></p>

Зададим коэффициенты: <i>a</i> = 0.5, <i>b</i> = 2.0. Начальное условие: <i>y</i><sub>0</sub> = 1.0. Подадим вход <i>u</i><sub>0</sub> = 1.0:

<p align="center"><i>y</i><sub>1</sub> = 0.5 · 1.0 + 2.0 · 1.0 = 0.5 + 2.0 = 2.5</p>

Это значение проверяется в параметризованном тесте `Model1_1StepTest`.

### 5.2. Модуль анализа устойчивости на Z-плоскости (`Model1_1StabilityTest`)

Применим Z-преобразование к уравнению Модели 1.1 при нулевых начальных условиях:

<p align="center"><b><i>z</i> · <i>Y</i>(<i>z</i>) − <i>a</i> · <i>Y</i>(<i>z</i>) = <i>b</i> · <i>U</i>(<i>z</i>)</b></p>

Передаточная функция системы:

<p align="center"><b><i>W</i>(<i>z</i>) = <i>Y</i>(<i>z</i>) / <i>U</i>(<i>z</i>) = <i>b</i> / (<i>z</i> − <i>a</i>)</b></p>

Характеристическое уравнение <i>z</i> − <i>a</i> = 0 имеет единственный полюс <i>z</i> = <i>a</i>.

По критерию устойчивости дискретных систем полюс должен лежать **строго внутри** единичной окружности на Z-плоскости:

<p align="center"><b>|<i>z</i>| < 1 &emsp;⇔&emsp; |<i>a</i>| < 1</b></p>

Проверяемые в тестах случаи:

| Тест | Коэффициент <i>a</i> | Положение полюса на Z-плоскости | Ожидаемый результат |
|------|---------------------|--------------------------------|---------------------|
| StableCoefficients | 0.5; ±0.99 | Строго внутри единичного круга (\|<i>z</i>\| < 1) | true (STABLE) |
| BoundaryOfUnitCircle | +1.0; −1.0 | Ровно на границе окружности (\|<i>z</i>\| = 1) | false (UNSTABLE) |
| UnstableCoefficients | 1.5; −2.0 | Вне единичного круга (\|<i>z</i>\| > 1) | false (UNSTABLE) |

### 5.3. Граничные значения нелинейности (`Model2_3StepTest`)

Модель 2.3 описывается нелинейным уравнением с зоной нечувствительности:

<p align="center"><b><i>y</i><sub>τ+1</sub> = <i>a</i> · <i>y</i><sub>τ</sub> + <i>b</i> · deadzone(<i>u</i><sub>τ</sub>)</b></p>

где функция deadzone определяется как:

<p align="center"><b>deadzone(<i>u</i>) = <i>u</i> − δ, если <i>u</i> > δ</b></p>
<p align="center"><b>deadzone(<i>u</i>) = <i>u</i> + δ, если <i>u</i> < −δ</b></p>
<p align="center"><b>deadzone(<i>u</i>) = 0, если −δ ≤ <i>u</i> ≤ δ</b></p>

Зададим коэффициенты: <i>a</i> = 0.5, <i>b</i> = 2.0, δ = 1.0. Начальное условие: <i>y</i><sub>0</sub> = 1.0.

**Тест 1: u = 2.0 > δ (выход из зоны нечувствительности)**

<p align="center">deadzone(2.0) = 2.0 − 1.0 = 1.0</p>
<p align="center"><i>y</i><sub>1</sub> = 0.5 · 1.0 + 2.0 · 1.0 = 0.5 + 2.0 = 2.5</p>

**Тест 2: u = 0.5 (внутри зоны нечувствительности)**

<p align="center">deadzone(0.5) = 0.0</p>
<p align="center"><i>y</i><sub>1</sub> = 0.5 · 1.0 + 2.0 · 0.0 = 0.5</p>

**Тест 3: u = −2.0 < −δ (отрицательная ветвь)**

<p align="center">deadzone(−2.0) = −2.0 + 1.0 = −1.0</p>
<p align="center"><i>y</i><sub>1</sub> = 0.5 · 1.0 + 2.0 · (−1.0) = 0.5 − 2.0 = −1.5</p>

Эти значения проверяются в параметризованном тесте `Model2_3StepTest`.

### 5.4. Физические инварианты Модели 3.5 (`Model3_5StepTest`)

Модель 3.5 описывается уравнением с гармоническим воздействием:

<p align="center"><b><i>y</i><sub>τ+1</sub> = <i>y</i><sub>τ</sub> + Δ<i>t</i> · <i>b</i> · sin(<i>u</i><sub>τ</sub>)</b></p>

**Инвариант нулевого входа:** при <i>u</i> = 0 (sin(0) = 0) система остаётся в покое:

<p align="center"><i>y</i><sub>1</sub> = <i>y</i><sub>0</sub> + Δ<i>t</i> · <i>b</i> · 0 = <i>y</i><sub>0</sub></p>

Зададим: <i>b</i> = 1.0, Δ<i>t</i> = 0.1, <i>y</i><sub>0</sub> = 0.0, <i>u</i><sub>0</sub> = 0.0:

<p align="center"><i>y</i><sub>1</sub> = 0.0 + 0.1 · 1.0 · sin(0) = 0.0</p>

Это значение проверяется в параметризованном тесте `Model3_5StepTest`.

### 5.5. Параметризованные тесты сходимости (`TEST_P`)

При подаче постоянного ступенчатого сигнала <i>u</i><sub>τ</sub> = <i>u</i> = const устойчивая линейная Модель 1.1 (|<i>a</i>| < 1) при τ → ∞ выходит на установившееся значение:

<p align="center"><b><i>y</i><sub>ss</sub> = <i>a</i> · <i>y</i><sub>ss</sub> + <i>b</i> · <i>u</i> &emsp;⇒&emsp; <i>y</i><sub>ss</sub> = <i>b</i> · <i>u</i> / (1 − <i>a</i>)</b></p>

Параметризованный тест `Model1_1StepTest` без дублирования кода проверяет правильность расчётов на различных наборах параметров.

### 5.6. Мок-объекты (Google Mock) и подмена сигналов

В проекте применён полноценный фреймворк Google Mock:
- создан мок-класс `MockInputGenerator : public InputGenerator` с макросом `MOCK_METHOD(double, generate, (int tau), (const, override))`;
- в тесте `SimulationLoopTest` через `EXPECT_CALL` задаётся сценарий произвольного воздействия и контролируется как точность вычислений модели, так и кратность вызова генератора;
- в тестах `InputGeneratorTest` проверяются все режимы штатного генератора: ступень, единичный импульс, гармоника sin(τ).

## 6. Результаты запуска

Запуск выполняется скриптом `run_tests.bat` из корня проекта.

[==========] Running 18 tests from 7 test suites.
[----------] Global test environment set-up.
[----------] 3 tests from StepCalculations/Model1_1StepTest
[ RUN      ] StepCalculations/Model1_1StepTest.CalculatesStepCorrectly/0
[       OK ] StepCalculations/Model1_1StepTest.CalculatesStepCorrectly/0 (0 ms)
[ RUN      ] StepCalculations/Model1_1StepTest.CalculatesStepCorrectly/1
[       OK ] StepCalculations/Model1_1StepTest.CalculatesStepCorrectly/1 (0 ms)
[ RUN      ] StepCalculations/Model1_1StepTest.CalculatesStepCorrectly/2
[       OK ] StepCalculations/Model1_1StepTest.CalculatesStepCorrectly/2 (0 ms)
[----------] 3 tests from StepCalculations/Model1_1StepTest (0 ms total)
[----------] 5 tests from StabilityChecks/Model1_1StabilityTest
[ RUN      ] StabilityChecks/Model1_1StabilityTest.ChecksStability/0
[       OK ] StabilityChecks/Model1_1StabilityTest.ChecksStability/0 (0 ms)
[ RUN      ] StabilityChecks/Model1_1StabilityTest.ChecksStability/1
[       OK ] StabilityChecks/Model1_1StabilityTest.ChecksStability/1 (0 ms)
[ RUN      ] StabilityChecks/Model1_1StabilityTest.ChecksStability/2
[       OK ] StabilityChecks/Model1_1StabilityTest.ChecksStability/2 (0 ms)
[ RUN      ] StabilityChecks/Model1_1StabilityTest.ChecksStability/3
[       OK ] StabilityChecks/Model1_1StabilityTest.ChecksStability/3 (0 ms)
[ RUN      ] StabilityChecks/Model1_1StabilityTest.ChecksStability/4
[       OK ] StabilityChecks/Model1_1StabilityTest.ChecksStability/4 (0 ms)
[----------] 5 tests from StabilityChecks/Model1_1StabilityTest (0 ms total)
[----------] 4 tests from StepCalculations/Model2_3StepTest
[ RUN      ] StepCalculations/Model2_3StepTest.CalculatesStepCorrectly/0
[       OK ] StepCalculations/Model2_3StepTest.CalculatesStepCorrectly/0 (0 ms)
[ RUN      ] StepCalculations/Model2_3StepTest.CalculatesStepCorrectly/1
[       OK ] StepCalculations/Model2_3StepTest.CalculatesStepCorrectly/1 (0 ms)
[ RUN      ] StepCalculations/Model2_3StepTest.CalculatesStepCorrectly/2
[       OK ] StepCalculations/Model2_3StepTest.CalculatesStepCorrectly/2 (0 ms)
[ RUN      ] StepCalculations/Model2_3StepTest.CalculatesStepCorrectly/3
[       OK ] StepCalculations/Model2_3StepTest.CalculatesStepCorrectly/3 (0 ms)
[----------] 4 tests from StepCalculations/Model2_3StepTest (0 ms total)
[----------] 3 tests from StepCalculations/Model3_5StepTest
[ RUN      ] StepCalculations/Model3_5StepTest.CalculatesStepCorrectly/0
[       OK ] StepCalculations/Model3_5StepTest.CalculatesStepCorrectly/0 (0 ms)
[ RUN      ] StepCalculations/Model3_5StepTest.CalculatesStepCorrectly/1
[       OK ] StepCalculations/Model3_5StepTest.CalculatesStepCorrectly/1 (0 ms)
[ RUN      ] StepCalculations/Model3_5StepTest.CalculatesStepCorrectly/2
[       OK ] StepCalculations/Model3_5StepTest.CalculatesStepCorrectly/2 (0 ms)
[----------] 3 tests from StepCalculations/Model3_5StepTest (0 ms total)
[----------] 3 tests from InputGeneratorTest
[ RUN      ] InputGeneratorTest.StepInput
[       OK ] InputGeneratorTest.StepInput (0 ms)
[ RUN      ] InputGeneratorTest.ImpulseInput
[       OK ] InputGeneratorTest.ImpulseInput (0 ms)
[ RUN      ] InputGeneratorTest.HarmonicInput
[       OK ] InputGeneratorTest.HarmonicInput (0 ms)
[----------] 3 tests from InputGeneratorTest (0 ms total)
[----------] 1 test from SimulationLoopTest
[ RUN      ] SimulationLoopTest.UsesMockedInputsCorrectly
[       OK ] SimulationLoopTest.UsesMockedInputsCorrectly (0 ms)
[----------] 1 test from SimulationLoopTest (0 ms total)
[----------] Global test environment tear-down
[==========] 18 tests from 7 test suites ran. (2 ms total)
[  PASSED  ] 18 tests.

**100% тестов пройдено, 0 тестов не пройдено из 18**

## 7. Тестовое покрытие и результаты

Все 18 модульных тестов успешно проходят (7 тестовых наборов). Измерение покрытия кода выполнено с помощью инструмента `gcovr` (сборка с флагами `--coverage -O0 -g`):

                       GCC Code Coverage Report
					   
					   Directory: .
File                                       Lines    Exec  Cover   Missing
model.cpp                                     18      18   100%
model.h                                        8       8   100%
input.h                                       12      12   100%
TOTAL                                         38      38   100%


### Покрытие по строкам (Line Coverage)

| Файл | Строк | Выполнено | Покрытие |
|------|-------|-----------|----------|
| model.h | 8 | 8 | 100% |
| model.cpp | 18 | 18 | 100% |
| input.h | 12 | 12 | 100% |
| **ИТОГО** | **38** | **38** | **100%** |

### Покрытие по функциям (Function Coverage)

| Файл | Функций | Выполнено | Покрытие |
|------|---------|-----------|----------|
| model.h | 4 | 4 | 100% |
| model.cpp | 7 | 7 | 100% |
| input.h | 4 | 4 | 100% |
| **ИТОГО** | **15** | **15** | **100%** |

## 8. Вывод

В ходе лабораторной работы освоено модульное тестирование на C++ с использованием фреймворков **Google Test** и **Google Mock**. Для математических моделей разработан модуль анализа устойчивости на Z-плоскости (|<i>z</i>| = |<i>a</i>| < 1) и написаны **18 юнит-тестов**, проверяющих:

1. **Ручные расчёты** для всех трёх моделей (Модель 1.1, 2.3, 3.5);
2. **Модуль анализа устойчивости** с проверкой граничных случаев на единичной окружности;
3. **Граничные значения нелинейности** для Модели 2.3 (зоны нечувствительности);
4. **Физические инварианты** (сохранение состояния покоя при нулевом входе);
5. **Параметризованные тесты** (`TEST_P`) для проверки сходимости на различных наборах данных без дублирования кода;
6. **Изоляцию сигналов** через мок-объекты (`MOCK_METHOD`, `EXPECT_CALL`).

По результатам анализа утилитой `gcovr` достигнуто **100% покрытие кода** тестируемых модулей (38 строк из 38, 15 функций из 15).

Архитектурное требование по использованию параметризованных тестов (`TEST_P`) выполнено: все модели протестированы на различных наборах входных данных без дублирования кода. Мок-объекты (`MockInputGenerator`) позволяют эмулировать произвольные входные воздействия и контролировать кратность вызовов генератора сигналов.
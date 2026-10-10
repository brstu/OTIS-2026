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
<p align="right">Овсянниккова Д.В.</p>
<p align="right">Проверил:</p>
<p align="right">Дворанинович Д.А.</p>
<br><br><br><br><br>
<p align="center">Брест 2026</p>

# Общее задание
1. Написать отчет по выполненной лабораторной работе №1 в .md формате (readme.md) и с помощью запроса на внесение изменений (**pull request**) разместить его в следующем каталоге: **trunk\ii0xxyy\task_01\doc** (где **xx** - номер группы, **yy** - номер студента, например **ii02302**).
2. Исходный код написанной программы разместить в каталоге: **trunk\ii0xxyy\task_01\src**.
3. Выполнить рецензирование ([review](https://linearb.io), [checklist](https://linearb.io)) запросов других студентов (минимум 2-е рецензии).
4. Отразить выполнение работы в файле readme.md в соответствующей строке (например, для студента под порядковым номером 1 - https://github.com).
5. Выходные данные должны быть представлены в табличном виде (в консоли или экспортированы в `.csv` / `.txt` файл).
6. В программе должна быть предусмотрена возможность получения нужного количества значений (настройка количества шагов симуляции $n$).
7. В программе должна быть визуализация полученных результатов (инструменты для визуализации выбрать самостоятельно: gnuplot, Python/Matplotlib, MATLAB, Excel или встроенные C++ графические библиотеки). 
8. Создать UML Диаграмму созданной программы (диаграмму классов, отражающую структуру объектно-ориентированной реализации).
9. Архитектурное требование: реализовать программу на основе принципов **ООП**. Создать базовый абстрактный класс (например, `Model`) с виртуальным методом для расчета следующего шага симуляции, а конкретные уравнения реализовать в виде классов-наследников.
10. Для тестирования объектов использовать три типа входных воздействий $u_{\tau}$: ступенчатое ($u_{\tau} = \text{const}$), импульсное ($u_0 = 1, u_{\tau > 0} = 0$) и гармоническое ( $u_{\tau} = \sin(\tau)$ ).
11. Сборка проекта должна осуществляться с помощью системы сборки CMake. Запуск сборки должен осуществляться через  `.bat` файл и должен автоматически демонстрировать прохождение процесса сборки (для уточнения или примера смотри ЛР№2)

### Дополнительное задание (Advanced Level): (Необязательное)
12. **Анализ устойчивости систем:** Для выбранных линейных дискретных моделей (из блока 1) аналитически определить критерий устойчивости (найти корни характеристического уравнения на Z-плоскости). В программе реализовать проверку: если заданные пользователем коэффициенты $a_i$ приводят к расходящемуся (неустойчивому) процессу, программа должна выводить предупреждение в консоль перед началом симуляции.

---

### Таблица вариантов 

| Вариант | Линейная модель (Блок 1) | Нелинейная модель (Блок 2) | Дифференциальное уравнение (Блок 3) |
| :---: | :---: | :---: | :---: |
| **14** | Model 1.4 | Model 2.8 | Model 3.2 |

---

## Task 1. Modeling controlled object (Extended Version)

Let's get some object to be controlled. We want to simulate its behavior and temperature transition processes using a wide range of mathematical approximations. 

The task is to write a program (**C++**) that implements **10 linear models**, **10 non-linear models**, and **10 simple differential equations**, simulates their behavior over discrete time moments $\tau$ ($1,2,3{\dots}n$), and outputs the results.

---

### 1. Linear Models (Линейные модели)

*   **Model 1.4 (Discrete Integrator with Loss):**
    $$\Large y_{\tau+1} = ay_{\tau} + b(u_{\tau} - u_{\tau-1})$$

Where $a, a_i, b, b_i$ — constant coefficients; $k$ — delay step ($k \ge 1$).

---

### 2. Non-linear Models (Нелинейные модели)  
*   **Model 2.8 (Chaotic Logistic Map Disturbance):**
    $$\Large y_{\tau+1} = ay_{\tau}(1 - y_{\tau}) + bu_{\tau} + c\sin(y_{\tau-1} \cdot u_{\tau})$$

Where $a, a_i, b, c, d, U_{max}, U_{min}, \delta, \epsilon$ — parameters and physical limits of the system.

---

### 3. Simple Differential Equations (Дифференциальные уравнения)
*Continuous equations solved via Euler's method with time step* $$\Delta t$$ *:*

*   **Model 3.2 (Pure Constant Input Drive):**
    $$\Large \frac{dy}{dt} = b \cdot u$$
    <!--
    $$ \implies y_{\tau+1} = y_{\tau} + \Delta t \cdot b u_{\tau}$$
    -->
    

Where $a, a_1, a_2, b$ — constants; $\Delta t$ — simulation time step.

# Результат



# UML-диаграмма классов

```mermaid
classDiagram
    class Model {
        <<abstract>>
        +step(double u) double*
        +reset() void*
    }

    class LinearModel {
        -a1 : double
        -a2 : double
        -b : double
        -y_prev : double
        -y_curr : double
        +LinearModel(a1, a2, b)
        +step(double u) double
        +reset() void
        +isStable() bool
    }

    class NonlinearModel {
        -a : double
        -b : double
        -y_curr : double
        +NonlinearModel(a, b)
        +step(double u) double
        +reset() void
    }

    class DifferentialModel {
        -a : double
        -b : double
        -dt : double
        -y_curr : double
        +DifferentialModel(a, b, dt)
        +step(double u) double
        +reset() void
    }

    Model <|-- LinearModel
    Model <|-- NonlinearModel
    Model <|-- DifferentialModel
```

## Описание

- **Model** — абстрактный базовый класс с чисто виртуальными методами `step` и `reset`.
- **LinearModel** — реализация линейной модели 1.3 + проверка устойчивости.
- **NonlinearModel** — реализация нелинейной модели 2.5.
- **DifferentialModel** — реализация дифференциальной модели 3.7 методом Эйлера.

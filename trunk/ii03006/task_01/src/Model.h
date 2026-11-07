#pragma once

class Model {
private:
    double state_value{0.0};

public:
    Model() = default;
    explicit Model(double initial_state) : state_value(initial_state) {}
    virtual ~Model() = default;

    virtual double step(double input_u) = 0;

    [[nodiscard]] double currentState() const noexcept {
        return state_value;
    }

    void updateState(double next_state) noexcept {
        state_value = next_state;
    }

    virtual void reset(double initial_state = 0.0) {
        state_value = initial_state;
    }
};
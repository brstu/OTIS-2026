#pragma once

class Model {
private:
    double state_y;

protected:
    auto retrieveState() const -> double {
        return state_y;
    }

    auto assignState(double new_state) -> void {
        state_y = new_state;
    }

public:
    explicit Model(double initial_state = 0.0) : state_y(initial_state) {}
    virtual ~Model() = default;

    virtual auto step(double input_signal) -> double = 0;
    virtual auto reset(double initial_state = 0.0) -> void {
        state_y = initial_state;
    }
};
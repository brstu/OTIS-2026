#pragma once
// Mock objects (Google Mock) used to replace the input signal and the model in the tests.
#include <string>

#include <gmock/gmock.h>

#include "models.h"
#include "signals.h"

// Fake input signal: the test decides what the signal returns and checks how it was called.
class MockInputSignal : public InputSignal {
public:
    MOCK_METHOD(double, value, (int tau, double t), (const, override));
    MOCK_METHOD(std::string, name, (), (const, override));
    MOCK_METHOD(std::string, tag, (), (const, override));
};

// Fake model: lets the test check which input values simulate() passes to nextStep().
class MockModel : public Model {
public:
    MockModel() {
        ON_CALL(*this, timeStep()).WillByDefault(::testing::Return(1.0));
    }

    MOCK_METHOD(double, nextStep, (double u), (override));
    MOCK_METHOD(double, current, (), (const, override));
    MOCK_METHOD(std::string, name, (), (const, override));
    MOCK_METHOD(std::string, tag, (), (const, override));
    MOCK_METHOD(std::string, equation, (), (const, override));
    MOCK_METHOD(std::string, params, (), (const, override));
    MOCK_METHOD(double, timeStep, (), (const, override));
};

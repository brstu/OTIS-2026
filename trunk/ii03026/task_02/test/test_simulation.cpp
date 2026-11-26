#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include "Model.h"
#include "Models.h"
#include "Models.h"

namespace fs = std::filesystem;
using ::testing::_;
using ::testing::AtLeast;
using ::testing::Return;

class MockModel : public Model {
public:
    MOCK_METHOD(double, calculateNextStep, (double u, double y_prev), (override));
    MOCK_METHOD(bool, isStable, (), (const, override));
    MOCK_METHOD(void, printStabilityInfo, (), (const, override));
};

namespace {
std::string findGeneratedFile(const std::string& baseName) {
    if (fs::exists(baseName)) {
        return baseName;
    }
    const std::string customName = baseName + "_custom.csv";
    if (fs::exists(customName)) {
        return customName;
    }
    const std::string harmonicName = baseName + "_harmonic.csv";
    if (fs::exists(harmonicName)) {
        return harmonicName;
    }
    for (const auto& entry : fs::directory_iterator(".")) {
        if (!entry.is_regular_file()) {
            continue;
        }
        const std::string name = entry.path().filename().string();
        if (name.rfind(baseName, 0) == 0) {
            return entry.path().string();
        }
    }
    return {};
}

void cleanupGeneratedFiles(const std::string& baseName) {
    for (const auto& entry : fs::directory_iterator(".")) {
        if (!entry.is_regular_file()) {
            continue;
        }
        const std::string name = entry.path().filename().string();
        if (name.rfind(baseName, 0) == 0) {
            std::error_code ec;
            fs::remove(entry.path(), ec);
        }
    }
}
} // namespace

TEST(MockModelTest, StepInput) {
    MockModel model;
    EXPECT_CALL(model, calculateNextStep(_, _))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(20.0));

    const std::string baseName = "test_step_output.csv";
    cleanupGeneratedFiles(baseName);

    runSimulation(model, baseName, 1, 5, 10.0);

    const std::string actualFile = findGeneratedFile(baseName);
    ASSERT_FALSE(actualFile.empty());

    std::ifstream file(actualFile);
    ASSERT_TRUE(file.is_open());
    std::string firstLine;
    std::getline(file, firstLine);
    EXPECT_FALSE(firstLine.empty());
    file.close();

    cleanupGeneratedFiles(baseName);
}

TEST(MockModelTest, PulseInput) {
    MockModel model;
    EXPECT_CALL(model, calculateNextStep(_, _))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(5.0));

    const std::string baseName = "test_pulse_output.csv";
    cleanupGeneratedFiles(baseName);

    runSimulation(model, baseName, 2, 5, 10.0);

    const std::string actualFile = findGeneratedFile(baseName);
    ASSERT_FALSE(actualFile.empty());

    std::ifstream file(actualFile);
    ASSERT_TRUE(file.is_open());
    file.close();

    cleanupGeneratedFiles(baseName);
}

TEST(MockModelTest, HarmonicInput) {
    MockModel model;
    EXPECT_CALL(model, calculateNextStep(_, _))
        .Times(3)
        .WillRepeatedly(Return(1.0));

    const std::string baseName = "test_harmonic_output.csv";
    cleanupGeneratedFiles(baseName);

    runSimulation(model, baseName, 3, 5, 10.0);

    const std::string actualFile = findGeneratedFile(baseName);
    ASSERT_FALSE(actualFile.empty());

    std::ifstream file(actualFile);
    ASSERT_TRUE(file.is_open());
    file.close();

    cleanupGeneratedFiles(baseName);
}

TEST(SimulationIntegrationTest, LinearModelStepSimulation) {
    LinearModel1_1 model(0.5, 0.5);
    const std::string baseName = "test_linear_sim.csv";
    cleanupGeneratedFiles(baseName);

    runSimulation(model, baseName, 1, 3, 2.0);

    const std::string actualFile = findGeneratedFile(baseName);
    ASSERT_FALSE(actualFile.empty());

    std::ifstream file(actualFile);
    ASSERT_TRUE(file.is_open());

    std::string line;
    std::getline(file, line);
    EXPECT_FALSE(line.empty());

    file.close();
    cleanupGeneratedFiles(baseName);
}
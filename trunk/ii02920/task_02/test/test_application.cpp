#include "application.hpp"
#include <gtest/gtest.h>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <random>
#include <sstream>

namespace {
class ApplicationTest : public testing::Test {
protected:
    std::filesystem::path dir, csv;
    std::ostringstream out, err;
    void SetUp() override {
        const auto time = std::chrono::steady_clock::now().time_since_epoch().count();
        dir = std::filesystem::temp_directory_path() /
              ("otis25-" + std::to_string(time) + "-" + std::to_string(std::random_device{}()));
        ASSERT_TRUE(std::filesystem::create_directory(dir));
        csv = dir / "result.csv";
    }
    void TearDown() override { std::filesystem::remove_all(dir); }
    int run(const std::string& data) {
        std::istringstream in(data);
        return runApplication(in, out, err, csv);
    }
    std::string csvText() const {
        std::ifstream file(csv);
        std::ostringstream text;
        text << file.rdbuf();
        return text.str();
    }
};

TEST_F(ApplicationTest, DefaultsRunTwentySteps) {
    EXPECT_EQ(run(std::string(8, '\n')), 0);
    EXPECT_NE(out.str().find("Model 1.10"), std::string::npos);
    EXPECT_NE(out.str().find("Signal: Step"), std::string::npos);
    EXPECT_EQ(out.str().find("[WARN]"), std::string::npos);
    std::istringstream lines(csvText());
    int count = 0;
    for (std::string line; std::getline(lines, line);) ++count;
    EXPECT_EQ(count, 22);  // Заголовок и состояния y0 ... y20.
}
TEST_F(ApplicationTest, BoundaryWarningPrecedesCancellationAndKeepsPreviousCsv) {
    std::ofstream(csv) << "previous run\n";
    EXPECT_EQ(run("1\n-1\n0\n0\n2\nn\n"), 0);
    EXPECT_NE(out.str().find("|z| = 1"), std::string::npos);
    EXPECT_EQ(out.str().find("Choose input signal"), std::string::npos);
    EXPECT_EQ(csvText(), "previous run\n");
}
TEST_F(ApplicationTest, UnstableWarningPrecedesAnApprovedRun) {
    EXPECT_EQ(run("1\n2\n0\n0\n1\ny\n2\n3\n"), 0);
    ASSERT_NE(out.str().find("|z| > 1"), std::string::npos);
    ASSERT_NE(out.str().find("Choose input signal"), std::string::npos);
    EXPECT_LT(out.str().find("|z| > 1"), out.str().find("Choose input signal"));
    EXPECT_EQ(csvText(), "tau,u_tau,y_tau\n0,1,1\n1,0,2\n2,0,4\n3,0,8\n");
}
TEST_F(ApplicationTest, NonlinearModelAcceptsHarmonicInput) {
    EXPECT_EQ(run("2\n0.5\n1\n0\n3\n3\n"), 0);
    EXPECT_NE(out.str().find("Model 2.5"), std::string::npos);
    EXPECT_NE(out.str().find("Signal: Harmonic"), std::string::npos);
    EXPECT_NE(csvText().find("2,0.909"), std::string::npos);
}
TEST_F(ApplicationTest, EulerRetriesInvalidSelectionsAndStep) {
    EXPECT_EQ(run("0\n3\nx\n1\n0.5\n0\n0.5\n0\n2\n1x\n2\n"), 0);
    EXPECT_NE(out.str().find("Invalid value"), std::string::npos);
    EXPECT_EQ(csvText(), "tau,u_tau,y_tau\n0,1,0\n1,0,0.75\n2,0,0.625\n");
}
TEST_F(ApplicationTest, EndOfInputReturnsError) {
    EXPECT_EQ(run("1\n"), 1);
    EXPECT_NE(err.str().find("Input ended"), std::string::npos);
    EXPECT_FALSE(std::filesystem::exists(csv));
}
TEST_F(ApplicationTest, RejectsUnrepresentableFinalTime) {
    EXPECT_EQ(run("3\n0\n0\n1e308\n0\nY\n2\n4\n"), 1);
    EXPECT_NE(err.str().find("Final time"), std::string::npos);
    EXPECT_FALSE(std::filesystem::exists(csv));
}
TEST_F(ApplicationTest, ReportsCsvOpenFailure) {
    csv = dir; // Невозможно открыть существующий каталог как файл.
    EXPECT_EQ(run(std::string(8, '\n')), 1);
    EXPECT_NE(err.str().find("Cannot create"), std::string::npos);
}
TEST_F(ApplicationTest, StopsBeforeWritingAnInfiniteOutput) {
    EXPECT_EQ(run("1\n2\n0\n0\n1e308\ny\n2\n3\n"), 1);
    EXPECT_NE(err.str().find("overflow at step 1"), std::string::npos);
    EXPECT_NE(csvText().find("0,1,1e+308"), std::string::npos);
    EXPECT_EQ(csvText().find("\n1,"), std::string::npos);
}
}  // namespace

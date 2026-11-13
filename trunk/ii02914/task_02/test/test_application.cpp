// Ввод, отчёты, CSV и сценарий программы целиком (через stringstream).
#include <gtest/gtest.h>

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <limits>
#include <sstream>
#include <string>

#include "Application.h"
#include "ConsoleInput.h"
#include "Report.h"
#include "Simulation.h"

namespace {
std::string tempPath(const std::string& name) {
    return (std::filesystem::temp_directory_path() / name).string();
}
std::string slurp(const std::string& path) {
    std::ifstream f(path);
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}
bool contains(const std::string& s, const std::string& part) {
    return s.find(part) != std::string::npos;
}
}  // namespace

// =============== ConsoleInput ===============
TEST(ReadDouble, ParsesNumber) {
    std::istringstream in("3.14\n");
    std::ostringstream out;
    EXPECT_DOUBLE_EQ(readDouble(in, out, "x", 0.0), 3.14);
}

TEST(ReadDouble, EmptyLineGivesDefault) {
    std::istringstream in("\n");
    std::ostringstream out;
    EXPECT_DOUBLE_EQ(readDouble(in, out, "x", 7.5), 7.5);
    EXPECT_TRUE(contains(out.str(), "default 7.5"));
}

TEST(ReadDouble, GarbageGivesDefaultAndMessage) {
    for (const char* bad : {"abc\n", "1.5abc\n", "nan\n", "inf\n", "1e999\n"}) {
        std::istringstream in(bad);
        std::ostringstream out;
        EXPECT_DOUBLE_EQ(readDouble(in, out, "x", 2.0), 2.0) << bad;
        EXPECT_TRUE(contains(out.str(), "Failed to parse")) << bad;
    }
}

TEST(ReadDouble, ThrowsOnEof) {
    std::istringstream in("");
    std::ostringstream out;
    EXPECT_THROW(readDouble(in, out, "x", 0.0), InputClosed);
}

TEST(ReadInt, ParsesDefaultsAndRejects) {
    std::ostringstream out;
    { std::istringstream in("42\n"); EXPECT_EQ(readInt(in, out, "n", 1), 42); }
    { std::istringstream in("\n");   EXPECT_EQ(readInt(in, out, "n", 20), 20); }
    { std::istringstream in("4.5\n"); EXPECT_EQ(readInt(in, out, "n", 20), 20); }
    { std::istringstream in("zzz\n"); EXPECT_EQ(readInt(in, out, "n", 3), 3); }
    { std::istringstream in("99999999999\n"); EXPECT_EQ(readInt(in, out, "n", 5), 5); }
    { std::istringstream in(""); EXPECT_THROW(readInt(in, out, "n", 1), InputClosed); }
}

TEST(ReadChoice, AcceptsValueInRange) {
    std::istringstream in("2\n");
    std::ostringstream out;
    EXPECT_EQ(readChoice(in, out, "?", 1, 3), 2);
}

TEST(ReadChoice, RepeatsAfterInvalidAndOutOfRange) {
    std::istringstream in("abc\n0\n4\n3\n");
    std::ostringstream out;
    EXPECT_EQ(readChoice(in, out, "?", 1, 3), 3);
    EXPECT_TRUE(contains(out.str(), "Invalid input"));
    EXPECT_TRUE(contains(out.str(), "between 1 and 3"));
}

TEST(ReadChoice, BoundariesAreInclusive) {
    std::ostringstream out;
    { std::istringstream in("1\n"); EXPECT_EQ(readChoice(in, out, "?", 1, 3), 1); }
    { std::istringstream in("3\n"); EXPECT_EQ(readChoice(in, out, "?", 1, 3), 3); }
}

TEST(ReadChoice, ThrowsInsteadOfLoopingForeverOnEof) {
    std::istringstream in("bad\n");
    std::ostringstream out;
    EXPECT_THROW(readChoice(in, out, "?", 1, 3), InputClosed);
}

// =============== Report ===============
TEST(PrintTable, ContainsHeaderAndFormattedValues) {
    std::ostringstream out;
    printTable(out, {{1, 1.0, 0.4}, {2, 1.0, 0.8}});
    const std::string s = out.str();
    EXPECT_TRUE(contains(s, "tau"));
    EXPECT_TRUE(contains(s, "u_tau"));
    EXPECT_TRUE(contains(s, "y_tau"));
    EXPECT_TRUE(contains(s, "0.40000"));
    EXPECT_TRUE(contains(s, "0.80000"));
}

TEST(SaveCsv, WritesHeaderAndRows) {
    const std::string path = tempPath("otis_test_ok.csv");
    ASSERT_TRUE(saveCsv(path, {{1, 1.0, 0.4}, {2, 1.0, 0.8}}));
    EXPECT_EQ(slurp(path), "tau,u_tau,y_tau\n1,1,0.4\n2,1,0.8\n");
    std::remove(path.c_str());
}

TEST(SaveCsv, ReturnsFalseForUnwritablePath) {
    EXPECT_FALSE(saveCsv(tempPath("no_such_dir_otis/sub/file.csv"), {{1, 1.0, 0.4}}));
}

// =============== makeSignal / makeModel ===============
TEST(MakeSignal, CreatesThreeKindsAndRejectsOthers) {
    EXPECT_NE(makeSignal(1)->name().find("Step"), std::string::npos);
    EXPECT_NE(makeSignal(2)->name().find("Impulse"), std::string::npos);
    EXPECT_NE(makeSignal(3)->name().find("Harmonic"), std::string::npos);
    EXPECT_EQ(makeSignal(0), nullptr);
    EXPECT_EQ(makeSignal(4), nullptr);
}

TEST(MakeModel, BuildsEachModelFromInput) {
    std::ostringstream out;
    { std::istringstream in("0.5\n0.4\n0.2\n0.1\n");
      auto m = makeModel(1, in, out); ASSERT_NE(m, nullptr);
      EXPECT_NEAR(m->next(1.0), 0.4, 1e-12); }
    { std::istringstream in("0.6\n0.5\n");
      auto m = makeModel(2, in, out); ASSERT_NE(m, nullptr);
      EXPECT_NEAR(m->next(1.0), 0.5, 1e-12); }
    { std::istringstream in("0.8\n0.05\n");
      auto m = makeModel(3, in, out); ASSERT_NE(m, nullptr);
      EXPECT_NEAR(m->next(0.0), 0.96, 1e-12); }
}

TEST(MakeModel, UsesDefaultsOnEmptyLines) {
    std::ostringstream out;
    std::istringstream in("\n\n\n\n");
    auto m = makeModel(1, in, out);  // a=0.5, b=0.4/0.2/0.1
    EXPECT_NEAR(m->next(1.0), 0.4, 1e-12);
}

TEST(MakeModel, UnknownChoiceGivesNull) {
    std::istringstream in("");
    std::ostringstream out;
    EXPECT_EQ(makeModel(0, in, out), nullptr);
    EXPECT_EQ(makeModel(4, in, out), nullptr);
}

// =============== runApplication ===============
TEST(RunApplication, Model17StepEndToEndWritesCsv) {
    const std::string csv = tempPath("otis_run_17.csv");
    std::remove(csv.c_str());
    std::istringstream in("1\n\n\n\n\n1\n5\n");  // модель 1.7 (дефолты), ступень, n=5
    std::ostringstream out;

    EXPECT_EQ(runApplication(in, out, csv), 0);

    EXPECT_TRUE(contains(out.str(), "Model 1.7"));
    EXPECT_FALSE(contains(out.str(), "WARNING"));
    EXPECT_TRUE(contains(out.str(), "1.32500"));  // y5
    EXPECT_EQ(slurp(csv), "tau,u_tau,y_tau\n1,1,0.4\n2,1,0.8\n3,1,1.1\n4,1,1.25\n5,1,1.325\n");
    std::remove(csv.c_str());
}

TEST(RunApplication, Model29AndModel31Work) {
    const std::string csv = tempPath("otis_run_other.csv");
    {
        std::istringstream in("2\n0.6\n0.5\n1\n3\n");
        std::ostringstream out;
        EXPECT_EQ(runApplication(in, out, csv), 0);
        EXPECT_TRUE(contains(out.str(), "0.98000"));
    }
    {
        std::istringstream in("3\n0.8\n0.05\n1\n3\n");
        std::ostringstream out;
        EXPECT_EQ(runApplication(in, out, csv), 0);
        EXPECT_TRUE(contains(out.str(), "0.88474"));
    }
    std::remove(csv.c_str());
}

TEST(RunApplication, MenuRepeatsOnInvalidChoicesAndUsesDefaultN) {
    const std::string csv = tempPath("otis_run_menu.csv");
    std::istringstream in("9\n3\n\n\n2\n\n");  // 9 - ошибка, 3.1 дефолты, импульс, n по умолчанию
    std::ostringstream out;
    EXPECT_EQ(runApplication(in, out, csv), 0);
    EXPECT_TRUE(contains(out.str(), "between 1 and 3"));
    EXPECT_TRUE(contains(out.str(), "Impulse"));
    const std::string text = slurp(csv);
    EXPECT_EQ(std::count(text.begin(), text.end(), '\n'), 21);  // заголовок + 20 строк (n по умолчанию)
    std::remove(csv.c_str());
}

TEST(RunApplication, UnstableModelWarnsAndContinuesOnY) {
    const std::string csv = tempPath("otis_run_unstable.csv");
    std::istringstream in("1\n1.5\n\n\n\ny\n1\n3\n");
    std::ostringstream out;
    EXPECT_EQ(runApplication(in, out, csv), 0);
    EXPECT_TRUE(contains(out.str(), "WARNING"));
    EXPECT_TRUE(contains(out.str(), "UNSTABLE"));
    // предупреждение выводится ДО таблицы результатов
    EXPECT_LT(out.str().find("WARNING"), out.str().find("tau"));
    EXPECT_TRUE(contains(slurp(csv), "tau,u_tau,y_tau"));
    std::remove(csv.c_str());
}

TEST(RunApplication, UnstableModelCanBeCancelled) {
    const std::string csv = tempPath("otis_run_cancel.csv");
    std::remove(csv.c_str());
    std::istringstream in("1\n1.5\n\n\n\nn\n");
    std::ostringstream out;
    EXPECT_EQ(runApplication(in, out, csv), 0);
    EXPECT_TRUE(contains(out.str(), "Cancelled"));
    EXPECT_FALSE(std::filesystem::exists(csv));  // симуляция не запускалась
}

TEST(RunApplication, BoundaryAEqualsOneTriggersWarning) {
    std::istringstream in("1\n1\n\n\n\nn\n");
    std::ostringstream out;
    runApplication(in, out, tempPath("otis_unused.csv"));
    EXPECT_TRUE(contains(out.str(), "boundary"));
}

TEST(RunApplication, NonPositiveNIsError) {
    std::istringstream in("1\n\n\n\n\n1\n0\n");
    std::ostringstream out;
    EXPECT_EQ(runApplication(in, out, tempPath("otis_unused.csv")), 1);
    EXPECT_TRUE(contains(out.str(), "n must be > 0"));
}

TEST(RunApplication, ClosedInputIsError) {
    std::istringstream in("1\n");  // дальше ввода нет
    std::ostringstream out;
    EXPECT_EQ(runApplication(in, out, tempPath("otis_unused.csv")), 1);
    EXPECT_TRUE(contains(out.str(), "Input closed"));
}

TEST(RunApplication, UnwritableCsvIsError) {
    std::istringstream in("1\n\n\n\n\n1\n3\n");
    std::ostringstream out;
    EXPECT_EQ(runApplication(in, out, tempPath("no_such_dir_otis/x.csv")), 1);
    EXPECT_TRUE(contains(out.str(), "Could not write"));
}

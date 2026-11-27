#include <gtest/gtest.h>

#include <cmath>
#include <complex>
#include <sstream>
#include <string>
#include <vector>
#include <array>

#include "StabilityAnalyzer.h"
#include "TestUtils.h"

namespace
{

bool isStable(const std::vector<double>& coefficients)
{
    return StabilityAnalyzer::isStable(coefficients);
}

void expectVerdict(const std::vector<double>& coefficients, StabilityVerdict expected)
{
    EXPECT_EQ(StabilityAnalyzer::schurVerdict(coefficients), expected)
        << "полином: " << [&coefficients]
           {
               std::ostringstream out;
               for (std::size_t i = 0; i < coefficients.size(); ++i)
               {
                   if (i)
                       out << " + ";
                   out << coefficients[i] << "*z^" << i;
               }
               return out.str();
           }();
}
}

TEST(StabilityRoots, FindRootsCoversAllDegrees)
{
    {
        SCOPED_TRACE("кубический: корни внутри единичного круга");

        const auto roots = StabilityAnalyzer::findRoots({-0.1, -0.2, -0.5, 1.0});
        ASSERT_EQ(roots.size(), 3u);
        for (const auto& root : roots)
            EXPECT_LT(std::abs(root), 1.0) << "|z| = " << std::abs(root);
    }
    {
        SCOPED_TRACE("корни удовлетворяют уравнению p(z) = 0");
        const auto coefficients = std::vector{-0.3, 0.17, -0.8, 1.0};
        const auto roots = StabilityAnalyzer::findRoots(coefficients);
        ASSERT_EQ(roots.size(), 3u);
        for (const auto& root : roots)
        {
            std::complex<double> value(0.0, 0.0);
            for (std::size_t i = coefficients.size(); i-- > 0;)
                value = value * root + coefficients[i];
            EXPECT_LT(std::abs(value), 1e-8);
        }
    }
    {
        SCOPED_TRACE("квадратичный: спектральный радиус равен 1");
        const auto roots = StabilityAnalyzer::findRoots({1.0, 0.0, 1.0});
        EXPECT_NEAR(StabilityAnalyzer::spectralRadius(roots), 1.0, 1e-12);
    }
    {
        SCOPED_TRACE("линейный: z = -c0 / c1");
        const auto roots = StabilityAnalyzer::findRoots({2.0, 1.0});
        ASSERT_EQ(roots.size(), 1u);
        EXPECT_LT(std::abs(roots.front() - std::complex<double>(-2.0, 0.0)), 1e-12);
    }
    {
        SCOPED_TRACE("нулевой старший коэффициент отбрасывается");
        const auto roots = StabilityAnalyzer::findRoots({1.0, 2.0, 0.0});
        ASSERT_EQ(roots.size(), 1u);
        EXPECT_LT(std::abs(roots.front() - std::complex<double>(-0.5, 0.0)), 1e-12);
    }
    {
        SCOPED_TRACE("константа не имеет корней");
        EXPECT_TRUE(StabilityAnalyzer::findRoots({5.0}).empty());
    }
    {
        SCOPED_TRACE("степень больше трех не поддерживается");
        EXPECT_TRUE(StabilityAnalyzer::findRoots({1.0, 0.0, 0.0, 0.0, 1.0}).empty());
    }
}

TEST(SchurCriterion, CoversStableUnstableMarginalAndDegenerate)
{

    expectVerdict({-0.1, -0.2, -0.5, 1.0}, StabilityVerdict::Stable);

    expectVerdict({0.0, 0.0, -2.0, 1.0}, StabilityVerdict::Unstable);

    expectVerdict({0.0, 0.0, -1.0, 1.0}, StabilityVerdict::Marginal);

    expectVerdict({0.7, 1.0}, StabilityVerdict::Stable);
    expectVerdict({-0.7, 1.0}, StabilityVerdict::Stable);
    expectVerdict({1.0, 1.0}, StabilityVerdict::Marginal);
    expectVerdict({1.5, 1.0}, StabilityVerdict::Unstable);

    expectVerdict({0.0, 0.0, 0.0, 1.0}, StabilityVerdict::Stable);
    expectVerdict({1.0}, StabilityVerdict::Stable);
    expectVerdict({5.0}, StabilityVerdict::Stable);

    expectVerdict({0.5, 1.0, 0.0}, StabilityVerdict::Stable);
}

TEST(SchurCriterion, ScalingDoesNotChangeVerdict)
{

    const auto base = std::vector{-0.1, -0.2, -0.5, 1.0};
    const auto scaled = std::vector{-10.0, -20.0, -50.0, 100.0};

    EXPECT_EQ(StabilityAnalyzer::schurVerdict(base),
              StabilityAnalyzer::schurVerdict(scaled));
}

TEST(Model16Stability, CharacteristicPolynomialAndVerdicts)
{
    {
        SCOPED_TRACE("форма характеристического полинома");
        otis_test::expectNear(StabilityAnalyzer::characteristicPolynomial1_6(0.5, 0.2, 0.1),
                              {-0.1, -0.2, -0.5, 1.0});
    }
    {
        SCOPED_TRACE("устойчивые коэффициенты");
        EXPECT_EQ(StabilityAnalyzer::analyzeModel1_6(0.5, 0.2, 0.1).verdict,
                  StabilityVerdict::Stable);
        EXPECT_TRUE(isStable(StabilityAnalyzer::characteristicPolynomial1_6(1.2, -0.3, 0.05)));
        EXPECT_TRUE(isStable(StabilityAnalyzer::characteristicPolynomial1_6(0.9, 0.0, 0.0)));
        EXPECT_TRUE(isStable(StabilityAnalyzer::characteristicPolynomial1_6(1.0, 0.0, -0.2)));
        EXPECT_TRUE(isStable(StabilityAnalyzer::characteristicPolynomial1_6(0.0, 0.0, 0.0)));
    }
    {
        SCOPED_TRACE("неустойчивые и граничные коэффициенты");

        EXPECT_FALSE(isStable(StabilityAnalyzer::characteristicPolynomial1_6(2.0, 0.0, 0.0)));

        const auto unitRoot =
            StabilityAnalyzer::characteristicPolynomial1_6(1.0, 0.0, 0.0);
        EXPECT_FALSE(isStable(unitRoot));
        EXPECT_TRUE(StabilityAnalyzer::isMarginallyStable(unitRoot));

        const auto boundary =
            StabilityAnalyzer::characteristicPolynomial1_6(0.6, 0.5, -0.1);
        EXPECT_FALSE(isStable(boundary));
        EXPECT_TRUE(StabilityAnalyzer::isMarginallyStable(boundary));
    }
}

TEST(Model16Stability, SpectralRadiusAndReport)
{
    {
        SCOPED_TRACE("спектральные радиусы (эталон numpy)");
        EXPECT_NEAR(StabilityAnalyzer::analyzeModel1_6(0.5, 0.2, 0.1).spectralRadius,
                    0.864913416, 1e-8);
        EXPECT_NEAR(StabilityAnalyzer::analyzeModel1_6(2.0, 0.0, 0.0).spectralRadius,
                    2.0, 1e-8);
        EXPECT_NEAR(StabilityAnalyzer::analyzeModel1_6(1.2, -0.3, 0.05).spectralRadius,
                    0.93671642, 1e-8);
        EXPECT_NEAR(StabilityAnalyzer::analyzeModel1_6(1.0, 0.0, -0.2).spectralRadius,
                    0.724895177, 1e-8);
    }
    {
        SCOPED_TRACE("отчет содержит данные о модели");
        const StabilityReport report = StabilityAnalyzer::analyzeModel1_6(0.5, 0.2, 0.1);
        EXPECT_EQ(report.coefficients.size(), 4u);
        EXPECT_EQ(report.roots.size(), 3u);
        EXPECT_EQ(report.verdict, StabilityVerdict::Stable);
        EXPECT_FALSE(report.modelName.empty());
    }
}

TEST(StabilityWarning, WarningNeededOnlyForNonStable)
{
    const StabilityReport stable = StabilityAnalyzer::analyzeModel1_6(0.5, 0.2, 0.1);
    const StabilityReport unstable = StabilityAnalyzer::analyzeModel1_6(2.0, 0.0, 0.0);
    const StabilityReport marginal = StabilityAnalyzer::analyzeModel1_6(1.0, 0.0, 0.0);

    EXPECT_FALSE(StabilityAnalyzer::needsWarning(stable));
    EXPECT_TRUE(StabilityAnalyzer::needsWarning(unstable));
    EXPECT_TRUE(StabilityAnalyzer::needsWarning(marginal));

    EXPECT_TRUE(StabilityAnalyzer::warningText(stable).empty());
    EXPECT_FALSE(StabilityAnalyzer::warningText(unstable).empty());
}

TEST(StabilityWarning, TextDescribesState)
{
    {
        SCOPED_TRACE("неустойчивая: WARNING + модель + радиус");
        const StabilityReport unstable = StabilityAnalyzer::analyzeModel1_6(2.0, 0.0, 0.0);
        const std::string text = StabilityAnalyzer::warningText(unstable);
        EXPECT_EQ(text.rfind("WARNING:", 0), 0u);
        EXPECT_NE(text.find("1.6"), std::string::npos);
        EXPECT_NE(text.find("|z|max"), std::string::npos);
    }
    {
        SCOPED_TRACE("граничная: упоминает границу устойчивости");
        const StabilityReport marginal = StabilityAnalyzer::analyzeModel1_6(1.0, 0.0, 0.0);
        const std::string text = StabilityAnalyzer::warningText(marginal);
        EXPECT_NE(text.find("границе устойчивости"), std::string::npos);
    }
}

TEST(StabilityWarning, PrintWarningWritesToStream)
{
    {
        SCOPED_TRACE("неустойчивая модель: печатается WARNING");
        const StabilityReport unstable = StabilityAnalyzer::analyzeModel1_6(2.0, 0.0, 0.0);
        std::ostringstream out;
        EXPECT_TRUE(StabilityAnalyzer::printWarningIfNeeded(unstable, out));
        EXPECT_NE(out.str().find("WARNING"), std::string::npos);
    }
    {
        SCOPED_TRACE("граничная модель: предупреждение упоминает границу");
        StabilityReport marginal = StabilityAnalyzer::analyzeModel1_6(1.0, 0.0, 0.0);
        marginal.verdict = StabilityVerdict::Marginal;
        std::ostringstream out;
        EXPECT_TRUE(StabilityAnalyzer::printWarningIfNeeded(marginal, out));
        EXPECT_NE(out.str().find("WARNING"), std::string::npos);
        EXPECT_NE(out.str().find("границе"), std::string::npos);
    }
    {
        SCOPED_TRACE("устойчивая модель: без WARNING, сообщение об устойчивости");
        const StabilityReport stable = StabilityAnalyzer::analyzeModel1_6(0.5, 0.2, 0.1);
        std::ostringstream out;
        EXPECT_FALSE(StabilityAnalyzer::printWarningIfNeeded(stable, out));
        EXPECT_EQ(out.str().find("WARNING"), std::string::npos);
        EXPECT_NE(out.str().find("устойчива"), std::string::npos);
    }
}

TEST(StabilityWarning, DescribeReportsCoefficientsRootsAndVerdict)
{
    {
        SCOPED_TRACE("описание устойчивой модели");
        const StabilityReport report = StabilityAnalyzer::analyzeModel1_6(0.5, 0.2, 0.1);
        const std::string text = StabilityAnalyzer::describe(report);
        EXPECT_NE(text.find("Характеристическое уравнение"), std::string::npos);
        EXPECT_NE(text.find("Корни"), std::string::npos);
        EXPECT_NE(text.find("STABLE"), std::string::npos);
        EXPECT_NE(text.find("z^3"), std::string::npos);
    }
    {
        SCOPED_TRACE("имя вердикта для каждого состояния");
        struct Case
        {
            double a1;
            double a2;
            double a3;
            StabilityVerdict verdict;
            const char* name;
        };

        const std::vector<Case> cases  = {
            {0.5, 0.2, 0.1, StabilityVerdict::Stable,   "STABLE"},
            {1.0, 0.0, 0.0, StabilityVerdict::Marginal, "MARGINAL"},
            {2.0, 0.0, 0.0, StabilityVerdict::Unstable, "UNSTABLE"},
        };

        for (const Case& item : cases)
        {
            const StabilityReport report =
                StabilityAnalyzer::analyzeModel1_6(item.a1, item.a2, item.a3);

            ASSERT_EQ(report.verdict, item.verdict)
                << "набор: a1=" << item.a1 << " a2=" << item.a2 << " a3=" << item.a3;

            const std::string text = StabilityAnalyzer::describe(report);
            EXPECT_NE(text.find(item.name), std::string::npos)
                << "describe() не вывел вердикт " << item.name;
        }
    }
    {
        SCOPED_TRACE("защитная ветка UNKNOWN в verdictName");
        StabilityReport report = StabilityAnalyzer::analyzeModel1_6(0.5, 0.2, 0.1);
        report.verdict = static_cast<StabilityVerdict>(99);
        EXPECT_NE(StabilityAnalyzer::describe(report).find("UNKNOWN"), std::string::npos);
    }
}

#include "../../src/core/include/entities/CodingTask.hpp"
#include "../../src/core/include/entities/TestCase.hpp"
#include "../../src/core/include/services/StaticAnalyzer.hpp"

#include <QtTest>

using namespace cppforge::entities;
using namespace cppforge::services;

class TestStaticAnalyzer : public QObject
{
    Q_OBJECT

private:
    std::set<TestCase> dummyTestCases;

    CodingTask createBasicTask(std::optional<std::set<QString>> whitelist, std::optional<std::set<QString>> blacklist)
    {
        return CodingTask(1, 1, "Test Task", "Desc", "int main() {}", dummyTestCases, 2000, 256, whitelist, blacklist);
    }

private slots:
    void test_NoRestrictions()
    {
        StaticAnalyzer analyzer;
        CodingTask task = createBasicTask(std::nullopt, std::nullopt);
        QString code = "int main() { return 0; }";
        auto result = analyzer.analyze(task, code);
        QVERIFY(!result.has_value());
    }

    void test_BlacklistedWordPresent()
    {
        StaticAnalyzer analyzer;
        std::set<QString> blacklist = {"system", "fork"};
        CodingTask task = createBasicTask(std::nullopt, blacklist);
        QString code = "#include <unistd.h>\nint main() { fork(); return 0; }";
        auto result = analyzer.analyze(task, code);
        QVERIFY(result.has_value());
        QVERIFY(result->contains("forbidden word"));
        QVERIFY(result->contains("fork"));
    }

    void test_WhitelistedWordMissing()
    {
        StaticAnalyzer analyzer;
        std::set<QString> whitelist = {"std::cout", "std::endl"};
        CodingTask task = createBasicTask(whitelist, std::nullopt);
        QString code = "#include <iostream>\nint main() { return 0; }";
        auto result = analyzer.analyze(task, code);
        QVERIFY(result.has_value());
        QVERIFY(result->contains("Missing required word"));
    }

    void test_WhitelistedWordPresentAndNoBlacklistHits()
    {
        StaticAnalyzer analyzer;
        std::set<QString> whitelist = {"std::cout"};
        std::set<QString> blacklist = {"printf"};
        CodingTask task = createBasicTask(whitelist, blacklist);
        QString code = "#include <iostream>\nint main() { std::cout << \"Hello\"; return 0; }";
        auto result = analyzer.analyze(task, code);
        QVERIFY(!result.has_value());
    }
};

QTEST_MAIN(TestStaticAnalyzer)

#include "test_StaticAnalyzer.moc"

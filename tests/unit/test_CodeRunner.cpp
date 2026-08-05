#include "../../src/core/include/entities/ExecutionResult.hpp"
#include "../../src/core/include/entities/TestCase.hpp"
#include "../../src/core/include/services/CodeRunner.hpp"
#include "../../src/core/include/services/DockerCodeRunner.hpp"

#include <QtTest>

using namespace cppforge::entities;
using namespace cppforge::services;

class TestCodeRunner : public QObject
{
    Q_OBJECT

private slots:
    void test_CodeRunnerDetection()
    {
        CodeRunner runner;
        qDebug() << "Is using Docker Sandbox:" << runner.isUsingDocker();
        QVERIFY(true);
    }

    void test_ValidCodeExecution()
    {
        CodeRunner runner;

        QString code = R"(
#include <iostream>
int main() {
    int a, b;
    if (std::cin >> a >> b) {
        std::cout << (a + b) << std::endl;
    }
    return 0;
}
)";

        std::vector<TestCase> testCases = {TestCase(1, "5 7", "12", false), TestCase(2, "100 200", "300", false)};

        ExecutionResult result = runner.runBlocking(code, testCases);
        qDebug() << "Output log:" << result.getOutput();
        qDebug() << "Error log:" << result.getErrors();
        qDebug() << "Passed count:" << result.getPassedTestsCount();

        QVERIFY2(result.isSuccess(), result.getErrors().toUtf8().constData());
        QCOMPARE(result.getPassedTestsCount(), static_cast<uint32_t>(2));
    }
};

QTEST_MAIN(TestCodeRunner)

#include "test_CodeRunner.moc"

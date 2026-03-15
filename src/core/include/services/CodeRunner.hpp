#pragma once

#include "../entities/ExecutionResult.hpp"
#include "../entities/TestCase.hpp"

#include <QFuture>
#include <QObject>
#include <QString>

#include <vector>

namespace cppforge::services
{
    /**
     * @brief Responsible for compiling and executing user-submitted code against test cases.
     * 
     * CodeRunner provides both synchronous and asynchronous APIs to evaluate source code.
     * It handles the temporary file creation, compilation via system tools, and 
     * captured execution output comparison.
     */
    class CodeRunner : public QObject
    {
        Q_OBJECT

    public:
        /**
         * @brief Constructs a new CodeRunner instance.
         * @param parent The optional parent QObject.
         */
        explicit CodeRunner(QObject *parent = nullptr);

        /**
         * @brief Destructor for CodeRunner.
         */
        ~CodeRunner() override;

        /**
         * @brief Evaluates code against a set of tests in a background thread.
         * 
         * @param code The source code text to be evaluated.
         * @param tests A collection of test cases defining expected behavior.
         * @return A QFuture that will contain the final ExecutionResult when finished.
         */
        QFuture<cppforge::entities::ExecutionResult> runAsync(const QString &code,
                                                              const std::vector<cppforge::entities::TestCase> &tests);

        /**
         * @brief Evaluates code against a set of tests, blocking the current thread.
         * 
         * @param code The source code text to be evaluated.
         * @param tests A collection of test cases defining expected behavior.
         * @return The final ExecutionResult report.
         */
        cppforge::entities::ExecutionResult runBlocking(const QString &code,
                                                        const std::vector<cppforge::entities::TestCase> &tests);

    private:
        /**
         * @brief Compiles the given code and returns the path to the resulting executable.
         * @param code raw source code.
         * @return QString path to executable on success; throws or returns empty on error.
         */
        QString compileCodeBlocking(const QString &code);

        /**
         * @brief Executes a compiled binary against the provided test scenarios.
         * @param executablePath the path to the binary to run.
         * @param tests the test suite to execute.
         * @return A consolidated ExecutionResult.
         */
        cppforge::entities::ExecutionResult runTestsBlocking(const QString &executablePath,
                                                              const std::vector<cppforge::entities::TestCase> &tests);
    };
} // namespace cppforge::services

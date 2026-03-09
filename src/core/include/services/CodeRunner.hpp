#pragma once

#include "../entities/ExecutionResult.hpp"
#include "../entities/TestCase.hpp"

#include <QFuture>
#include <QObject>
#include <QString>

#include <vector>

namespace cppforge
{
    namespace services
    {
        class CodeRunner : public QObject
        {
            Q_OBJECT

        public:
            explicit CodeRunner(QObject *parent = nullptr);
            ~CodeRunner() override;

            QFuture<cppforge::entities::ExecutionResult>
            runAsync(const QString &code, const std::vector<cppforge::entities::TestCase> &tests);

            cppforge::entities::ExecutionResult runBlocking(const QString &code,
                                                            const std::vector<cppforge::entities::TestCase> &tests);

        private:
            QString compileCodeBlocking(const QString &code);
            cppforge::entities::ExecutionResult
            runTestsBlocking(const QString &executablePath, const std::vector<cppforge::entities::TestCase> &tests);
        };
    } // namespace services
} // namespace cppforge

#pragma once

#include "ICodeExecutionEngine.hpp"
#include "../entities/ExecutionResult.hpp"
#include "../entities/TestCase.hpp"

#include <QFuture>
#include <QObject>
#include <QString>

#include <memory>
#include <vector>

namespace cppforge::services
{
    /**
     * @brief High-level facade for evaluating student C/C++ source code.
     *
     * Delegates code execution to a secure Docker sandbox engine when available,
     * or falls back to local execution.
     */
    class CodeRunner : public QObject
    {
        Q_OBJECT

    public:
        explicit CodeRunner(QObject *parent = nullptr);
        explicit CodeRunner(std::unique_ptr<ICodeExecutionEngine> engine, QObject *parent = nullptr);
        ~CodeRunner() override;

        QFuture<entities::ExecutionResult> runAsync(const QString &code,
                                                   const std::vector<entities::TestCase> &tests);

        entities::ExecutionResult runBlocking(const QString &code,
                                               const std::vector<entities::TestCase> &tests);

        /**
         * @brief Checks if Docker sandbox mode is active.
         */
        bool isUsingDocker() const;

    private:
        std::unique_ptr<ICodeExecutionEngine> m_engine;
        bool m_isUsingDocker{false};
    };
} // namespace cppforge::services

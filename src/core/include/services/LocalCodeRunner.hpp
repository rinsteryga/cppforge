#pragma once

#include "ICodeExecutionEngine.hpp"

#include <QObject>

namespace cppforge::services
{
    /**
     * @brief Fallback execution engine that compiles and runs code using local host environment toolchain.
     */
    class LocalCodeRunner : public ICodeExecutionEngine
    {
    public:
        LocalCodeRunner() = default;
        ~LocalCodeRunner() override = default;

        entities::ExecutionResult runBlocking(const QString &code,
                                              const std::vector<entities::TestCase> &tests) override;

        bool isAvailable() const override;

    private:
        QString compileCodeBlocking(const QString &code);
        entities::ExecutionResult runTestsBlocking(const QString &executablePath,
                                                   const std::vector<entities::TestCase> &tests);
    };
} // namespace cppforge::services

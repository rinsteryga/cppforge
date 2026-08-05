#pragma once

#include "../entities/ExecutionResult.hpp"
#include "../entities/TestCase.hpp"

#include <QString>

#include <vector>

namespace cppforge::services
{
    /**
     * @brief Abstract interface for code execution engines (Docker sandbox, local compiler fallback, etc.).
     */
    class ICodeExecutionEngine
    {
    public:
        virtual ~ICodeExecutionEngine() = default;

        /**
         * @brief Evaluates code against a set of test cases synchronously.
         *
         * @param code Source code to execute.
         * @param tests Test suite to evaluate against.
         * @return ExecutionResult containing passed tests count, errors, and output.
         */
        virtual entities::ExecutionResult runBlocking(const QString &code,
                                                      const std::vector<entities::TestCase> &tests) = 0;

        /**
         * @brief Checks whether this execution engine is available and ready on the current environment.
         * @return true if available, false otherwise.
         */
        virtual bool isAvailable() const = 0;
    };
} // namespace cppforge::services

#pragma once

#include "ICodeExecutionEngine.hpp"

#include <QString>

namespace cppforge::services
{
    /**
     * @brief Secure code execution engine that compiles and runs student code inside Docker containers.
     */
    class DockerCodeRunner : public ICodeExecutionEngine
    {
    public:
        /**
         * @brief Constructs DockerCodeRunner with target Docker image tag.
         * @param imageName Docker image identifier (default: "cppforge-runner:latest").
         */
        explicit DockerCodeRunner(QString imageName = "cppforge-runner:latest");
        ~DockerCodeRunner() override = default;

        entities::ExecutionResult runBlocking(const QString &code,
                                              const std::vector<entities::TestCase> &tests) override;

        bool isAvailable() const override;

    private:
        QString m_imageName;

        QString compileInDocker(const QString &workDir);
        entities::ExecutionResult runTestsInDocker(const QString &workDir,
                                                   const std::vector<entities::TestCase> &tests);
    };
} // namespace cppforge::services

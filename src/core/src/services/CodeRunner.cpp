#include "../../include/services/CodeRunner.hpp"

#include "../../include/entities/ExecutionResult.hpp"
#include "../../include/entities/TestCase.hpp"
#include "../../include/services/DockerCodeRunner.hpp"
#include "../../include/services/LocalCodeRunner.hpp"

#include <QDebug>
#include <QtConcurrent>

namespace cppforge::services
{
    CodeRunner::CodeRunner(QObject *parent) : QObject(parent)
    {
        auto dockerEngine = std::make_unique<DockerCodeRunner>();
        if (dockerEngine->isAvailable())
        {
            qDebug() << "[CodeRunner] Docker daemon detected. Using Docker Sandbox Engine.";
            m_engine = std::move(dockerEngine);
            m_isUsingDocker = true;
        }
        else
        {
            qDebug() << "[CodeRunner] Docker daemon not found or inactive. Falling back to Local Code Engine.";
            m_engine = std::make_unique<LocalCodeRunner>();
            m_isUsingDocker = false;
        }
    }

    CodeRunner::CodeRunner(std::unique_ptr<ICodeExecutionEngine> engine, QObject *parent)
        : QObject(parent), m_engine(std::move(engine))
    {
        m_isUsingDocker = (dynamic_cast<DockerCodeRunner *>(m_engine.get()) != nullptr);
    }

    CodeRunner::~CodeRunner() = default;

    bool CodeRunner::isUsingDocker() const
    {
        return m_isUsingDocker;
    }

    QFuture<entities::ExecutionResult> CodeRunner::runAsync(const QString &code,
                                                            const std::vector<entities::TestCase> &tests)
    {
        return QtConcurrent::run([this, code, tests] { return runBlocking(code, tests); });
    }

    entities::ExecutionResult CodeRunner::runBlocking(const QString &code, const std::vector<entities::TestCase> &tests)
    {
        if (!m_engine)
        {
            return entities::ExecutionResult(0, false, "", "No execution engine configured", 0, 0);
        }

        return m_engine->runBlocking(code, tests);
    }
} // namespace cppforge::services

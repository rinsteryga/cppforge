#include "../../include/services/LocalCodeRunner.hpp"

#include "../../include/entities/ExecutionResult.hpp"
#include "../../include/entities/TestCase.hpp"
#include "../../include/services/CppToolchain.hpp"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QProcessEnvironment>
#include <QUuid>

#include <stdexcept>

namespace cppforge::services
{
    bool LocalCodeRunner::isAvailable() const
    {
        return CppToolchain::detected().isAvailable();
    }

    entities::ExecutionResult LocalCodeRunner::runBlocking(const QString &code,
                                                           const std::vector<entities::TestCase> &tests)
    {
        try
        {
            const QString executablePath = compileCodeBlocking(code);
            return runTestsBlocking(executablePath, tests);
        }
        catch (const std::runtime_error &e)
        {
            return entities::ExecutionResult(0, false, "", QString::fromUtf8(e.what()), 0, 0);
        }
    }

    QString LocalCodeRunner::compileCodeBlocking(const QString &code)
    {
        const CppToolchain &toolchain = CppToolchain::detected();
        if (!toolchain.isAvailable())
        {
            throw std::runtime_error("No C++ compiler found. Install Visual Studio Build Tools or GCC/Clang, or set "
                                     "CPPFORGE_CXX.");
        }

        const QString sourcePath =
            QDir::tempPath() + "/cppforge_" + QUuid::createUuid().toString(QUuid::WithoutBraces) + ".cpp";
        QFile sourceFile(sourcePath);
        if (!sourceFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            throw std::runtime_error("Cannot create temporary source file for local compilation");
        }
        sourceFile.write(code.toUtf8());
        sourceFile.close();

        const QString executablePath = sourcePath + CppToolchain::executableSuffix();

        QProcess compiler;
        compiler.setProcessEnvironment(toolchain.environment());
        compiler.setWorkingDirectory(QFileInfo(sourcePath).absolutePath());
        compiler.start(toolchain.compilerPath(), toolchain.compileArguments(sourcePath, executablePath));

        const bool finished = compiler.waitForFinished(30000);
        const bool compiled = finished && compiler.exitCode() == 0;
        QString errorLog = QString::fromLocal8Bit(compiler.readAllStandardError());
        if (errorLog.trimmed().isEmpty())
        {
            errorLog = QString::fromLocal8Bit(compiler.readAllStandardOutput());
        }

        QFile::remove(sourcePath);
        QFile::remove(QFileInfo(sourcePath).absolutePath() + "/" + QFileInfo(sourcePath).completeBaseName() + ".obj");

        if (!compiled)
        {
            QFile::remove(executablePath);
            if (errorLog.trimmed().isEmpty())
            {
                errorLog = finished ? "Compiler exited with code " + QString::number(compiler.exitCode())
                                    : "Compilation timed out";
            }
            throw std::runtime_error("Compilation Error:\n" + errorLog.toStdString());
        }

        return executablePath;
    }

    entities::ExecutionResult LocalCodeRunner::runTestsBlocking(const QString &executablePath,
                                                                const std::vector<entities::TestCase> &tests)
    {
        uint32_t passedCount = 0;
        QString lastOutput;
        QString errorLog;
        const QProcessEnvironment environment = CppToolchain::detected().environment();

        if (tests.empty())
        {
            QProcess process;
            process.setProcessEnvironment(environment);
            process.setProcessChannelMode(QProcess::MergedChannels);
            process.start(executablePath, QStringList());

            if (process.waitForFinished(3000))
            {
                lastOutput = QString::fromLocal8Bit(process.readAll().trimmed());
            }
            else
            {
                process.kill();
                errorLog = "Time Limit Exceeded (No tests provided)";
            }
        }
        else
        {
            for (const auto &test : tests)
            {
                QProcess process;
                process.setProcessEnvironment(environment);
                process.setProcessChannelMode(QProcess::MergedChannels);
                process.start(executablePath, QStringList());

                if (!test.getInput().isEmpty())
                {
                    process.write(test.getInput().toUtf8());
                    process.closeWriteChannel();
                }

                if (process.waitForFinished(2000))
                {
                    QString output = QString::fromLocal8Bit(process.readAll().trimmed()).replace("\r\n", "\n");
                    QString expected = test.getExpectedOutput().trimmed().replace("\r\n", "\n");

                    if (output == expected)
                    {
                        passedCount++;
                    }
                    lastOutput = output;
                }
                else
                {
                    process.kill();
                    lastOutput = "TLE (Time Limit Exceeded)";
                    break;
                }
            }
        }

        QFile::remove(executablePath);

        bool isAllPassed = (!tests.empty() && passedCount == tests.size()) || (tests.empty() && errorLog.isEmpty());

        return entities::ExecutionResult(0, isAllPassed, lastOutput, errorLog, 0, passedCount);
    }
} // namespace cppforge::services

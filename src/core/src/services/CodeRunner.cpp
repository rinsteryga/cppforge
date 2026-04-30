#include "../../include/services/CodeRunner.hpp"

#include "../../include/entities/ExecutionResult.hpp"
#include "../../include/entities/TestCase.hpp"

#include <QDir>
#include <QFile>
#include <QProcess>
#include <QTemporaryFile>
#include <QtConcurrent>

namespace cppforge::services
{
    CodeRunner::CodeRunner(QObject *parent) : QObject(parent) {}
    CodeRunner::~CodeRunner() = default;

    QFuture<cppforge::entities::ExecutionResult>
    CodeRunner::runAsync(const QString &code, const std::vector<cppforge::entities::TestCase> &tests)
    {
        return QtConcurrent::run([this, code, tests] { return runBlocking(code, tests); });
    }

    cppforge::entities::ExecutionResult CodeRunner::runBlocking(const QString &code,
                                                                const std::vector<cppforge::entities::TestCase> &tests)
    {
        try
        {
            QString exePath = compileCodeBlocking(code);
            return runTestsBlocking(exePath, tests);
        }
        catch (const std::runtime_error &e)
        {
            return cppforge::entities::ExecutionResult(0, false, "", QString::fromUtf8(e.what()), 0, 0);
        }
    }

    QString CodeRunner::compileCodeBlocking(const QString &code)
    {
        QTemporaryFile sourceFile(QDir::tempPath() + "/cppforge_XXXXXX.cpp");
        sourceFile.setAutoRemove(false);
        if (!sourceFile.open())
        {
            throw std::runtime_error("can not create temporary file");
        }

        sourceFile.write(code.toUtf8());
        QString sourcePath = sourceFile.fileName();
        sourceFile.close();

        QString exePath = sourcePath + ".exe";

        QProcess compiler;
        compiler.start("g++", {sourcePath, "-o", exePath, "-O2"});

        if (!compiler.waitForFinished(10000) || compiler.exitCode() != 0)
        {
            QString errorLog = QString::fromLocal8Bit(compiler.readAllStandardError());
            QFile::remove(sourcePath);
            throw std::runtime_error("Compilation Error:\n" + errorLog.toStdString());
        }

        QFile::remove(sourcePath);
        return exePath;
    }

    cppforge::entities::ExecutionResult
    CodeRunner::runTestsBlocking(const QString &executablePath, const std::vector<cppforge::entities::TestCase> &tests)
    {
        uint32_t passedCount = 0;
        QString lastOutput;
        QString errorLog;

        if (tests.empty())
        {
            QProcess process;
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

        return cppforge::entities::ExecutionResult(0, isAllPassed, lastOutput, errorLog, 0, passedCount);
    }
} // namespace cppforge::services
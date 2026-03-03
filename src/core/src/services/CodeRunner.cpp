#include "../../include/services/CodeRunner.hpp"

#include <QDebug>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QProcess>
#include <QStringBuilder>
#include <QTextStream>
#include <QUuid>
#include <QtConcurrent>

namespace cppforge::services
{
    CodeRunner::CodeRunner(QObject *parent) : QObject(parent) {}

    CodeRunner::~CodeRunner() = default;

    QFuture<cppforge::entities::ExecutionResult>
    CodeRunner::runAsync(const QString &code, const std::vector<cppforge::entities::TestCase> &tests)
    {
        return QtConcurrent::run([this, code, tests]() { return this->runBlocking(code, tests); });
    }

    cppforge::entities::ExecutionResult CodeRunner::runBlocking(const QString &code,
                                                                const std::vector<cppforge::entities::TestCase> &tests)
    {
        QString executablePath = compileCodeBlocking(code);

        if (executablePath.isEmpty())
        {
            return cppforge::entities::ExecutionResult(0, // placeholder
                                                       false, "", "Compilation failed", 0, 0);
        }

        auto result = runTestsBlocking(executablePath, tests);

        QFile::remove(executablePath);

        return result;
    }

    QString CodeRunner::compileCodeBlocking(const QString &code)
    {
        QDir tempDir = QDir::temp();
        tempDir.mkdir("cppforge_runs");
        tempDir.cd("cppforge_runs");

        QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
        QString sourceFilePath = tempDir.absoluteFilePath(uuid + ".cpp");
#ifdef Q_OS_WIN
        QString executablePath = tempDir.absoluteFilePath(uuid + ".exe");
#else
        QString executablePath = tempDir.absoluteFilePath(uuid);
#endif

        QFile sourceFile(sourceFilePath);
        if (!sourceFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qWarning() << "CodeRunner: Failed to create source file" << sourceFilePath;
            return QString();
        }

        QTextStream out(&sourceFile);
        out << code;
        sourceFile.close();

        QProcess compilerProcess;
        QStringList arguments;
        arguments << "-O2" << "-Wall" << "-std=c++20" << "-o" << executablePath << sourceFilePath;

        compilerProcess.start("g++", arguments);

        if (!compilerProcess.waitForStarted())
        {
            qWarning() << "CodeRunner: Failed to start g++";
            QFile::remove(sourceFilePath);
            return QString();
        }

        compilerProcess.waitForFinished();

        QFile::remove(sourceFilePath);

        if (compilerProcess.exitStatus() == QProcess::NormalExit && compilerProcess.exitCode() == 0)
        {
            return executablePath;
        }
        else
        {
            qWarning() << "CodeRunner: Compilation failed with exit code" << compilerProcess.exitCode();
            qWarning() << compilerProcess.readAllStandardError();
            return QString();
        }
    }

    cppforge::entities::ExecutionResult
    CodeRunner::runTestsBlocking(const QString &executablePath, const std::vector<cppforge::entities::TestCase> &tests)
    {
        uint32_t passedCount = 0;
        int32_t totalTimeMs = 0;
        QString allOutput;
        QString allErrors;
        bool allPassed = true;

        for (const auto &test : tests)
        {
            QProcess runProcess;
            runProcess.start(executablePath);

            if (!runProcess.waitForStarted())
            {
                allErrors = allErrors % "Failed to start executable for test " % QString::number(test.getId()) % "\n";
                allPassed = false;
                continue;
            }

            QElapsedTimer timer;
            timer.start();

            runProcess.write(test.getInput().toUtf8());
            runProcess.closeWriteChannel();

            if (!runProcess.waitForFinished(5000))
            {
                runProcess.kill();
                allErrors = allErrors % "Test " % QString::number(test.getId()) % " timed out\n";
                allPassed = false;
                continue;
            }

            totalTimeMs += timer.elapsed();

            QString output = QString::fromUtf8(runProcess.readAllStandardOutput()).trimmed();
            QString errorOutput = QString::fromUtf8(runProcess.readAllStandardError()).trimmed();

            allOutput = allOutput % "Test " % QString::number(test.getId()) % " output:\n" % output % "\n";

            if (!errorOutput.isEmpty())
            {
                allErrors = allErrors % "Test " % QString::number(test.getId()) % " errors:\n" % errorOutput % "\n";
            }

            if (output == test.getExpectedOutput().trimmed() && runProcess.exitCode() == 0)
            {
                passedCount++;
            }
            else
            {
                allPassed = false;
                allErrors = allErrors % "Test " % QString::number(test.getId()) % " failed. Expected: '" %
                            test.getExpectedOutput().trimmed() % "', Got: '" % output % "'\n";
            }
        }

        return cppforge::entities::ExecutionResult(0, // submissionId (placeholder)
                                                   allPassed, allOutput, allErrors, totalTimeMs, passedCount);
    }
} // namespace cppforge::services

#include "../../include/services/DockerCodeRunner.hpp"

#include "../../include/entities/ExecutionResult.hpp"
#include "../../include/entities/TestCase.hpp"

#include <QDir>
#include <QFile>
#include <QProcess>
#include <QTemporaryDir>

#include <stdexcept>

namespace cppforge::services
{
    DockerCodeRunner::DockerCodeRunner(QString imageName) : m_imageName(std::move(imageName)) {}

    bool DockerCodeRunner::isAvailable() const
    {
        QProcess process;
        process.start("docker", {"info"});
        return process.waitForFinished(2000) && (process.exitCode() == 0);
    }

    entities::ExecutionResult DockerCodeRunner::runBlocking(const QString &code,
                                                            const std::vector<entities::TestCase> &tests)
    {
        QTemporaryDir tempDir;
        if (!tempDir.isValid())
        {
            return entities::ExecutionResult(0, false, "", "Failed to create temporary directory for Docker execution",
                                             0, 0);
        }

        QString sourceFilePath = tempDir.path() + "/main.cpp";
        QFile sourceFile(sourceFilePath);
        if (!sourceFile.open(QIODevice::WriteOnly))
        {
            return entities::ExecutionResult(0, false, "", "Failed to write source code to temp directory", 0, 0);
        }
        sourceFile.write(code.toUtf8());
        sourceFile.close();

        sourceFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ReadUser |
                                  QFileDevice::WriteUser | QFileDevice::ReadGroup | QFileDevice::WriteGroup |
                                  QFileDevice::ReadOther | QFileDevice::WriteOther);

        try
        {
            compileInDocker(tempDir.path());
            return runTestsInDocker(tempDir.path(), tests);
        }
        catch (const std::runtime_error &e)
        {
            return entities::ExecutionResult(0, false, "", QString::fromUtf8(e.what()), 0, 0);
        }
    }

    QString DockerCodeRunner::compileInDocker(const QString &workDir)
    {
        QProcess compiler;
        QString nativeWorkDir = QDir::toNativeSeparators(workDir);
        QString volumeArgument = QString("%1:/sandbox").arg(nativeWorkDir);

        QStringList args = {"run", "--rm",     "--network", "none", "--memory=256m", "--cpus=1.0", "-v", volumeArgument,
                            "-w",  "/sandbox", m_imageName, "g++",  "-O2",           "main.cpp",   "-o", "app"};

        compiler.start("docker", args);

        if (!compiler.waitForFinished(15000) || compiler.exitCode() != 0)
        {
            QString errorLog = QString::fromLocal8Bit(compiler.readAllStandardError());
            if (errorLog.isEmpty())
            {
                errorLog = QString::fromLocal8Bit(compiler.readAllStandardOutput());
            }
            throw std::runtime_error("Compilation Error (Docker Sandbox):\n" + errorLog.toStdString());
        }

        QString exePath = workDir + "/app";
        QFile::setPermissions(exePath, QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner |
                                           QFileDevice::ReadUser | QFileDevice::WriteUser | QFileDevice::ExeUser |
                                           QFileDevice::ReadGroup | QFileDevice::WriteGroup | QFileDevice::ExeGroup |
                                           QFileDevice::ReadOther | QFileDevice::WriteOther | QFileDevice::ExeOther);

        return exePath;
    }

    entities::ExecutionResult DockerCodeRunner::runTestsInDocker(const QString &workDir,
                                                                 const std::vector<entities::TestCase> &tests)
    {
        uint32_t passedCount = 0;
        QString lastOutput;
        QString errorLog;

        QString nativeWorkDir = QDir::toNativeSeparators(workDir);
        QString volumeArgument = QString("%1:/sandbox:ro").arg(nativeWorkDir);

        if (tests.empty())
        {
            QProcess process;
            process.setProcessChannelMode(QProcess::MergedChannels);

            QStringList args = {"run", "--rm",         "-i", "--network", "none",      "--memory=128m", "--cpus=0.5",
                                "-v",  volumeArgument, "-w", "/sandbox",  m_imageName, "./app"};

            process.start("docker", args);

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

                QStringList args = {"run",           "--rm",       "-i",   "--network",    "none",
                                    "--memory=128m", "--cpus=0.5", "-v",   volumeArgument, "-w",
                                    "/sandbox",      m_imageName,  "./app"};

                process.start("docker", args);

                if (!test.getInput().isEmpty())
                {
                    process.write(test.getInput().toUtf8());
                    process.closeWriteChannel();
                }

                if (process.waitForFinished(3000))
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
                    lastOutput = "TLE (Time Limit Exceeded in Docker Sandbox)";
                    break;
                }
            }
        }

        bool isAllPassed = (!tests.empty() && passedCount == tests.size()) || (tests.empty() && errorLog.isEmpty());

        return entities::ExecutionResult(0, isAllPassed, lastOutput, errorLog, 0, passedCount);
    }
} // namespace cppforge::services

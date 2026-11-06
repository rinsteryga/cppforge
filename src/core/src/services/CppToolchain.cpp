#include "../../include/services/CppToolchain.hpp"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QTemporaryFile>

#include <utility>

namespace cppforge::services
{
    namespace
    {
#ifdef Q_OS_WIN
        constexpr QChar kPathSeparator = ';';
#else
        constexpr QChar kPathSeparator = ':';
#endif

        QStringList pathEntries(const QString &value)
        {
            return value.split(kPathSeparator, Qt::SkipEmptyParts);
        }

        QString joinPathEntries(const QStringList &entries)
        {
            return entries.join(kPathSeparator);
        }

        bool isMsvc(const QString &compilerPath)
        {
            return QFileInfo(compilerPath).completeBaseName().compare("cl", Qt::CaseInsensitive) == 0;
        }

        QProcessEnvironment withCompilerDirectory(const QProcessEnvironment &base, const QString &compilerPath)
        {
            QProcessEnvironment environment = base;
            if (compilerPath.isEmpty())
                return environment;

            const QString directory = QFileInfo(compilerPath).absolutePath();
            QStringList entries = pathEntries(environment.value("PATH"));
            entries.removeAll(directory);
            entries.prepend(directory);
            environment.insert("PATH", joinPathEntries(entries));
            return environment;
        }

        bool verify(const QString &compilerPath, const QProcessEnvironment &environment)
        {
            if (compilerPath.isEmpty() || !QFileInfo(compilerPath).isExecutable())
                return false;

            if (isMsvc(compilerPath))
            {
                QTemporaryDir directory;
                if (!directory.isValid())
                    return false;

                QFile source(directory.filePath("probe.cpp"));
                if (!source.open(QIODevice::WriteOnly))
                    return false;
                source.write("int main() { return 0; }\n");
                source.close();

                QProcess process;
                process.setProcessEnvironment(environment);
                process.setWorkingDirectory(directory.path());
                process.start(compilerPath, {"/nologo", "/EHsc", "/MT", "probe.cpp", "/Fe:probe.exe"});
                return process.waitForFinished(60000) && process.exitCode() == 0;
            }

            QProcess process;
            process.setProcessEnvironment(environment);
            process.start(compilerPath, {"--version"});
            return process.waitForFinished(10000) && process.exitCode() == 0;
        }

#ifdef Q_OS_WIN
        QString findVcvars64()
        {
            const QStringList roots = {qEnvironmentVariable("ProgramFiles(x86)"), qEnvironmentVariable("ProgramFiles")};
            for (const QString &root : roots)
            {
                if (root.isEmpty())
                    continue;

                const QString vswhere = root + "/Microsoft Visual Studio/Installer/vswhere.exe";
                if (!QFileInfo::exists(vswhere))
                    continue;

                QProcess process;
                process.start(vswhere,
                              {"-latest", "-products", "*", "-requires",
                               "Microsoft.VisualStudio.Component.VC.Tools.x86.x64", "-property", "installationPath"});
                if (!process.waitForFinished(15000) || process.exitCode() != 0)
                    continue;

                const QString installation = QString::fromLocal8Bit(process.readAllStandardOutput()).trimmed();
                if (installation.isEmpty())
                    continue;

                const QString vcvars = QDir::cleanPath(installation) + "/VC/Auxiliary/Build/vcvars64.bat";
                if (QFileInfo::exists(vcvars))
                    return vcvars;
            }
            return {};
        }

        QProcessEnvironment captureVcvarsEnvironment(const QString &vcvarsPath)
        {
            QTemporaryFile script(QDir::tempPath() + "/cppforge_vcvars_XXXXXX.bat");
            if (!script.open())
                return {};

            const QString contents = QStringLiteral("@echo off\r\ncall \"") + QDir::toNativeSeparators(vcvarsPath) +
                                     QStringLiteral("\" >nul\r\nset\r\n");
            script.write(contents.toLocal8Bit());
            script.close();

            QProcess process;
            process.start("cmd.exe", {"/d", "/s", "/c", script.fileName()});
            if (!process.waitForFinished(60000) || process.exitCode() != 0)
                return {};

            QProcessEnvironment environment;
            const QStringList lines = QString::fromLocal8Bit(process.readAllStandardOutput())
                                          .split(QRegularExpression("[\r\n]+"), Qt::SkipEmptyParts);
            for (const QString &line : lines)
            {
                const int separator = line.indexOf('=');
                if (separator > 0)
                    environment.insert(line.left(separator), line.mid(separator + 1));
            }
            return environment;
        }
#endif
    } // namespace

    const CppToolchain &CppToolchain::detected()
    {
        static const CppToolchain toolchain = []
        {
            const CppToolchain resolved = resolve();
            qDebug() << "[CppToolchain] Using" << (resolved.isAvailable() ? resolved.compilerPath() : "none");
            return resolved;
        }();
        return toolchain;
    }

    CppToolchain CppToolchain::resolve()
    {
        QString override = qEnvironmentVariable("CPPFORGE_CXX").trimmed();
        if (!override.isEmpty())
        {
            const QString found = QStandardPaths::findExecutable(override);
            if (!found.isEmpty())
                override = found;

#ifdef Q_OS_WIN
            if (isMsvc(override))
            {
                const QString vcvars = findVcvars64();
                if (!vcvars.isEmpty())
                {
                    const QProcessEnvironment environment = captureVcvarsEnvironment(vcvars);
                    if (verify(override, environment))
                        return CppToolchain(override, environment, Family::Msvc);
                }
            }
            else
#endif
            {
                const CppToolchain toolchain = fromCompiler(override);
                if (verify(toolchain.m_compilerPath, toolchain.m_environment))
                    return toolchain;
            }
        }

#ifdef Q_OS_WIN
        {
            const QString vcvars = findVcvars64();
            if (!vcvars.isEmpty())
            {
                const QProcessEnvironment environment = captureVcvarsEnvironment(vcvars);
                const QString cl = QStandardPaths::findExecutable("cl", pathEntries(environment.value("PATH")));
                if (verify(cl, environment))
                    return CppToolchain(cl, environment, Family::Msvc);
            }
        }
#endif

        for (const char *candidate : {"g++", "clang++", "c++"})
        {
            const QString path = QStandardPaths::findExecutable(QString::fromLatin1(candidate));
            if (path.isEmpty())
                continue;

            const CppToolchain toolchain = fromCompiler(path);
            if (verify(toolchain.m_compilerPath, toolchain.m_environment))
                return toolchain;
        }

        return CppToolchain();
    }

    CppToolchain CppToolchain::fromCompiler(const QString &compilerPath)
    {
        Family family = isMsvc(compilerPath) ? Family::Msvc : Family::Gcc;
        return CppToolchain(compilerPath, withCompilerDirectory(QProcessEnvironment::systemEnvironment(), compilerPath),
                            family);
    }

    CppToolchain::CppToolchain(QString compilerPath, QProcessEnvironment environment, Family family)
        : m_family(family), m_compilerPath(std::move(compilerPath)), m_environment(std::move(environment))
    {
    }

    QStringList CppToolchain::compileArguments(const QString &sourcePath, const QString &executablePath) const
    {
        if (m_family == Family::Msvc)
        {
            return {"/nologo", "/std:c++20", "/EHsc", "/O2", "/MT", "/utf-8", sourcePath, "/Fe:" + executablePath};
        }

        return {"-std=c++20", "-O2", sourcePath, "-o", executablePath};
    }

    QString CppToolchain::executableSuffix()
    {
#ifdef Q_OS_WIN
        return ".exe";
#else
        return {};
#endif
    }
} // namespace cppforge::services

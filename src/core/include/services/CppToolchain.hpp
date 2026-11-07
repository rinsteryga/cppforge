#pragma once

#include <QProcessEnvironment>
#include <QString>
#include <QStringList>

namespace cppforge::services
{
    /**
     * @brief Host C++ compiler used to build user code locally.
     *
     * The toolchain is resolved once per process. On Windows MSVC is preferred,
     * because the application and its vcpkg dependencies are built with it;
     * GCC and Clang are used as a fallback on every platform. The CPPFORGE_CXX
     * environment variable can be used to force a specific compiler.
     */
    class CppToolchain
    {
    public:
        CppToolchain() = default;

        /**
         * @brief Returns the detected toolchain, resolving it on first use.
         */
        static const CppToolchain &detected();

        /**
         * @brief Whether a usable compiler was found.
         */
        bool isAvailable() const { return m_family != Family::None; }

        /**
         * @brief Absolute path to the compiler executable.
         */
        QString compilerPath() const { return m_compilerPath; }

        /**
         * @brief Environment required to run the compiler and produced programs.
         */
        QProcessEnvironment environment() const { return m_environment; }

        /**
         * @brief Arguments that compile sourcePath into executablePath.
         */
        QStringList compileArguments(const QString &sourcePath, const QString &executablePath) const;

        /**
         * @brief Platform executable suffix (".exe" on Windows).
         */
        static QString executableSuffix();

    private:
        enum class Family
        {
            None,
            Msvc,
            Gcc
        };

        CppToolchain(QString compilerPath, QProcessEnvironment environment, Family family);

        static CppToolchain resolve();
        static CppToolchain fromCompiler(const QString &compilerPath);

        Family m_family{Family::None};
        QString m_compilerPath;
        QProcessEnvironment m_environment;
    };
} // namespace cppforge::services

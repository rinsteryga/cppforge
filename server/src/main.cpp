#include "server/ServerApp.hpp"
#include <utils/EnvLoader.hpp>

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QStringList>

#include <iostream>

namespace
{
    void loadEnvironment()
    {
        const QStringList paths = {QCoreApplication::applicationDirPath() + QStringLiteral("/.env"),
                                   QDir::currentPath() + QStringLiteral("/.env"),
                                   QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) +
                                       QStringLiteral("/.env")};

        for (const auto &path : paths)
        {
            if (QFileInfo::exists(path))
            {
                cppforge::utils::loadEnvFile(path);
                return;
            }
        }
    }
} // namespace

int main(int argc, char *argv[])
{
    QCoreApplication application(argc, argv);
    loadEnvironment();

    try
    {
        cppforge::server::ServerApp server(cppforge::server::ServerConfig::fromEnvironment());
        server.run();
    }
    catch (const std::exception &)
    {
        std::cerr << "[CppForge Server] Failed to start; verify server and database configuration.\n";
        return 1;
    }

    return 0;
}

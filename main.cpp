#include "AuthWindow.hpp"
#include "MainWindow.hpp"
#include "src/core/include/services/AuthManager.hpp"
#include "src/data/include/repositories/PgUserRepository.hpp"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QResource>
#include <QScreen>
#include <QSqlError>
#include <QSqlQuery>
#include <QTimer>

namespace cppforge
{
    namespace data
    {
        QSqlDatabase connectDatabase();
    }
} // namespace cppforge

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resources);
    QApplication app(argc, argv);

    QSqlDatabase db = cppforge::data::connectDatabase();

    if (db.isOpen())
    {
        QSqlQuery syncQuery(db);
        QString encodingQuery;

#ifdef Q_OS_WIN
        encodingQuery = "SET client_encoding TO 'WIN1251';";
        qDebug() << "Applying Windows compatibility settings (WIN1251).";
#else
        // Для Linux (Ubuntu), macOS и прочих — используем стандартный UTF8
        encodingQuery = "SET client_encoding TO 'UTF8';";
        qDebug() << "Applying Unix-like compatibility settings (UTF8).";
#endif

        if (syncQuery.exec(encodingQuery))
        {
            qDebug() << "Database client encoding successfully synchronized.";
        }
        else
        {
            qWarning() << "Failed to set database encoding:" << syncQuery.lastError().text();
        }
    }

    auto userRepository = std::make_unique<cppforge::repositories::PgUserRepository>(db);
    auto authManager = std::make_shared<cppforge::services::AuthManager>(std::move(userRepository));

    AuthWindow authWindow(authManager);
    MainWindow mainWindow;

    QObject::connect(&authWindow, &AuthWindow::switchToMainMenu,
                     [&]()
                     {
                         qDebug() << "Switching to MainWindow...";

                         QScreen *screen = QGuiApplication::primaryScreen();
                         if (screen)
                         {
                             QRect availableGeometry = screen->availableGeometry();
                             int x = availableGeometry.x() + (availableGeometry.width() - mainWindow.width()) / 2;
                             int y = availableGeometry.y() + (availableGeometry.height() - mainWindow.height()) / 2;
                             mainWindow.move(x, y);
                         }

                         mainWindow.show();
                         mainWindow.fadeIn();
                         authWindow.close();
                     });

    authWindow.show();

    return app.exec();
}
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

    app.setQuitOnLastWindowClosed(false);

    QSqlDatabase db = cppforge::data::connectDatabase();

    if (db.isOpen())
    {
        QSqlQuery syncQuery(db);
        if (syncQuery.exec("SET client_encoding TO 'UTF8';"))
        {
            qDebug() << "Encoding set to UTF8 successfully.";
        }
        else
        {
            qWarning() << "Failed to set encoding:" << syncQuery.lastError().text();
        }
    }

    auto userRepository = std::make_unique<cppforge::repositories::PgUserRepository>(db);
    auto authManager = std::make_shared<cppforge::services::AuthManager>(std::move(userRepository));

    AuthWindow authWindow(authManager);
    MainWindow mainWindow;

    // ИЗМЕНЕНО: Добавлен параметр userId в лямбду
    QObject::connect(&authWindow, &AuthWindow::switchToMainMenu,
                     [&](const QString &username, int userId)
                     {
                         mainWindow.setCurrentUser(username);
                         // ИЗМЕНЕНО: Установка ID в главном окне
                         mainWindow.setUserId(userId); 

                         QScreen *screen = QGuiApplication::primaryScreen();
                         if (screen)
                         {
                             QRect geom = screen->availableGeometry();
                             mainWindow.move(geom.center() - mainWindow.rect().center());
                         }

                         mainWindow.show();
                         mainWindow.fadeIn();
                         authWindow.hide();
                     });

    QObject::connect(&app, &QApplication::lastWindowClosed, &app, &QApplication::quit);

    authWindow.show();
    return app.exec();
}
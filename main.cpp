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
#include <QSettings>
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
        syncQuery.exec("SET client_encoding TO 'UTF8';");
    }

    auto userRepository = std::make_unique<cppforge::repositories::PgUserRepository>(db);
    auto authManager = std::make_shared<cppforge::services::AuthManager>(std::move(userRepository));

    QSettings settings("CppForge", "StudyApp");
    bool remember = settings.value("auth/remember", false).toBool();
    int savedUserId = settings.value("auth/user_id", -1).toInt();
    QString savedUsername = settings.value("auth/username", "").toString();

    AuthWindow authWindow(authManager);
    MainWindow mainWindow;

    auto showMain = [&](const QString &username, int userId)
    {
        mainWindow.setCurrentUser(username);
        mainWindow.setUserId(userId);

        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen)
        {
            QRect geom = screen->availableGeometry();
            mainWindow.move(geom.center() - mainWindow.rect().center());
        }

        authWindow.hide(); 
        mainWindow.show();
        mainWindow.fadeIn();
    };

    QObject::connect(&authWindow, &AuthWindow::switchToMainMenu, showMain);
    QObject::connect(&app, &QApplication::lastWindowClosed, &app, &QApplication::quit);

    bool autoLoginValid = false;
    if (remember && savedUserId != -1 && !savedUsername.isEmpty())
    {
        QSqlQuery checkQuery;
        checkQuery.prepare("SELECT id FROM users WHERE id = :id AND username = :name");
        checkQuery.bindValue(":id", savedUserId);
        checkQuery.bindValue(":name", savedUsername);

        if (checkQuery.exec() && checkQuery.next()) {
            autoLoginValid = true;
            qDebug() << "Auto-login verified for user:" << savedUsername;
            showMain(savedUsername, savedUserId);
        }
        else {
            qDebug() << "Auto-login failed: User no longer exists in database. Clearing settings.";
            settings.remove("auth/remember");
            settings.remove("auth/user_id");
            settings.remove("auth/username");
            settings.sync();
        }
    }

    if (!autoLoginValid)
    {
        authWindow.show();
    }

    return app.exec();
}
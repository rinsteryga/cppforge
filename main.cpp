#include "AuthWindow.hpp"
#include "MainWindow.hpp"
#include "src/core/include/services/AuthManager.hpp"
#include "src/data/repositories/PgUserRepository.hpp"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QResource>
#include <QScreen>
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
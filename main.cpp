#include "src/data/repositories/PgUserRepository.hpp"
#include "src/core/include/services/AuthManager.hpp"
#include "AuthWindow.hpp"
#include "MainWindow.hpp"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QResource>
#include <QTimer>
#include <QScreen>
#include <QGuiApplication>

namespace cppforge { namespace data { QSqlDatabase connectDatabase(); } }

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resources);
    QApplication app(argc, argv);

    QSqlDatabase db = cppforge::data::connectDatabase();

    auto userRepository =
        std::make_unique<cppforge::repositories::PgUserRepository>(db);

    auto authManager =
        std::make_shared<cppforge::services::AuthManager>(std::move(userRepository));

    AuthWindow authWindow(authManager);
    MainWindow mainWindow;

    // Подключаемся к правильному сигналу switchToMainMenu
    QObject::connect(&authWindow,
                     &AuthWindow::switchToMainMenu,  // ИСПРАВЛЕНО: был loginSuccessful
                     [&]()
                     {
                         qDebug() << "Switching to MainWindow...";
                         
                         // Центрируем главное окно
                         QScreen *screen = QGuiApplication::primaryScreen();
                         if (screen) {
                             QRect availableGeometry = screen->availableGeometry();
                             int x = availableGeometry.x() + (availableGeometry.width() - mainWindow.width()) / 2;
                             int y = availableGeometry.y() + (availableGeometry.height() - mainWindow.height()) / 2;
                             mainWindow.move(x, y);
                         }
                         
                         mainWindow.show();
                         mainWindow.fadeIn();  // Плавное появление
                         authWindow.close();
                     });

    authWindow.show();

    return app.exec();
}
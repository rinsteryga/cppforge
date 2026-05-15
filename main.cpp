#include "AuthWindow.hpp"
#include "MainWindow.hpp"
#include "ThemeManager.hpp"
#include "WindowStateManager.hpp"
#include "repositories/PgAchievementRepository.hpp"
#include "repositories/PgCodingTaskRepository.hpp"
#include "repositories/PgLessonRepository.hpp"
#include "repositories/PgUserRepository.hpp"
#include "services/AchievementService.hpp"
#include "services/AuthManager.hpp"
#include "services/CourseService.hpp"
#include "services/TaskManager.hpp"
#include "services/ThemeService.hpp"
#include "services/UserService.hpp"

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

namespace cppforge::data
{
    QSqlDatabase connectDatabase();
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    Q_INIT_RESOURCE(resources);
    QApplication app(argc, argv);

    app.setStyleSheet("QToolTip { background-color: rgb(255, 255, 255); color: rgb(0, 0, 0); border: 1px solid "
                      "rgb(150, 150, 150); padding: 5px; }");

    app.setQuitOnLastWindowClosed(false);

    QSqlDatabase db = cppforge::data::connectDatabase();

    if (db.isOpen())
    {
        QSqlQuery syncQuery(db);
        syncQuery.exec("SET client_encoding TO 'UTF8';");
    }

    // Initialize repositories
    auto authUserRepo = std::make_unique<cppforge::repositories::PgUserRepository>(db);
    auto serviceUserRepo = std::make_unique<cppforge::repositories::PgUserRepository>(db);
    auto achievementRepo = std::make_unique<cppforge::repositories::PgAchievementRepository>(db);
    auto lessonRepo = std::make_unique<cppforge::repositories::PgLessonRepository>(db);
    auto codingTaskRepo = std::make_unique<cppforge::repositories::PgCodingTaskRepository>(db);

    // Initialize services
    auto authManager = std::make_shared<cppforge::services::AuthManager>(std::move(authUserRepo));
    auto userService = std::make_shared<cppforge::services::UserService>(*serviceUserRepo, *achievementRepo);
    auto achievementService =
        std::make_shared<cppforge::services::AchievementService>(*serviceUserRepo, *achievementRepo);
    auto courseService = std::make_shared<cppforge::services::CourseService>(lessonRepo.get(), codingTaskRepo.get(),
                                                                             serviceUserRepo.get());

    // Initialize TaskManager singleton
    cppforge::services::TaskManager::instance().setTaskRepository(codingTaskRepo.get());

    auto themeService = std::make_shared<cppforge::services::ThemeService>();
    cppforge::gui::ThemeManager themeManager(themeService.get());

    QSettings settings("CppForge", "StudyApp");
    bool remember = settings.value("auth/remember", false).toBool();
    int savedUserId = settings.value("auth/user_id", -1).toInt();
    QString savedUsername = settings.value("auth/username", "").toString();

    AuthWindow authWindow(authManager);
    authWindow.setThemeService(themeService.get());
    MainWindow mainWindow;
    mainWindow.setUserService(userService.get());
    mainWindow.setAchievementService(achievementService.get());
    mainWindow.setCourseService(courseService.get());
    mainWindow.setThemeService(themeService.get());

    auto showMain = [&](const QString &username, int userId)
    {
        mainWindow.setCurrentUser(username);
        mainWindow.setUserId(userId);

        authWindow.hide();
        WindowStateManager::instance().applyState(&mainWindow, QSize(1200, 800));
        mainWindow.fadeIn();
    };

    QObject::connect(&authWindow, &AuthWindow::switchToMainMenu, showMain);
    QObject::connect(&app, &QApplication::lastWindowClosed, &app, &QApplication::quit);

    bool autoLoginValid = false;
    if (remember && savedUserId != -1 && !savedUsername.isEmpty())
    {
        auto user = serviceUserRepo->findById(static_cast<uint64_t>(savedUserId));
        if (user && user->getUsername() == savedUsername)
        {
            autoLoginValid = true;
            qDebug() << "Auto-login verified for user:" << savedUsername;
            showMain(savedUsername, savedUserId);
        }
        else
        {
            qDebug() << "Auto-login failed: User no longer exists in database. Clearing settings.";
            settings.remove("auth/remember");
            settings.remove("auth/user_id");
            settings.remove("auth/username");
            settings.sync();
        }
    }

    if (!autoLoginValid)
    {
        WindowStateManager::instance().applyState(&authWindow, QSize(1024, 720));
    }

    return app.exec();
}
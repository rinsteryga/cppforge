#include "AuthController.hpp"
#include "DashboardController.hpp"
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
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQuickWindow>
#include <QResource>
#include <QScreen>
#include <QSettings>
#include <QSqlError>
#include <QSqlQuery>
#include <QTimer>
#include <QtQml/qqmlextensionplugin.h>

Q_IMPORT_QML_PLUGIN(CppForgePlugin)

namespace cppforge::data
{
    QSqlDatabase connectDatabase();
}

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resources);
    QApplication app(argc, argv);

    app.setOrganizationName("CppForge");
    app.setOrganizationDomain("cppforge.org");
    app.setApplicationName("StudyApp");

    app.setStyleSheet("QToolTip { background-color: rgb(255, 255, 255); color: rgb(0, 0, 0); border: 1px solid "
                      "rgb(150, 150, 150); padding: 5px; }");

    app.setQuitOnLastWindowClosed(true);

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

    QQmlApplicationEngine engine;
    engine.addImportPath(":/qt/qml");

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--test-qml") == 0)
        {
            QQmlComponent authComp(&engine, QUrl(QStringLiteral("qrc:/qt/qml/CppForge/qml/AuthWindow.qml")));
            if (authComp.isError())
            {
                qCritical() << "AuthWindow error:" << authComp.errors();
                return 1;
            }
            QQmlComponent mainComp(&engine, QUrl(QStringLiteral("qrc:/qt/qml/CppForge/qml/MainWindow.qml")));
            if (mainComp.isError())
            {
                qCritical() << "MainWindow error:" << mainComp.errors();
                return 1;
            }
            qDebug() << "QML self-test PASSED: All main windows and components loaded without errors!";
            return 0;
        }
    }

    auto authController = std::make_unique<cppforge::gui::AuthController>();
    authController->setAuthManager(authManager);

    auto dashboardController = std::make_unique<cppforge::gui::DashboardController>();
    dashboardController->setUserService(userService.get());
    dashboardController->setCourseService(courseService.get());

    QPointer<QQuickWindow> currentQuickWin;

    std::function<void(const QString &, int)> showMain;
    std::function<void()> showAuth;

    showMain = [&](const QString &username, int userId)
    {
        dashboardController->setUser(username, userId);

        auto oldWin = currentQuickWin;
        currentQuickWin = nullptr;

        QQmlComponent mainComp(&engine, QUrl(QStringLiteral("qrc:/qt/qml/CppForge/qml/MainWindow.qml")));
        if (mainComp.isError())
        {
            qCritical() << "MainWindow QML error:" << mainComp.errors();
            if (oldWin)
            {
                currentQuickWin = oldWin;
            }
            return;
        }

        QObject *mainObj = mainComp.createWithInitialProperties({
            {"dashboardController", QVariant::fromValue(dashboardController.get())}
        });

        if (auto *mainWin = qobject_cast<QQuickWindow *>(mainObj))
        {
            currentQuickWin = mainWin;
            mainWin->show();
            mainWin->raise();
            mainWin->requestActivate();
        }

        if (oldWin)
        {
            oldWin->close();
            oldWin->deleteLater();
        }
    };

    showAuth = [&]()
    {
        auto oldWin = currentQuickWin;
        currentQuickWin = nullptr;

        QQmlComponent authComp(&engine, QUrl(QStringLiteral("qrc:/qt/qml/CppForge/qml/AuthWindow.qml")));
        if (authComp.isError())
        {
            qCritical() << "AuthWindow QML error:" << authComp.errors();
            if (oldWin)
            {
                currentQuickWin = oldWin;
            }
            return;
        }

        QObject *authObj = authComp.createWithInitialProperties({
            {"authController", QVariant::fromValue(authController.get())}
        });

        if (auto *authWin = qobject_cast<QQuickWindow *>(authObj))
        {
            currentQuickWin = authWin;
            authWin->show();
            authWin->raise();
            authWin->requestActivate();
        }

        if (oldWin)
        {
            oldWin->close();
            oldWin->deleteLater();
        }
    };

    QObject::connect(authController.get(), &cppforge::gui::AuthController::loginSucceeded, showMain);
    QObject::connect(dashboardController.get(), &cppforge::gui::DashboardController::logoutRequested, showAuth);
    QObject::connect(&app, &QApplication::lastWindowClosed, &app, &QApplication::quit);

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--test-main-window") == 0)
        {
            dashboardController->setUser("Sernock", 1);
            showMain("Sernock", 1);
            QTimer::singleShot(2000, &app, &QApplication::quit);
            return app.exec();
        }
    }

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
        showAuth();
    }

    return app.exec();
}
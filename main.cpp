#include "src/data/repositories/PgUserRepository.hpp"
#include "src/core/include/services/AuthManager.hpp"
#include "AuthWindow.hpp"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QResource>


namespace cppforge { namespace data { QSqlDatabase connectDatabase(); } }

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resources);
    QApplication app(argc, argv);

    qDebug() << "Logo exists? " << !QPixmap(":/icons/main_logo.ico").isNull();
    qDebug() << "Open eye exists? " << !QPixmap(":/images/eye_open.png").isNull();

    QSqlDatabase db = cppforge::data::connectDatabase();
    auto userRepository = std::make_unique<cppforge::repositories::PgUserRepository>(db);
    auto authManager = std::make_shared<cppforge::services::AuthManager>(std::move(userRepository));

    AuthWindow window(authManager);
    window.show();

    int result = app.exec();

    qDebug() << "Application exiting with code:" << result;
    
    return result;
}
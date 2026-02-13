#include "AuthWindow.hpp"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QResource>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resources);
    QApplication app(argc, argv);

    qDebug() << "Logo exists? " << !QPixmap(":/icons/main_logo.ico").isNull();
    qDebug() << "Open eye exists? " << !QPixmap(":/images/eye_open.png").isNull();

    AuthWindow window;
    window.show();

    int result = app.exec();

    qDebug() << "Application exiting with code:" << result;
    
    return result;
}
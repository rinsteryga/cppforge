#include <QApplication>
#include <QDebug>
#include "MainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    qDebug() << "Application starting...";
    
    MainWindow window;
    window.show();
    
    int result = app.exec();
    
    qDebug() << "Application exiting with code:" << result;
    
    // Принудительное завершение для Windows
    #ifdef _WIN32
    _exit(result);
    #endif
    
    return result;
}
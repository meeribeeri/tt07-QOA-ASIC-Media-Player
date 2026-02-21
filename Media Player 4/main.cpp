#include "main.h"
#include <bitset>
#include <iostream>


//this is tiny
int main(int argc, char *argv[]) {
    QApplication application(argc, argv);
    application.setApplicationDisplayName("tto7 QOA Player");
    application.setApplicationName("tt07 QOA Player");

   /* debugHandler->moveToThread(&debugThread);
    QObject::connect(&debugThread, &QThread::finished, debugHandler, &QObject::deleteLater);
    debugThread.start();*/

    QObject::connect(debugHandler, &DebugHandler::saveLog, fileHandler, &FileHandler::saveLogFile);

    QObject::connect(&application, &QApplication::lastWindowClosed, settings, &Settings::onClose);
    //QObject::connect(&application, &QApplication::lastWindowClosed, debugHandler, &DebugHandler::saveLogFile);
    QObject::connect(&application, &QApplication::lastWindowClosed, fileHandler, &FileHandler::applicationClosing);


    settings->openInitialSettings();

    Window mainWindow;
    mainWindow.show();
    return application.exec();
}
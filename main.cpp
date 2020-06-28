#include "mainwindow.h"
#include "networkthread.h"
#include "runguard.h"
#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    RunGuard guard("WifiMouseServer");
    if( !guard.tryToRun() ) {
        // QMessageBox error;
        // error.setText("Another instance of WifiMouseServer is already running.");
        // error.exec();
        qInfo() << "Another instance of WifiMouseServer is already running.";
        return 0;
    }

    QCoreApplication::setOrganizationName("WifiMouseAuthors");
    QCoreApplication::setApplicationName("WifiMouseServer");

    MainWindow w;
    NetworkThread networkThread;
    networkThread.mainWindow = &w;

    networkThread.start();
    int result = a.exec();

    networkThread.requestInterruption();
    networkThread.exit();
    networkThread.wait(2000);

    return result;
}

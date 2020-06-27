#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include "setpassworddialog.h"
#include "helpipdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    QIcon *programIcon;
    SetPasswordDialog *setPasswordDialog;
    HelpIpDialog *helpIpDialog;

    QByteArray serverPassword;

    void updateServerIp();

    void createTrayIcon();
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    void createActions();
    QAction *quitAction;
    QAction *maximizeAction;
    QAction *passwordAction;
    QAction *clientTitleAction;
    QAction *clientIpAction;

public Q_SLOTS:
    void clickMaximized();
    void clickMinimized();
    void clickQuit();
    void clickSetPassword();
    void clickIpHelper();

    QByteArray getPassword();
    void setPassword(QString newPassword);
    void setClientIp(QString ip);

    void loadSettings();
    void saveSettings();
};

#endif // MAINWINDOW_H

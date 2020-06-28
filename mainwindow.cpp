#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "encryptutils.h"
#include <QTimer>
#include <QPainter>
#include <QtNetwork/QNetworkInterface>
#include <QSettings>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QCloseEvent>
#include "fakeinput.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    programIcon(new QIcon(":/images/icon64.png"))
{
    ui->setupUi(this);

    setPasswordDialog = new SetPasswordDialog(this);
    helpIpDialog = new HelpIpDialog(this);

    loadSettings();

    this->setWindowIcon(*programIcon);
    this->setFixedSize(this->geometry().width(),this->geometry().height());

    this->setWindowFlags(Qt::Dialog);
    this->setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    updateServerIp();

    createActions();
    createTrayIcon();

    if(ui->startMinimizedCheck->isChecked() == false)
        this->show();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete programIcon;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    QMainWindow::closeEvent(event);
}

void MainWindow::loadSettings()
{
    QSettings settings;
    serverPassword = settings.value("pass","").toByteArray();
    if(serverPassword.length() != 16)
        serverPassword = EncryptUtils::makeHash16("");
    ui->startMinimizedCheck->setChecked( settings.value("startMinimized", true).toBool() );
    ui->tcpEnabledCheck->setChecked( settings.value("tcpServerEnabled", true).toBool() );
    ui->bluetoothEnabledCheck->setChecked( settings.value("bluetoothServerEnabled", true).toBool() );
}

void MainWindow::saveSettings()
{
    QSettings settings;
    settings.setValue("pass", serverPassword);
    settings.setValue("startMinimized", ui->startMinimizedCheck->isChecked());
    settings.setValue("tcpServerEnabled", ui->tcpEnabledCheck->isChecked());
    settings.setValue("bluetoothServerEnabled", ui->bluetoothEnabledCheck->isChecked());
    emit configChanged();
}

QByteArray MainWindow::getPassword()
{
    return serverPassword;
}

void MainWindow::setPassword(QString newPassword)
{
    // Store password as 16 length hash ready to be used as AES key
    serverPassword = EncryptUtils::makeHash16(newPassword.toUtf8());
    saveSettings();
}

void MainWindow::setClientIp(QString ip)
{
    if (ip != lastClient ) {
        lastClient = ip;
        if(ip == "Not connected") {
            if ( ui->tcpEnabledCheck->isChecked() || ui->bluetoothEnabledCheck->isChecked() )
                ui->clientInfoLabel->setText("Listening for clients");
           else
                ui->clientInfoLabel->setText("Server Disabled");
        } else
            ui->clientInfoLabel->setText("Client: "+ip);
    }
}

void MainWindow::updateServerIp()
{
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost)) {
            setWindowTitle( QString("WifiMouseServer: %1").arg(address.toString()) );
            return;
        }
    }
}

void MainWindow::clickMaximized()
{
    this->show();
    this->raise();
}

void MainWindow::clickMinimized()
{
    this->hide();
}

void MainWindow::clickQuit()
{
    this->close();
}

void MainWindow::clickSetPassword()
{
    if(!setPasswordDialog->isVisible())
        setPasswordDialog->show();
}

void MainWindow::clickIpHelper()
{
    helpIpDialog->show();
}

void MainWindow::createActions()
{
    quitAction = new QAction(tr("Quit"),this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(clickQuit()));
    connect(ui->minimizeButton, SIGNAL(released()), this, SLOT(clickMinimized()));

    maximizeAction = new QAction(tr("Display"),this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(clickMaximized()));

    passwordAction = new QAction(tr("Set password"),this);
    connect(passwordAction, SIGNAL(triggered()), this, SLOT(clickSetPassword()));
    connect(ui->passwordButton, SIGNAL(released()), this, SLOT(clickSetPassword()));

    connect(ui->startMinimizedCheck, SIGNAL(released()), this, SLOT(saveSettings()));

    connect(ui->tcpEnabledCheck, SIGNAL(released()), this, SLOT(saveSettings()));
    connect(ui->bluetoothEnabledCheck, SIGNAL(released()), this, SLOT(saveSettings()));

    connect(ui->helpButton, SIGNAL(released()), this, SLOT(clickIpHelper()));
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
  if (reason == QSystemTrayIcon::Trigger)
    clickMaximized();
}

void MainWindow::createTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(*programIcon);
    trayIcon->setToolTip("WifiMouseServer");
    trayIcon->show();

    trayIconMenu = new QMenu(this);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(passwordAction);
    trayIconMenu->addAction(quitAction);
    trayIcon->setContextMenu(trayIconMenu);

    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
}

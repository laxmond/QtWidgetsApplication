#pragma once

#include <QtWidgets/QMainWindow>
//#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QTcpSocket>
#include "ui_QtWidgetsApplication.h"


class QtWidgetsApplication : public QMainWindow
{
    Q_OBJECT

public:
    QtWidgetsApplication(QWidget *parent = nullptr);
    ~QtWidgetsApplication();


public slots:

    void serverConnected();

    void serverDisconnected();

    void updateClientProgress(qint64 numBytes);

    void displayError(QAbstractSocket::SocketError);

    void connectServer();

    void disconnectServer();

    void sendMsg();

private:
    Ui::QtWidgetsApplicationClass ui;
    QTcpSocket* m_tcpClient;

    QByteArray m_outBlock;
};

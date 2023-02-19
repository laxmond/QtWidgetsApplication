#pragma once

#include <QtWidgets/QMainWindow>
#include <QtNetwork/QtcpServer>
#include <QtNetwork/QTcpSocket>
#include "ui_QtServerApplication.h"

class QtServerApplication : public QMainWindow
{
    Q_OBJECT

public:
    QtServerApplication(QWidget *parent = nullptr);
    ~QtServerApplication();


 
public slots:
    void startServer();

    void acceptConnection();

    void readData();

    void displayError(QAbstractSocket::SocketError);

    void displayAcceptError(QAbstractSocket::SocketError);

private:
    Ui::QtServerApplicationClass ui;

    QTcpServer m_tcpServer;

    QTcpSocket* m_tcpServerConnection = nullptr;
};

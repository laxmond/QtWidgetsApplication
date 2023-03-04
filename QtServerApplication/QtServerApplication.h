#pragma once

#include <QtWidgets/QMainWindow>
#include <QtNetwork/QtcpServer>
#include <QtNetwork/QTcpSocket>
#include "ui_QtServerApplication.h"
#include "TcpSocketConnection.h"

class QtServerApplication : public QMainWindow
{
    Q_OBJECT

public:
    QtServerApplication(QWidget *parent = nullptr);
    ~QtServerApplication();


 
public slots:
    void startServer();

    void acceptConnection();

    void displayLog(QString);

    void process(QTcpSocket* , QByteArray&);

    void displayAcceptError(QAbstractSocket::SocketError);

    void sendMsg();

    void disconnected(qintptr);

private:
    Ui::QtServerApplicationClass ui;

    QTcpServer m_tcpServer;

    QMap<qintptr, TcpSocketConnection*> m_tcpServerConnectionMap;

    //TcpSocketConnection* m_tcpServerConnection = nullptr;
};

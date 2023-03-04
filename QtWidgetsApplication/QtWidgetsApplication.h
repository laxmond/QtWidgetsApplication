#pragma once

#include <QtWidgets/QMainWindow>
//#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QTcpSocket>
#include <QFile>
#include "ui_QtWidgetsApplication.h"

class QtWidgetsApplication : public QMainWindow
{
    Q_OBJECT

public:
    QtWidgetsApplication(QWidget *parent = nullptr);
    ~QtWidgetsApplication();


public Q_SLOTS:

    void serverConnected();

    void serverDisconnected();

    void handleReadData();

    void handleBytesWritten(qint64 numBytes);

    void displayError(QAbstractSocket::SocketError);

    void connectServer();

    void disconnectServer();

    void sendMsg();

    void openFile();

    void sendFile();

private:
    Ui::QtWidgetsApplicationClass ui;
    QTcpSocket* m_chatSocket;
    QTcpSocket* m_fileSocket;

    quint64 m_payloadSize;

    QByteArray m_outBlock;
    QString m_sendFilePath;
    QString m_sendFileName;
    //quint64 m_totalBytesToWrite;
    //quint64 m_bytesToWrite;
    //quint64 m_bytesWritten;
    //QFile* m_sendFile;

    //quint64 m_totalBytesReceived;
    //quint64 m_bytesReceived;
    //quint64 m_fileNameSize;
    //QString m_fileName;
    //QByteArray m_inBlock;
    //QByteArray m_receiveFileName;
    //QFile* m_receiveFile;
};

#pragma once

#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QHostAddress>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QListWidget>
#include <QFile>

class ChatWindow : public QWidget
{
    Q_OBJECT

public:
    ChatWindow(QString title, QWidget* parent = nullptr);
    ~ChatWindow();

private slots:
    void onSendMessageClicked();
    void onNewConnection();
    void onReadyRead();
    void onConnectClicked();
    void onSendFileClicked();
    void onFileOpenClicked();
    void onFileBytesWritten(qint64 bytes);
    void onFileRead(qint64 bytes);

private:
    void setupUI();
    void createConnection();

private:
    QString m_title;
    QTcpServer m_tcpServer;
    QTcpSocket* m_tcpSocket;
    QLineEdit* m_ipLineEdit;
    QLineEdit* m_portLineEdit;
    QTextEdit* m_chatTextEdit;
    QLineEdit* m_inputLineEdit;
    QPushButton* m_sendMessageButton;
    QListWidget* m_fileListWidget;
    QPushButton* m_fileOpenButton;
    QPushButton* m_sendFileButton;
    QTcpSocket* m_fileSocket;
    QFile* m_file;
    qint64 m_fileBytesWritten;
    qint64 m_fileTotalBytes;
    QString m_filePath;
};

#endif // CHATWINDOW_H

#include "QtServerApplication.h"
#include <QMessageBox>
#include <QDateTime>

QtServerApplication::QtServerApplication(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    connect(ui.btnStart, &QPushButton::clicked, this, &QtServerApplication::startServer);

    connect(&m_tcpServer, &QTcpServer::newConnection, this, &QtServerApplication::acceptConnection);
    connect(&m_tcpServer, &QTcpServer::acceptError, this, &QtServerApplication::displayAcceptError);

    connect(ui.btnSend, &QPushButton::clicked, this, &QtServerApplication::sendMsg);

    //connect(&m_tcpServer, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
    //connect(ui.btnStart, SIGNAL(clicked(bool)), this, SLOT(startServer()));
}

QtServerApplication::~QtServerApplication() {
    //delete m_tcpServerConnection;
    //m_tcpServerConnection = nullptr;
}


void QtServerApplication::startServer() {
    int port = ui.txtPort->text().toInt();
    if (!m_tcpServer.listen(QHostAddress::LocalHost, port)) {
        QMessageBox::information(this, tr("Server Listen"),
            tr("The following error occurred: %1.")
            .arg(m_tcpServer.errorString()));
        return;
    }
    m_tcpServer.waitForNewConnection();
    ui.txtDataLog->append(txt("¿ªÊ¼¼àÌý"));
}

void QtServerApplication::acceptConnection() {
    
    QTcpSocket* tcpSocket = m_tcpServer.nextPendingConnection();
    ui.txtDataLog->append(txt(tr("clients %1:%2 gets online").arg(tcpSocket->peerAddress().toString()))
        .arg(tcpSocket->peerPort()));
    TcpSocketConnection* m_tcpServerConnection = new TcpSocketConnection(tcpSocket);
    connect(m_tcpServerConnection, &TcpSocketConnection::log, this, &QtServerApplication::displayLog);
    connect(m_tcpServerConnection, &TcpSocketConnection::disconnected, this, &QtServerApplication::disconnected);
    connect(m_tcpServerConnection, &TcpSocketConnection::process, this, &QtServerApplication::process);
    m_tcpServerConnectionMap[tcpSocket->socketDescriptor()] = m_tcpServerConnection;

    //m_tcpServerConnection->waitForReadyRead(3000);
}

void QtServerApplication::displayLog(QString logString) {
    ui.txtDataLog->append(logString);
}

void QtServerApplication::process(QTcpSocket* tcpSocket, QByteArray& data) {
    QMapIterator<qintptr, TcpSocketConnection*> i(m_tcpServerConnectionMap);
    while (i.hasNext()) {
        i.next();
        i.value()->write(data);
        //if (i.key() != tcpSocket->socketDescriptor()) {
        //    i.value()->write(data);
        //}
    }
}


void QtServerApplication::displayAcceptError(QAbstractSocket::SocketError socketError) {
    switch (socketError) {
    default:
        ui.txtDataLog->append(txt(tr("accetpt error: %1").arg(m_tcpServer.errorString())));
    }
}

void QtServerApplication::sendMsg() {
    QString s = ui.txtDataSend->text();
    QMapIterator<qintptr, TcpSocketConnection*> i(m_tcpServerConnectionMap);
    while (i.hasNext()) {
        i.next();
        i.value()->write(s);
    }
}

void QtServerApplication::disconnected(qintptr socketDescriptor)
{
    if (m_tcpServerConnectionMap.contains(socketDescriptor)) {
        TcpSocketConnection* tcpSocketConnection = m_tcpServerConnectionMap.value(socketDescriptor);
        delete tcpSocketConnection;
        m_tcpServerConnectionMap.remove(socketDescriptor);
        qDebug() << "m_tcpServerConnectionMap remove" << socketDescriptor;
    }
}



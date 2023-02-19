#include "QtServerApplication.h"
#include <QMessageBox>
#include <QDateTime>


QString txt(const char* s) {
    return QObject::tr("[%1]# %2\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"), QString::fromLocal8Bit(s));
}

QString txt(const QString& s) {
    return QObject::tr("[%1]# %2\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"), s);
}

QtServerApplication::QtServerApplication(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    connect(ui.btnStart, &QPushButton::clicked, this, &QtServerApplication::startServer);
    connect(&m_tcpServer, &QTcpServer::newConnection, this, &QtServerApplication::acceptConnection);
    connect(&m_tcpServer, &QTcpServer::acceptError, this, &QtServerApplication::displayAcceptError);
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
    
    m_tcpServerConnection = m_tcpServer.nextPendingConnection();
    ui.txtDataLog->append(txt(tr("clients %1:%2 gets online").arg(m_tcpServerConnection->peerAddress().toString()))
        .arg(m_tcpServerConnection->peerPort()));
    connect(m_tcpServerConnection, &QAbstractSocket::readyRead, this, &QtServerApplication::readData);
    connect(m_tcpServerConnection, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
        this, &QtServerApplication::displayError);

    //m_tcpServerConnection->waitForReadyRead(3000);
}

void QtServerApplication::readData() {
    ui.txtDataLog->append(txt(tr("receive %1:%2 data: \n%3").arg(m_tcpServerConnection->peerAddress().toString())
        .arg(m_tcpServerConnection->peerPort())).arg(m_tcpServerConnection->readAll()));

}

void QtServerApplication::displayError(QAbstractSocket::SocketError socketError) {
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        ui.txtDataLog->append(txt(tr("clients %1:%2 offline").arg(m_tcpServerConnection->peerAddress().toString())
            .arg(m_tcpServerConnection->peerPort())));
        break;
    default:
        ui.txtDataLog->append(txt(tr("clients %1:%2 error: %3").arg(m_tcpServerConnection->peerAddress().toString())
            .arg(m_tcpServerConnection->peerPort())
            .arg(m_tcpServerConnection->errorString())));
    }
}

void QtServerApplication::displayAcceptError(QAbstractSocket::SocketError socketError) {
    switch (socketError) {
    default:
        ui.txtDataLog->append(txt(tr("accetpt error: %1").arg(m_tcpServer.errorString())));
    }
}



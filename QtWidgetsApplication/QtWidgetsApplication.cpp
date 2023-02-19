#include "QtWidgetsApplication.h"
#include <QtNetwork/QTcpSocket>
#include <QMessageBox>


QtWidgetsApplication::QtWidgetsApplication(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    m_tcpClient = new QTcpSocket(this);
    //connect(m_tcpClient, SIGNAL(connected()), this, SLOT(serverConnected()));
    //connect(m_tcpClient, SIGNAL(disconnected()), this, SLOT(serverDisconnected()));
    //connect(m_tcpClient, SIGNAL(bytesWritten(qint64)), this, SLOT(updateClientProgress(qint64)));
    //connect(m_tcpClient, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));

    connect(m_tcpClient, &QTcpSocket::connected, this, &QtWidgetsApplication::serverConnected);
    connect(m_tcpClient, &QTcpSocket::disconnected, this, &QtWidgetsApplication::serverDisconnected);
    connect(m_tcpClient, &QTcpSocket::bytesWritten, this, &QtWidgetsApplication::updateClientProgress);
    connect(m_tcpClient, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
        this, &QtWidgetsApplication::displayError);


    connect(ui.btnConnect, SIGNAL(clicked(bool)), this, SLOT(connectServer()));
    connect(ui.btnDisconnect, SIGNAL(clicked(bool)), this, SLOT(disconnectServer()));
    connect(ui.btnSend, SIGNAL(clicked(bool)), this, SLOT(sendMsg()));

}

QtWidgetsApplication::~QtWidgetsApplication()
{}


void QtWidgetsApplication::serverConnected() {
    ui.clientStatusLabel->setText(QString::fromLocal8Bit("连接成功"));
}

void QtWidgetsApplication::serverDisconnected() {

}

void QtWidgetsApplication::updateClientProgress(qint64 numBytes) {

}

void QtWidgetsApplication::displayError(QAbstractSocket::SocketError socketError) {
    switch (socketError) {
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Fortune Client"),
            tr("The host was not found. Please check the "
                "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Fortune Client"),
            tr("The connection was refused by the peer. "
                "Make sure the fortune server is running, "
                "and check that the host name and port "
                "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Fortune Client"),
            tr("The following error occurred: %1.")
            .arg(m_tcpClient->errorString()));
    }

    //qDebug() << m_tcpClient->errorString();
    //m_tcpClient->close();

}

void QtWidgetsApplication::connectServer() {
    ui.clientStatusLabel->setText(QString::fromLocal8Bit("连接中..."));
    QString ip = ui.txtIp->text();
    int port = ui.txtPort->text().toInt();
    m_tcpClient->connectToHost(ip, port);
    m_tcpClient->waitForConnected(3000);
    //if (m_tcpClient->waitForConnected(3000))
    //{
    //    qDebug() << "Connected!";

    //    // send
    //    m_tcpClient->write("hello server\r\n\r\n\r\n\r\n");
    //    m_tcpClient->waitForBytesWritten(1000);
    //    m_tcpClient->waitForReadyRead(3000);
    //    qDebug() << "Reading: " << m_tcpClient->bytesAvailable();

    //    qDebug() << m_tcpClient->readAll();

    //    m_tcpClient->close();
    //}
    //else
    //{
    //    qDebug() << "Not connected!";
    //}
}

void QtWidgetsApplication::disconnectServer() {
    ui.clientStatusLabel->setText(QString::fromLocal8Bit("断开连接..."));
    m_tcpClient->disconnectFromHost();
    //m_tcpClient->waitForDisconnected(3000);
}

void QtWidgetsApplication::sendMsg() {
    QString s =  ui.txtContent->toPlainText();
    m_tcpClient->write(s.toUtf8());
    m_tcpClient->waitForBytesWritten(1000);
    ui.clientStatusLabel->setText(QString::fromLocal8Bit("发送数据..."));
}
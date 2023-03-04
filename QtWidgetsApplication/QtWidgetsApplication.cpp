#include "QtWidgetsApplication.h"
#include <QtNetwork/QTcpSocket>
#include <QMessageBox>
#include <QFileDialog>
#include "Global.h"

QtWidgetsApplication::QtWidgetsApplication(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    m_payloadSize = 64 * 1024;

    //m_totalBytesToWrite = 0;
    //m_bytesToWrite = 0;
    //m_bytesWritten = 0;
    //m_sendFile = nullptr;

    //m_totalBytesReceived = 0;
    //m_bytesReceived = 0;
    //m_fileNameSize = 0;
    //m_receiveFile = nullptr;

    m_chatSocket = nullptr;

    m_fileSocket = new QTcpSocket(this);
    //connect(m_tcpClient, SIGNAL(connected()), this, SLOT(serverConnected()));
    //connect(m_tcpClient, SIGNAL(disconnected()), this, SLOT(serverDisconnected()));
    //connect(m_tcpClient, SIGNAL(bytesWritten(qint64)), this, SLOT(updateClientProgress(qint64)));
    //connect(m_tcpClient, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));

    connect(m_fileSocket, &QTcpSocket::connected, this, &QtWidgetsApplication::serverConnected);
    connect(m_fileSocket, &QTcpSocket::disconnected, this, &QtWidgetsApplication::serverDisconnected);

    // 发送文件
    connect(m_fileSocket, &QTcpSocket::bytesWritten, this, &QtWidgetsApplication::handleBytesWritten);

    // 接收文件
    connect(m_fileSocket, &QAbstractSocket::readyRead, this, &QtWidgetsApplication::handleReadData);

    // 错误处理
    connect(m_fileSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
        this, &QtWidgetsApplication::displayError);


    connect(ui.btnConnect, SIGNAL(clicked(bool)), this, SLOT(connectServer()));
    connect(ui.btnDisconnect, SIGNAL(clicked(bool)), this, SLOT(disconnectServer()));
    connect(ui.btnSendMsg, SIGNAL(clicked(bool)), this, SLOT(sendMsg()));
    connect(ui.btnOpenFile, SIGNAL(clicked(bool)), this, SLOT(openFile()));
    connect(ui.btnSendFile, SIGNAL(clicked(bool)), this, SLOT(sendFile()));
}

QtWidgetsApplication::~QtWidgetsApplication()
{}


void QtWidgetsApplication::serverConnected() {
    //m_fileSocket->waitForReadyRead();
    ui.clientStatusLabel->setText(QString::fromLocal8Bit("连接成功"));
}

void QtWidgetsApplication::serverDisconnected() {

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
            .arg(m_fileSocket->errorString()));
    }

    //qDebug() << m_tcpClient->errorString();
    //m_tcpClient->close();

}

void QtWidgetsApplication::connectServer() {
    ui.clientStatusLabel->setText(QString::fromLocal8Bit("连接中..."));
    QString ip = ui.txtIp->text();
    int port = ui.txtPort->text().toInt();
    m_fileSocket->connectToHost(ip, port);
    m_fileSocket->waitForConnected(3000);
}

void QtWidgetsApplication::disconnectServer() {
    ui.clientStatusLabel->setText(QString::fromLocal8Bit("断开连接..."));
    m_fileSocket->disconnectFromHost();
    m_fileSocket->waitForDisconnected(3000);
}

void QtWidgetsApplication::sendMsg() {    //ui.clientStatusLabel->setText(QString::fromLocal8Bit("发送数据..."));
    if (m_fileSocket && m_fileSocket->state() == QAbstractSocket::ConnectedState) {
        QString s = ui.txtContent->text();

        QDataStream out(&m_outBlock, QIODevice::WriteOnly);
        out << QString("MSG:").toUtf8() << s.toUtf8();
        m_fileSocket->write(m_outBlock);
        m_fileSocket->flush();
        ui.txtLog->append(txt(tr("send msg, msg: %1").arg(s)));
        m_outBlock.resize(0);
    }
}

void QtWidgetsApplication::openFile() {
    QString filePath = QFileDialog::getOpenFileName(this, "Open File", QDir::homePath());
    if (!filePath.isEmpty()) {
        m_sendFilePath = filePath;

        QFileInfo fileInfo(m_sendFilePath);
        m_sendFileName = fileInfo.fileName();
        ui.txtSendFileName->setText(m_sendFileName);
        ui.txtLog->append(txt(tr("Open File, File Name: %1").arg(m_sendFileName)));
    }
}


void QtWidgetsApplication::sendFile() {
    if (m_fileSocket && m_fileSocket->state() == QAbstractSocket::ConnectedState && !m_sendFilePath.isEmpty()) {
        QFile file(m_sendFilePath);
        if (!file.open(QFile::ReadOnly)) {
            ui.txtLog->append(txt(tr("open file error, file name: %1").arg(m_sendFileName)));
            return;
        }

        QByteArray data = file.readAll();
        file.close();

        QDataStream out(&m_outBlock, QIODevice::WriteOnly);
        out << QString("FILE:").toUtf8() << m_sendFileName.toUtf8() << file.size() << data;
        m_fileSocket->write(m_outBlock);
        m_fileSocket->flush();
        ui.txtLog->append(txt(tr("send file, file name: %1").arg(m_sendFileName)));
        m_outBlock.resize(0);
        //m_totalBytesToWrite = m_sendFile->size();
        //QDataStream out(&m_outBlock, QIODevice::WriteOnly);
        //out << quint64(0) << quint64(0) << m_sendFileName;
        //m_totalBytesToWrite += m_outBlock.size();

        //out.device()->seek(0);
        //out << m_totalBytesToWrite << quint64(m_outBlock.size() - sizeof(quint64) * 2);
        //m_bytesToWrite = m_totalBytesToWrite - m_fileSocket->write(m_outBlock);
        //m_fileSocket->waitForBytesWritten();
        //ui.txtLog->append(txt(tr("Send File, File Name: %1").arg(m_sendFileName)));
        //m_outBlock.resize(0);
    }
}


void QtWidgetsApplication::handleBytesWritten(qint64 numBytes) {
    //m_bytesWritten += numBytes;
    //if (m_bytesToWrite > 0) {
    //    m_outBlock = m_sendFile->read(qMin(m_bytesToWrite, m_payloadSize));
    //    m_bytesToWrite -= m_fileSocket->write(m_outBlock);
    //    m_fileSocket->waitForBytesWritten();
    //    m_outBlock.resize(0);
    //}
    //else {
    //    m_sendFile->close();
    //}

    //ui.fileProgressBar->setMaximum(m_totalBytesToWrite);
    //ui.fileProgressBar->setValue(m_bytesWritten);

    //if (m_bytesWritten == m_totalBytesToWrite) {
    //    m_sendFile->close();
    //    m_totalBytesToWrite = m_bytesToWrite = m_bytesWritten = 0;
    //}

}

void QtWidgetsApplication::handleReadData() {
    QDataStream in(m_fileSocket);
    QByteArray flag;
    in >> flag;
    if (flag == QString("FILE:").toUtf8()) {
        QByteArray fileNameArray;
        qint64 fileSize;
        in >> fileNameArray >> fileSize;
        QString fileName = QString::fromUtf8(fileNameArray);
        QFile file(fileName);
        if (file.open(QFile::WriteOnly)) {
            QByteArray fileData;
            in >> fileData;
            file.write(fileData);
            file.close();
        }
    }

    //QDataStream in(m_fileSocket);
    //if (m_bytesReceived <= sizeof(quint64) * 2) {
    //    if (m_fileSocket->bytesAvailable() >= sizeof(quint64) * 2 && m_fileNameSize == 0) {
    //        in >> m_totalBytesReceived >> m_fileNameSize;
    //        m_bytesReceived += sizeof(quint64) * 2;
    //    }
    //    if (m_fileSocket->bytesAvailable() >= m_fileNameSize && m_fileNameSize != 0) {
    //        in >> m_fileName;
    //        m_bytesReceived += m_fileNameSize;
    //        m_receiveFile = new QFile(m_fileName);
    //        ui.txtReceiveFileName->setText(m_fileName);
    //        if (!m_receiveFile->open(QFile::WriteOnly)) {
    //            qDebug() << "receive: open file error!";
    //            return;
    //        }
    //    }
    //    else {
    //        return;
    //    }
    //}

    //if (m_bytesReceived < m_totalBytesReceived) {
    //    m_bytesReceived += m_fileSocket->bytesAvailable();
    //    m_inBlock = m_fileSocket->readAll();
    //    m_receiveFile->write(m_inBlock);
    //    m_inBlock.resize(0);
    //}

    //ui.fileProgressBar->setMaximum(m_totalBytesReceived);
    //ui.fileProgressBar->setValue(m_bytesReceived);

    //if (m_bytesReceived == m_totalBytesReceived) {
    //    m_receiveFile->close();
    //    m_bytesReceived = m_totalBytesReceived = m_fileNameSize = 0;
    //    m_fileName.clear();
    //}
}


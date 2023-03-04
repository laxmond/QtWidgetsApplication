#include "TcpSocketConnection.h"
#include <QDateTime>

QString txt(const char* s) {
	return QObject::tr("[%1]# %2\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"), QString::fromLocal8Bit(s));
}

QString txt(const QString& s) {
	return QObject::tr("[%1]# %2\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"), s);
}

TcpSocketConnection::TcpSocketConnection(QTcpSocket* tcpSocket)
{
	m_tcpServerConnection = tcpSocket;

	m_socketDescriptor = tcpSocket->socketDescriptor();

	m_localAddress = tcpSocket->localAddress();
	m_localPort = tcpSocket->localPort();

	m_peerAddress = tcpSocket->peerAddress();
	m_peerPort = tcpSocket->peerPort();

	connect(m_tcpServerConnection, &QAbstractSocket::readyRead, this, &TcpSocketConnection::handleReadData);
	connect(m_tcpServerConnection, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
		this, &TcpSocketConnection::handleError);
}

TcpSocketConnection::~TcpSocketConnection()
{

}

void TcpSocketConnection::write(QString writeString)
{
	m_tcpServerConnection->write(writeString.toUtf8());
	m_tcpServerConnection->flush();
	QString logString = txt(tr("send %1:%2 data length: %3").arg(m_peerAddress.toString())
		.arg(m_peerPort).arg(writeString.size()));
	emit log(logString);
}

void TcpSocketConnection::write(QByteArray writeData)
{
	m_tcpServerConnection->write(writeData);
	m_tcpServerConnection->flush();
	QString logString = txt(tr("send %1:%2 data length: %3").arg(m_peerAddress.toString())
		.arg(m_peerPort).arg(writeData.size()));
	emit log(logString);
}

void TcpSocketConnection::handleReadData()
{
	QByteArray data = m_tcpServerConnection->readAll();

	// 处理数据
	emit process(m_tcpServerConnection, data);

	// 记录日志
	QString logString = txt(tr("receive %1:%2 data: \n%3").arg(m_peerAddress.toString())
		.arg(m_peerPort).arg(data.size()));
	emit log(logString);
}

void TcpSocketConnection::handleError(QAbstractSocket::SocketError socketError)
{
	switch (socketError) {
		case QAbstractSocket::RemoteHostClosedError: {
			QString logString = txt(tr("clients %1:%2 offline").arg(m_peerAddress.toString())
				.arg(m_peerPort));
			emit log(logString);
			break;
		}
		default: {
			QString errorString = m_tcpServerConnection->errorString();
			QString logString = txt(tr("clients %1:%2 error: %3").arg(m_peerAddress.toString())
				.arg(m_peerPort).arg(errorString));
			emit log(logString);
		}
	}
	emit disconnected(m_socketDescriptor);
}

#pragma once
#include <QtNetwork/QTcpSocket>

extern QString txt(const char* s);

extern QString txt(const QString& s);

class TcpSocketConnection: public QObject
{
	Q_OBJECT
public:
	TcpSocketConnection(QTcpSocket* tcpSocket);
	~TcpSocketConnection();

	void write(QString);
	void write(QByteArray);

public Q_SLOTS:
	void handleReadData();
	void handleError(QAbstractSocket::SocketError);

Q_SIGNALS:
	void process(QTcpSocket* ,QByteArray&);
	void log(QString);
	void disconnected(qintptr);

private:
	QTcpSocket* m_tcpServerConnection = nullptr;
	QHostAddress m_localAddress;
	quint16 m_localPort;
	QHostAddress m_peerAddress;
	quint16 m_peerPort;
	qintptr m_socketDescriptor;

};


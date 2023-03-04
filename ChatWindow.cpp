#include "chatwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFileInfo>
#include <QDataStream>
#include <QFileDialog>

ChatWindow::ChatWindow(QString title, QWidget* parent)
    : QWidget(parent), m_title(title), m_tcpSocket(nullptr), m_fileSocket(nullptr), m_file(nullptr)
{
    setupUI();
    createConnection();
}

ChatWindow::~ChatWindow()
{
    if (m_tcpSocket) {
        m_tcpSocket->disconnectFromHost();
        m_tcpSocket->deleteLater();
    }
    if (m_fileSocket) {
        m_fileSocket->disconnectFromHost();
        m_fileSocket->deleteLater();
    }
}

void ChatWindow::setupUI()
{
    setWindowTitle(m_title);

    m_ipLineEdit = new QLineEdit("127.0.0.1");
    m_portLineEdit->setValidator(new QIntValidator(0, 65535));
    m_chatTextEdit = new QTextEdit;
    m_chatTextEdit->setReadOnly(true);
    m_inputLineEdit = new QLineEdit;
    m_sendMessageButton = new QPushButton("Send");
    m_fileListWidget = new QListWidget;
    m_fileOpenButton = new QPushButton("Open");
    m_sendFileButton = new QPushButton("Send");
    m_sendFileButton->setEnabled(false);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_chatTextEdit);
    QHBoxLayout* inputLayout = new QHBoxLayout;
    inputLayout->addWidget(m_inputLineEdit);
    inputLayout->addWidget(m_sendMessageButton);
    mainLayout->addLayout(inputLayout);
    mainLayout->addWidget(m_fileListWidget);
    QHBoxLayout* fileLayout = new QHBoxLayout;
    fileLayout->addWidget(m_fileOpenButton);
    fileLayout->addWidget(m_sendFileButton);
    mainLayout->addLayout(fileLayout);
    setLayout(mainLayout);

    connect(m_sendMessageButton, &QPushButton::clicked, this, &ChatWindow::onSendMessageClicked);
    connect(&m_tcpServer, &QTcpServer::newConnection, this, &ChatWindow::onNewConnection);
    connect(m_fileOpenButton, &QPushButton::clicked, this, &ChatWindow::onFileOpenClicked);
    connect(m_sendFileButton, &QPushButton::clicked, this, &ChatWindow::onSendFileClicked);
}

void ChatWindow::createConnection()
{
    if (m_tcpSocket) {
        m_tcpSocket->disconnectFromHost();
        m_tcpSocket->deleteLater();
        m_tcpSocket = nullptr;
    }
    m_tcpSocket = new QTcpSocket(this);
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &ChatWindow::onReadyRead);
    connect(m_tcpSocket, &QTcpSocket::connected, [this]() {
        m_chatTextEdit->append("Connected to server.");
        });
    connect(m_tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred), [this](QAbstractSocket::SocketError socketError) {
        m_chatTextEdit->append("Socket Error: " + m_tcpSocket->errorString());
        });
}

void ChatWindow::onSendMessageClicked()
{
    if (m_tcpSocket && m_tcpSocket->state() == QAbstractSocket::ConnectedState) {
        QString message = m_inputLineEdit->text().trimmed();
        if (!message.isEmpty()) {
            m_tcpSocket->write(message.toUtf8());
            m_chatTextEdit->append("Me: " + message);
            m_inputLineEdit->clear();
        }
    }
}
  

void ChatWindow::onNewConnection()
{
    if (!m_tcpSocket) {
        m_tcpSocket = m_tcpServer.nextPendingConnection();
        connect(m_tcpSocket, &QTcpSocket::readyRead, this, &ChatWindow::onReadyRead);
        connect(m_tcpSocket, &QTcpSocket::disconnected, [this]() {
            m_chatTextEdit->append("Disconnected from client.");
            m_tcpSocket->deleteLater();
            m_tcpSocket = nullptr;
        });
        connect(m_tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred), [this](QAbstractSocket::SocketError socketError) {
            m_chatTextEdit->append("Socket Error: " + m_tcpSocket->errorString());
            });
        m_chatTextEdit->append("Connected to client.");
    }
}

void ChatWindow::onReadyRead()
{
    if (m_tcpSocket && m_tcpSocket->state() == QAbstractSocket::ConnectedState)
    {
        QByteArray message = m_tcpSocket->readAll();
        m_chatTextEdit->append("Client: " + QString::fromUtf8(message));
    }
}

void ChatWindow::onFileOpenClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open File", QDir::homePath());
    if (!filePath.isEmpty()) {
        m_filePath = filePath;
        m_fileListWidget->addItem(m_filePath.split('/').last());
        m_sendFileButton->setEnabled(true);
    }
}

void ChatWindow::onSendFileClicked()
{
    if (m_tcpSocket && m_tcpSocket->state() == QAbstractSocket::ConnectedState && !m_filePath.isEmpty()) {
        QFile file(m_filePath);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray fileData = file.readAll();
            QFileInfo fileInfo(m_filePath);
            QString fileName = fileInfo.fileName();
            QString message = QString("%1:%2").arg(fileName).arg(QString(fileData.toBase64()));
            m_tcpSocket->write(message.toUtf8());
            m_chatTextEdit->append(QString("Me: File %1 sent.").arg(fileName));
            file.close();
            m_filePath.clear();
            m_fileListWidget->clear();
            m_sendFileButton->setEnabled(false);
        }
        else {
            m_chatTextEdit->append("Failed to open file.");
        }
    }
}
#include "tcpsocketclient.h"

#include <QDataStream>
#include <QHostAddress>
#include <QTcpSocket>

TcpSocketClient::TcpSocketClient(QObject *parent) :
    QObject(parent),
    m_socket(new QTcpSocket(this)),
    m_ipAddress(""),
    m_port(0),
    m_sizeAnswerAsynchrone(0)
{
    connect(m_socket, &QTcpSocket::connected, this, &TcpSocketClient::connected);
    connect(m_socket, &QTcpSocket::disconnected, this, &TcpSocketClient::disconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpSocketClient::onReadyRead_Socket);
}

TcpSocketClient::~TcpSocketClient()
{
    delete m_socket;
}

/**************************************
 ***        GETTERS / SETTERS       ***
 *************************************/
bool TcpSocketClient::isConnected()
{
    return m_socket->isOpen();
}

/**************************************
 ***        PUBLIC FUNCTIONS        ***
 *************************************/
void TcpSocketClient::tryToConnect()
{
    if((m_ipAddress != "") && (m_port > 0))
    {
        m_socket->abort();
        m_socket->connectToHost(QHostAddress(m_ipAddress), m_port);
    }
}

bool TcpSocketClient::sendMessage(const QString &message)
{
    bool success = false;

    if(m_socket != nullptr)
    {
        //init request
        QByteArray requestToSend;
        QDataStream dataToSend(&requestToSend, QIODevice::WriteOnly);
        dataToSend << message.length();
        dataToSend << message;

        qint64 byteWritten = m_socket->write(requestToSend);
        if(byteWritten == requestToSend.length())
            success = true;
        else
            qCritical() << __PRETTY_FUNCTION__ << "error for sending, byteWritten:" << byteWritten << ", message length:" << message.length();
    }
    else
        qCritical() << __PRETTY_FUNCTION__ << "client is nullptr";

    return success;
}

/**************************************
 ***     PRIVATE  SLOTS FUNCTIONS   ***
 *************************************/
void TcpSocketClient::onReadyRead_Socket()
{
    qDebug() << __PRETTY_FUNCTION__;

    //init the answer
    QDataStream requestToRead(m_socket);

    if(m_sizeAnswerAsynchrone == 0)
    {
        //Check we have the minimum to start reading
        if(m_socket->bytesAvailable() < sizeof(quint64))
            return;

        //Check we have all the answer
        requestToRead >> m_sizeAnswerAsynchrone;
    }

    if(m_socket->bytesAvailable() < m_sizeAnswerAsynchrone)
        return;

    //From here, we have everything, so we can get all the message
    QString message;
    requestToRead >> message;

    m_sizeAnswerAsynchrone = 0;
    emit newMessage(message);
}

/**************************************
 ***        PRIVATE FUNCTIONS       ***
 *************************************/

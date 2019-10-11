#include "tcpsocketclient.h"

#include <QDataStream>
#include <QHostAddress>
#include <QTcpSocket>

TcpSocketClient::TcpSocketClient(QObject *parent) :
    QObject(parent),
    m_socketIn(nullptr),
    m_socketOut(new QTcpSocket(this)),
    m_ipAddress(""),
    m_port(0),
    m_sizeAnswerAsynchrone(0)
{
    connect(m_socketOut, &QTcpSocket::connected, this, &TcpSocketClient::connected);
    connect(m_socketOut, &QTcpSocket::disconnected, this, &TcpSocketClient::disconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpSocketClient::onReadyRead_SocketIn);
}

TcpSocketClient::~TcpSocketClient()
{
    deleteSocketIn();
    deleteSocketOut();
}

/**************************************
 ***        GETTERS / SETTERS       ***
 *************************************/
void TcpSocketClient::setSocketIn(QTcpSocket *socket)
{
    if(socket != nullptr)
    {
        deleteSocketIn();

        m_socketIn = socket;
        connect(m_socketIn, &QTcpSocket::readyRead, this, &TcpSocketClient::onReadyRead_SocketIn);
    }
}

bool TcpSocketClient::isConnected()
{
    bool connected = false;

    if((m_socketIn != nullptr) && (m_socketIn->isOpen())
            && (m_socketOut != nullptr) && (m_socketOut->isOpen()))
    {
        connected = true;
    }

    return connected;
}

/**************************************
 ***        PUBLIC FUNCTIONS        ***
 *************************************/
void TcpSocketClient::tryToConnect()
{
    if((m_ipAddress != "") && (m_port > 0))
    {
        m_socketOut->abort();
        m_socketOut->connectToHost(QHostAddress(m_ipAddress), m_port);
    }
}

bool TcpSocketClient::sendMessage(const QString &message)
{
    bool success = false;

    if(m_socketOut != nullptr)
    {
        //init request
        QByteArray requestToSend;
        QDataStream dataToSend(&requestToSend, QIODevice::WriteOnly);
        dataToSend << message.length();
        dataToSend << message;

        qint64 byteWritten = m_socketOut->write(requestToSend);
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
void TcpSocketClient::onReadyRead_SocketIn()
{
    qDebug() << __PRETTY_FUNCTION__;

    //init the answer
    QDataStream requestToRead(m_socketIn);

    if(m_sizeAnswerAsynchrone == 0)
    {
        //Check we have the minimum to start reading
        if(m_socketIn->bytesAvailable() < sizeof(quint64))
            return;

        //Check we have all the answer
        requestToRead >> m_sizeAnswerAsynchrone;
    }

    if(m_socketIn->bytesAvailable() < m_sizeAnswerAsynchrone)
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
void TcpSocketClient::deleteSocketIn()
{
    if(m_socketIn != nullptr)
    {
        m_socketIn->abort();
        delete m_socketIn;
    }
}

void TcpSocketClient::deleteSocketOut()
{
    if(m_socketOut != nullptr)
    {
        m_socketOut->abort();
        delete m_socketOut;
    }
}

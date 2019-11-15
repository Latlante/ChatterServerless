#include "tcpsocketclient.h"

#include <QDataStream>
#include <QHostAddress>
#include <QTcpSocket>

TcpSocketClient::TcpSocketClient(const QHostAddress &address, const quint16 port, QObject *parent) :
    QObject(parent),
    m_socketIn(nullptr),
    m_socketOut(new QTcpSocket(this)),
    m_ipAddress(address),
    m_port(port),
    m_sizeAnswerAsynchrone(0)
{
    connect(m_socketOut, &QTcpSocket::connected, this, &TcpSocketClient::connected);
    connect(m_socketOut, &QTcpSocket::disconnected, this, &TcpSocketClient::disconnected);
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
        if(socket->peerAddress() == ipAddress())
        {
            deleteSocketIn();

            m_socketIn = socket;
            connect(m_socketIn, &QTcpSocket::readyRead, this, &TcpSocketClient::onReadyRead_SocketIn, Qt::UniqueConnection);
        }
        else
            qCritical() << __PRETTY_FUNCTION__
                        << "ip address of the socket is incorrect, socketIn="
                        << socket->peerAddress().toString()
                        << ", localSocket=" << ipAddress().toString();
    }
    else
        qCritical() << __PRETTY_FUNCTION__ << "socket is nullptr";
}

QHostAddress TcpSocketClient::ipAddress()
{
    /*QHostAddress address;

    if(m_socketIn != nullptr)
        address = m_socketIn->peerAddress();
    else if(m_socketOut != nullptr)
        address = m_socketOut->peerAddress();

    return address;*/

    return m_ipAddress;
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
bool TcpSocketClient::contains(qintptr socketDescriptor)
{
    bool found = false;

    if(m_socketIn != nullptr)
        found = m_socketIn->socketDescriptor() == socketDescriptor;

    if((found == false) && (m_socketOut != nullptr))
        found = m_socketOut->socketDescriptor() == socketDescriptor;

    return found;
}

void TcpSocketClient::tryToConnect()
{
    if((m_ipAddress.isNull() == false) && (m_port > 1000))
    {
        m_socketOut->abort();
        m_socketOut->connectToHost(QHostAddress(m_ipAddress), m_port);
    }
    else
        qCritical() << __PRETTY_FUNCTION__
                    << "Address:" << ipAddress().toString()
                    << "and/or port:" << m_port
                    << "is/are wrong";
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

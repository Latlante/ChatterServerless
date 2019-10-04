#include "tcpserverclients.h"

#include <QDataStream>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>

#include "dataGeodrone.h"

TcpServerClients::TcpServerClients(QObject *parent) :
    QObject(parent),
    m_tcpServer(new QTcpServer(this)),
    m_listClients()
{
    connect(m_tcpServer, &QTcpServer::newConnection, this, &TcpServerClients::onNewConnection_TcpServer);
}

TcpServerClients::~TcpServerClients()
{

}

/**************************************
 ***        GETTERS / SETTERS       ***
 *************************************/


/**************************************
 ***        PUBLIC FUNCTIONS        ***
 *************************************/
bool TcpServerClients::start()
{
    bool success = false;

    if(m_tcpServer)
    {
        if(m_tcpServer->listen(QHostAddress::Any, TCPSERVER_PORT))
        {
            qDebug() << "Server listening...";
            success = true;
        }
        else
        {
            qDebug() << "Server not started: " << m_tcpServer->errorString();
        }
    }

    return success;
}

bool TcpServerClients::isRunning()
{
    bool running = false;

    if(m_tcpServer)
        running = m_tcpServer->isListening();

    return running;
}

bool TcpServerClients::newMessage(int socketDescriptor, QByteArray message)
{
    QTcpSocket* client = socketFromSocketDescriptor(socketDescriptor);
    return newMessage(client, message);
}

bool TcpServerClients::newMessage(QTcpSocket* client, QByteArray message)
{
    bool success = false;

    if(client != nullptr)
    {
        qint64 byteWritten = client->write(message);
        if(byteWritten == message.length())
            success = true;
        else
            qCritical() << __PRETTY_FUNCTION__ << "error for sending, byteWritten:" << byteWritten << ", message length:" << message.length();
    }
    else
        qCritical() << __PRETTY_FUNCTION__ << "client is nullptr";

    return success;
}

/**************************************
 ***       PROTECTED FUNCTIONS      ***
 *************************************/
void TcpServerClients::onNewConnection_TcpServer()
{
    qDebug() << __PRETTY_FUNCTION__;
    QTcpSocket* newClient = m_tcpServer->nextPendingConnection();
    m_listClients.insert(newClient, { newClient->socketDescriptor(), 0 });

    connect(newClient, &QTcpSocket::readyRead, this, &TcpServerClients::onReadyRead_Client);
    connect(newClient, &QTcpSocket::disconnected, this, &TcpServerClients::onDisconnected_Client);

    emit newConnection(newClient->socketDescriptor(), newClient->peerAddress().toString());
}

void TcpServerClients::onReadyRead_Client()
{
    qDebug() << __PRETTY_FUNCTION__;

    //init the answer
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());

    if(client != nullptr)
    {
        if(m_listClients.contains(client) == false)
        {
            qCritical() << __PRETTY_FUNCTION__ << "client is not in the list";
            return;
        }

        QByteArray message = client->readAll();
        emit newMessageReceived(client->socketDescriptor(), message);
    }
}

void TcpServerClients::onDisconnected_Client()
{
    qDebug() << __PRETTY_FUNCTION__;
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if(client)
    {
        emit newDisconnection(client->socketDescriptor());
        m_listClients.remove(client);
        client->deleteLater();
    }
}

/**************************************
 ***        PRIVATE FUNCTIONS       ***
 *************************************/
QTcpSocket* TcpServerClients::socketFromSocketDescriptor(int socketDescriptor)
{
    QTcpSocket *socket = nullptr;
    int indexLoop = 0;

    QList<QTcpSocket*> listKeys = m_listClients.keys();
    while((socket == nullptr) && (indexLoop < listKeys.count()))
    {
        if(m_listClients.value(listKeys[indexLoop]).socketDescriptor == socketDescriptor)
            socket = listKeys[indexLoop];

        indexLoop++;
    }

    return socket;
}

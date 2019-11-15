#include "tcpserverclients.h"

#include <QDataStream>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>

TcpServerClients::TcpServerClients(QObject *parent) :
    QObject(parent),
    m_tcpServer(new QTcpServer(this))
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
        if(m_tcpServer->listen(QHostAddress::Any, 44800))
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

/**************************************
 ***       PROTECTED FUNCTIONS      ***
 *************************************/
void TcpServerClients::onNewConnection_TcpServer()
{
    qDebug() << __PRETTY_FUNCTION__;
    QTcpSocket* newClient = m_tcpServer->nextPendingConnection();

    emit newConnection(newClient->socketDescriptor(), newClient->peerAddress());
}

/**************************************
 ***        PRIVATE FUNCTIONS       ***
 *************************************/

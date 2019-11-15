#include "contactshandler.h"

#include <QTcpSocket>
#include "tcpserverclients.h"
#include "tcpsocketclient.h"

const quint16 ContactsHandler::m_PORT_CONNECTION = 5000;

ContactsHandler::ContactsHandler(QObject *parent) :
    QObject(parent),
    m_serverListening(nullptr),
    m_listContacts()
{
    initTcpServerClient();
}

ContactsHandler::~ContactsHandler()
{
    if(m_serverListening != nullptr)
    {
        delete m_serverListening;
        m_serverListening = nullptr;
    }
}

/**************************************
 ***        GETTERS / SETTERS       ***
 *************************************/


/**************************************
 ***        PUBLIC FUNCTIONS        ***
 *************************************/
void ContactsHandler::addNewContact(const QHostAddress &address, const QString &name)
{
    TcpSocketClient* contact = new TcpSocketClient(address,
                                                   m_PORT_CONNECTION,
                                                   this);
    contact->tryToConnect();

    m_listContacts.push_back(contact);
}

/**************************************
 ***       PROTECTED FUNCTIONS      ***
 *************************************/

/**************************************
 ***     PRIVATE SLOTS FUNCTIONS    ***
 *************************************/
void ContactsHandler::onNewConnection_ServerListening(qintptr socketDescriptor, const QHostAddress& ipAddress)
{
    qDebug() << __PRETTY_FUNCTION__ << "from" << ipAddress.toString();

    //If we are already connected to the contact, we can complete the class
    if(contactIsAlreadyCreated(ipAddress) == true)
    {
        addSocketInToExistingContact(socketDescriptor, ipAddress);
    }
    //Not connected yet, so new contact
    else
    {
        addNewContact(socketDescriptor);
    }
}

/**************************************
 ***        PRIVATE FUNCTIONS       ***
 *************************************/
void ContactsHandler::initTcpServerClient()
{
    m_serverListening = new TcpServerClients(this);
    connect(m_serverListening, &TcpServerClients::newConnection, this, &ContactsHandler::onNewConnection_ServerListening);
    m_serverListening->start();
}

void ContactsHandler::addNewContact(qintptr socketDescriptor)
{
    QTcpSocket* socketInNewContact = new QTcpSocket();
    socketInNewContact->setSocketDescriptor(socketDescriptor);
    if(socketInNewContact->isValid() == true)
    {
        TcpSocketClient* contact = new TcpSocketClient(socketInNewContact->peerAddress(),
                                                       m_PORT_CONNECTION,
                                                       this);
        contact->setSocketIn(socketInNewContact);
        contact->tryToConnect();

        m_listContacts.push_back(contact);
    }
    else
        qCritical() << __PRETTY_FUNCTION__ << "socket descriptor" << socketDescriptor << "is invalid";

}

void ContactsHandler::addSocketInToExistingContact(qintptr socketDescriptorIn, const QHostAddress &ipAddress)
{
    int indexContact = indexOfContactFromIpAddress(ipAddress);
    if((indexContact >= 0) && (indexContact < m_listContacts.count()))
    {
        QTcpSocket* socketIn = new QTcpSocket();
        socketIn->setSocketDescriptor(socketDescriptorIn);
        if(socketIn->isValid() == true)
        {
            TcpSocketClient* contact = m_listContacts.value(indexContact);
            if(contact != nullptr)
                contact->setSocketIn(socketIn);
            else
                qCritical() << __PRETTY_FUNCTION__ << "contact is nullptr";
        }
        else
            qCritical() << __PRETTY_FUNCTION__ << "socketInNewContact is invalid";
    }
    else
        qCritical() << __PRETTY_FUNCTION__ << "index incorrect" << indexContact << "/" << m_listContacts.count();
}

bool ContactsHandler::contactIsAlreadyCreated(const QHostAddress &address)
{
    return indexOfContactFromIpAddress(address) >= 0;
}

int ContactsHandler::indexOfContactFromIpAddress(const QHostAddress &address)
{
    int indexList = -1;
    int indexLoop = 0;

    while((indexList == -1) && (indexLoop < m_listContacts.count()))
    {
        TcpSocketClient* contact = m_listContacts.value(indexLoop);
        if(contact != nullptr)
        {
            if(contact->ipAddress() == address)
                indexList = indexLoop;
        }
    }

    return indexList;
}

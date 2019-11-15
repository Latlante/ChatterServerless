#ifndef CONTACTSHANDLER_H
#define CONTACTSHANDLER_H

#include <QHostAddress>
#include <QObject>

class TcpServerClients;
class TcpSocketClient;

class ContactsHandler : public QObject
{
    Q_OBJECT
public:
    explicit ContactsHandler(QObject *parent = nullptr);
    ~ContactsHandler();

    void addNewContact(const QHostAddress& address, const QString& name);

signals:

private slots:
    void onNewConnection_ServerListening(qintptr socketDescriptor, const QHostAddress& ipAddress);

private:
    static const quint16 m_PORT_CONNECTION;

    TcpServerClients* m_serverListening;
    QList<TcpSocketClient*> m_listContacts;

    void initTcpServerClient();
    void addNewContact(qintptr socketDescriptor);
    void addSocketInToExistingContact(qintptr socketDescriptorIn, const QHostAddress& ipAddress);
    bool contactIsAlreadyCreated(const QHostAddress& address);
    int indexOfContactFromIpAddress(const QHostAddress& address);
};

#endif // CONTACTSHANDLER_H

#ifndef TCPSERVERCLIENTS_H
#define TCPSERVERCLIENTS_H


#include <QMap>

#include <QObject>

class QTcpServer;
class QTcpSocket;

class TcpServerClients : public QObject
{
    Q_OBJECT
public:
    explicit TcpServerClients(QObject *parent = nullptr);
    ~TcpServerClients();

    bool start();
    bool isRunning();

    bool newMessage(int socketDescriptor, QByteArray message);
    bool newMessage(QTcpSocket *client, QByteArray message);

signals:
    void newConnection(qintptr, const QString&);
    void newDisconnection(qintptr);
    void newMessageReceived(qintptr,QByteArray);

private slots:
    void onNewConnection_TcpServer();
    void onReadyRead_Client();
    void onDisconnected_Client();

private:
    struct InfoClient
    {
        qintptr socketDescriptor;
        quint16 sizeBuffer;
    };
    QTcpServer* m_tcpServer;
    QMap<QTcpSocket*, InfoClient> m_listClients;

    QTcpSocket* socketFromSocketDescriptor(int socketDescriptor);
};

#endif // TCPSERVERCLIENTS_H

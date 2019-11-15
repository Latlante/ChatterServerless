#ifndef TCPSERVERCLIENTS_H
#define TCPSERVERCLIENTS_H

#include <QHostAddress>
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

signals:
    void newConnection(qintptr, const QHostAddress&);

private slots:
    void onNewConnection_TcpServer();

private:
    QTcpServer* m_tcpServer;
};

#endif // TCPSERVERCLIENTS_H

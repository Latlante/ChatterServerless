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

signals:
    void newConnection(qintptr, const QString&);

private slots:
    void onNewConnection_TcpServer();

private:
    QTcpServer* m_tcpServer;
};

#endif // TCPSERVERCLIENTS_H

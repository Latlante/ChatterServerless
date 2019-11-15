#ifndef TCPSOCKETCLIENT_H
#define TCPSOCKETCLIENT_H

#include <QHostAddress>
#include <QObject>

class QTcpSocket;

class TcpSocketClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpSocketClient(const QHostAddress& address,
                             const quint16 port,
                             QObject *parent = nullptr);
    ~TcpSocketClient();

    void setSocketIn(QTcpSocket *socket);
    QHostAddress ipAddress();
    bool isConnected();

    bool contains(qintptr socketDescriptor);
    void tryToConnect();
    bool sendMessage(const QString& message);

signals:
    void connected();
    void disconnected();
    void newMessage(const QString&);

private slots:
    void onReadyRead_SocketIn();

private:
    QTcpSocket* m_socketIn;
    QTcpSocket* m_socketOut;
    QHostAddress m_ipAddress;
    quint16 m_port;

    qint64 m_sizeAnswerAsynchrone;

    void deleteSocketIn();
    void deleteSocketOut();
};

#endif // TCPSOCKETCLIENT_H

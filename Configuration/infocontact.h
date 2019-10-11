#ifndef INFOCONTACT_H
#define INFOCONTACT_H

#include <QObject>

class InfoContact
{
public:
    InfoContact();
    InfoContact(int id, const QString& name, const QString& ipAddress);

    int id();
    QString name();
    QString ipAddress();

    bool isOk();

private:
    int m_id;
    QString m_name;
    QString m_ipAddress;
};

#endif // INFOCONTACT_H

#ifndef CONFIGURATIONCONTACTS_H
#define CONFIGURATIONCONTACTS_H

#include <QObject>
#include "infocontact.h"

class ConfigurationContacts
{
public:
    enum ColumnCsvFile
    {
        COLUMN_ID = 0,
        COLUMN_IP_ADDRESS,
        COLUMN_NAME,
        COLUMN_COUNT
    };

    ConfigurationContacts();

    bool isOk();
    unsigned int countContacts();

    InfoContact contact(unsigned int index);
    int id(unsigned int index);
    QString ipAddress(unsigned int index);
    QString name(unsigned int index);

private:
    static const QString m_PATH_FILE_CONTACTS;

    bool m_isOk;
    QList<InfoContact> m_listInfoContacts;

    void loadFile();
};

#endif // CONFIGURATIONCONTACTS_H

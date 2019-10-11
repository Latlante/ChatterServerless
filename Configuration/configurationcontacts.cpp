#include "configurationcontacts.h"

#include <QDebug>
#include <QFile>

const QString ConfigurationContacts::m_PATH_FILE_CONTACTS = "Files/contacts.txt";

ConfigurationContacts::ConfigurationContacts() :
    m_isOk(false),
    m_listInfoContacts()
{
    loadFile();
}

/**************************************
 ***        GETTERS / SETTERS       ***
 *************************************/
bool ConfigurationContacts::isOk()
{
    return m_isOk;
}

unsigned int ConfigurationContacts::countContacts()
{
    return static_cast<unsigned int>(m_listInfoContacts.count());
}

InfoContact ConfigurationContacts::contact(unsigned int index)
{
    InfoContact info;

    if(index < countContacts())
        info = m_listInfoContacts.value(index);

    return info;
}

int ConfigurationContacts::id(unsigned int index)
{
    return contact(index).id();
}

QString ConfigurationContacts::ipAddress(unsigned int index)
{
    return contact(index).ipAddress();
}

QString ConfigurationContacts::name(unsigned int index)
{
    return contact(index).name();
}

/**************************************
 ***        PUBLIC FUNCTIONS        ***
 *************************************/


/**************************************
 ***     PRIVATE  SLOTS FUNCTIONS   ***
 *************************************/

/**************************************
 ***        PRIVATE FUNCTIONS       ***
 *************************************/
void ConfigurationContacts::loadFile()
{
    m_listInfoContacts.clear();

    QFile file(m_PATH_FILE_CONTACTS);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //read the file
        const QString contentFile = file.readAll();
        file.close();

        //split to get list of each line
        const QStringList listOfLines = contentFile.split('\n');
        foreach(const QString line, listOfLines)
        {
            const QStringList listElementInLine = line.split(';');
            //check if we have all columns we need
            if(listElementInLine.count() >= static_cast<int>(COLUMN_COUNT))
            {
                const int id = listElementInLine.value(static_cast<int>(COLUMN_ID)).toInt();
                const QString ipAddress = listElementInLine.value(static_cast<int>(COLUMN_IP_ADDRESS));
                const QString name = listElementInLine.value(static_cast<int>(COLUMN_NAME));

                m_listInfoContacts.push_back(InfoContact(id, name, ipAddress));
            }
            else
                qWarning() << __PRETTY_FUNCTION__ << "line:" << line << "has incorrect format";
        }

        m_isOk = true;
    }
}

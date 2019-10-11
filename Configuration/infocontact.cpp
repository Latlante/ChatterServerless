#include "infocontact.h"

InfoContact::InfoContact() :
    m_id(-1),
    m_name(""),
    m_ipAddress("")
{

}

InfoContact::InfoContact(int id, const QString &name, const QString &ipAddress) :
    m_id(id),
    m_name(name),
    m_ipAddress(ipAddress)
{

}

/**************************************
 ***        GETTERS / SETTERS       ***
 *************************************/
int InfoContact::id()
{
    return m_id;
}

QString InfoContact::name()
{
    return m_name;
}

QString InfoContact::ipAddress()
{
    return m_ipAddress;
}

/**************************************
 ***        PUBLIC FUNCTIONS        ***
 *************************************/
bool InfoContact::isOk()
{
    return (id() >= 0) && (name() != "") && (ipAddress() != "");
}

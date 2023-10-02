#ifndef ISERVERDATA_H
#define ISERVERDATA_H

#include <QDataStream>
#include <QByteArray>
#include <QDebug>

#include "i_basic_data.h"

/*!
 * \brief IServerData class класс обработки принимаемых и отправляемых значений.
 */
class IServerData : public IBasicData
{
public:
    IServerData();

    /*!
     * \brief generateFullMessage метод формирование посылки на агента.
     * \return сформированная к отправке посылка.
     */
    FromPult generateFullMessage();

    /*!
     * \brief parseFullMessage метод распоковки посылки от агента.
     * \param message посылка от агента.
     */
    void parseFullMessage(ToPult message);
};

#endif // ISERVERDATA_H

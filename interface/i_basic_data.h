#ifndef IBASICDATA_H
#define IBASICDATA_H

#include <QObject>
#include <QDebug>

#include "uv_state.h"

/*!
 * \brief IBasicData базовый информационный класс, имеющий глобальную переменную,
 *  которая хранит получаемую и передаваемую информацию о всех состояниях аппарата.
 */
class IBasicData : public QObject
{
    Q_OBJECT
public:
    explicit IBasicData(QObject *parent = nullptr);

    // Static UV_State variable, which we will be accessing
    static UVState agent[2];
    static int currentAgent;
};

#endif // IBASICDATA_H

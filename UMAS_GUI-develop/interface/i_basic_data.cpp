#include "i_basic_data.h"

UVState IBasicData::agent[2];
int IBasicData::currentAgent;


IBasicData::IBasicData(QObject *parent)
    : QObject{parent}
{}

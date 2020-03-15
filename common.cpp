#include "common.h"
#include <QDebug>
#include <QTime>
#include <QCoreApplication>

void QyhSleep(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);

    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
}


unsigned char getCheckSum(QByteArray qba)
{
    unsigned char out=0x00;
    for(int i=0;i<qba.length();++i){
        out ^= (unsigned char)(qba[i]);
    }
    return out;
}

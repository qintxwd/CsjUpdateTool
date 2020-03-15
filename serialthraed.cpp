#include "serialthraed.h"
#include <QSerialPort>
#include <QDebug>

SerialThraed::SerialThraed(QObject *parent) : QThread(parent),
    m_need_open(false),
    m_need_close(false)
{
    start();
}

SerialThraed::~SerialThraed()
{
    m_quit = true;
    wait();
}

void SerialThraed::slot_send(const QByteArray &qba)
{
    const QMutexLocker locker(&m_sendQueueMtx);
    sendQueue.enqueue(qba);
}

void SerialThraed::closeSerial()
{
    const QMutexLocker locker(&m_mtx);
    m_need_close = true;
}

void SerialThraed::startSerial(const QString &portName,int buadrate)
{
    const QMutexLocker locker(&m_mtx);
    m_portName = portName;
    m_buadrate = buadrate;
    m_need_close = false;
    m_need_open = true;
}

void SerialThraed::run()
{
    QSerialPort serial;
    bool temp_need_close = m_need_close;
    QString temp_portName = m_portName;
    int temp_buadrate = m_buadrate;
    bool temp_need_open = m_need_open;
    while (!m_quit) {
        {
            const QMutexLocker locker(&m_mtx);
            temp_need_close = m_need_close;
            temp_portName = m_portName;
            temp_buadrate = m_buadrate;
            temp_need_open = m_need_open;
        }
        if(temp_need_close && serial.isOpen()){
            m_need_close = false;
            //            qDebug()<<"nedd close and go close";
            serial.close();
            emit sig_stateChange(false);
        }
        if(!serial.isOpen() && !temp_need_open){
            QThread::msleep(100);
            continue;
        }
        if(temp_need_open){
            m_need_open = false;
            //            qDebug()<<"need open and go open";
            if(serial.isOpen()){
                serial.close();
                emit sig_stateChange(false);
            }
            serial.setPortName(temp_portName);
            serial.setBaudRate(temp_buadrate);
            serial.setParity(QSerialPort::NoParity);
            serial.setDataBits(QSerialPort::Data8);
            serial.setStopBits(QSerialPort::OneStop);
            if (!serial.open(QIODevice::ReadWrite)) {
                //                qDebug()<<"open fail";
                emit sig_error(tr("Can't open %1, error code %2")
                               .arg(temp_portName).arg(serial.error()));
                continue;
            }
            //            qDebug()<<"open success";
            serial.setPortName(temp_portName);
            serial.setBaudRate(temp_buadrate);
            serial.setParity(QSerialPort::NoParity);
            serial.setDataBits(QSerialPort::Data8);
            serial.setStopBits(QSerialPort::OneStop);
            emit sig_stateChange(true);
        }
        const QMutexLocker locker_(&m_sendQueueMtx);
        while (!sendQueue.isEmpty()){
            auto temp_data = sendQueue.dequeue();
            serial.write(temp_data);
            serial.flush();
            emit sig_alreadySend(temp_data);
            if (serial.waitForReadyRead()) {
                QByteArray responseData = serial.readAll();
                emit sig_read(responseData);
            }
        }
    }
}

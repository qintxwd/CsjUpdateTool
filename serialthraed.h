#ifndef SERIALTHRAED_H
#define SERIALTHRAED_H

#include <QMutex>
#include <QThread>
#include <QQueue>
#include <QWaitCondition>

class SerialThraed : public QThread
{
    Q_OBJECT
public:
    explicit SerialThraed(QObject *parent = nullptr);
    ~SerialThraed();
    void run() override;
    void startSerial(const QString &portName, int buadrate);
    void closeSerial();
signals:
    void sig_error(const QString &s);
    void sig_read(const QByteArray &qba);
    void sig_send(const QByteArray &qba);
    void sig_stateChange(bool isOpen);

    void sig_alreadySend(const QByteArray &qba);
public slots:
    void slot_send(const QByteArray &qba);
private:
    QString m_portName;
    int m_buadrate;
    bool m_quit = false;
    bool m_need_open = false;
    bool m_need_close = false;
    bool m_is_open = false;
    QMutex m_mtx;

    QQueue<QByteArray> sendQueue;
    QMutex m_sendQueueMtx;
};

#endif // SERIALTHRAED_H

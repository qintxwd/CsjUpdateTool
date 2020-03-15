#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>

class QLabel;
class QLineEdit;
class QSpinBox;
class QPushButton;
class QComboBox;
class QProgressBar;
class QTextEdit;
class QLineEdit;

//#include <QSerialPort>
class SerialThraed;

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:
    void sig_send(const QByteArray &qba);
public slots:
    void slot_connectBtnClicked();
    void slot_disconnectBtnClicked();
    void slot_chooseFileBtnClicked();
    void slot_error(QString info);
    void slot_serial_error(const QString &e);
    void slot_read(QByteArray qba);
//    void slot_readyRead(const QByteArray &qba);

    void slot_processPercent(int percent);
    void slot_connectStateChange(bool isOpen);

    void slot_showSend(const QByteArray &qba);

    void slot_startDownload();
    void slot_clear();

    void writeData(QByteArray qba);
private:
    QLabel *m_aboutLabel = nullptr;

    QLabel *m_serialPortLabel = nullptr;
    QComboBox *m_serialPortComboBox = nullptr;

    QLabel *m_serialPortBuadLabel = nullptr;
    QComboBox *m_serialPortBuadComboBox = nullptr;

    QLabel *m_downloadPercentLabel = nullptr;
    QPushButton *m_connectButton = nullptr;
    QPushButton *m_disconnecButton = nullptr;
    QPushButton *m_clearButton = nullptr;
    QPushButton *m_downloadButton = nullptr;

    QLineEdit *m_lineEdit = nullptr;
    QTextEdit *m_textEdit = nullptr;


    QLabel *m_processLabel = nullptr;
    QPushButton *m_chooseFileButton = nullptr;
    QProgressBar *m_processBar = nullptr;

    SerialThraed *m_serial;
};
#endif // MAINWINDOW_H

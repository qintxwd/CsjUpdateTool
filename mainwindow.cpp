#include "mainwindow.h"

#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSerialPortInfo>
#include <QProgressBar>
#include <QSerialPort>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextEdit>
#include <QDebug>
#include <QLineEdit>

#include "serialthraed.h"

#include "common.h"

MainWindow::MainWindow(QWidget *parent)
    : QDialog(parent),
      m_aboutLabel(new QLabel(tr("update tool for songtiandong,develop by qinyinghao."))),
      m_serialPortLabel(new QLabel(tr("serialport:"))),
      m_serialPortComboBox(new QComboBox),
      m_serialPortBuadLabel(new QLabel(tr("buadrate:"))),
      m_serialPortBuadComboBox(new QComboBox),
      m_connectButton(new QPushButton(tr("connect"))),
      m_disconnecButton(new QPushButton(tr("disconnect"))),
      m_processLabel(new QLabel(tr("download process:"))),
      m_processBar(new QProgressBar(this)),
      m_chooseFileButton(new QPushButton(tr("choose file"))),
      m_clearButton(new QPushButton(tr("clear"))),
      m_downloadButton(new QPushButton(tr("start download"))),
      m_textEdit(new QTextEdit(this)),
      m_lineEdit(new QLineEdit(this)),
      m_downloadPercentLabel(new QLabel("  0%")),
      m_serial(new SerialThraed(this))
{
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
        m_serialPortComboBox->addItem(info.portName());

    std::vector<QSerialPort::BaudRate> all_buadrates = {QSerialPort::Baud1200,
                                                        QSerialPort::Baud2400,
                                                        QSerialPort::Baud4800,
                                                        QSerialPort::Baud9600,
                                                        QSerialPort::Baud19200,
                                                        QSerialPort::Baud38400,
                                                        QSerialPort::Baud57600,
                                                        QSerialPort::Baud115200};
    for (auto e : all_buadrates) {
        m_serialPortBuadComboBox->addItem(QString("%1").arg((int)e));
    }
    m_processBar->setRange(0,100);

    auto mainLayout = new QGridLayout;
    mainLayout->addWidget(m_aboutLabel, 0, 0,1,8);
    mainLayout->addWidget(m_serialPortLabel, 1, 0);
    mainLayout->addWidget(m_serialPortComboBox, 1, 1);
    mainLayout->addItem(new QSpacerItem(20,10),1,2);
    mainLayout->addWidget(m_serialPortBuadLabel, 1, 3);
    mainLayout->addWidget(m_serialPortBuadComboBox, 1, 4);
    mainLayout->addItem(new QSpacerItem(20,10),1,5);
    mainLayout->addWidget(m_connectButton, 1, 6);
    mainLayout->addWidget(m_disconnecButton, 1, 7);

    mainLayout->addWidget(m_chooseFileButton, 2, 0, 1, 2);
    mainLayout->addWidget(m_lineEdit, 2, 2, 1, 4);
    mainLayout->addWidget(m_downloadButton, 2, 6, 1, 2);

    mainLayout->addWidget(m_processLabel, 3, 0, 1, 2);
    mainLayout->addWidget(m_processBar, 3, 2, 1, 5);
    mainLayout->addWidget(m_downloadPercentLabel, 3, 7, 1, 2);

    mainLayout->addWidget(m_clearButton, 4, 0, 1, 2);

    mainLayout->addWidget(m_textEdit, 5, 0, 6, 8);

    setLayout(mainLayout);

    m_disconnecButton->setEnabled(false);
    m_chooseFileButton->setEnabled(false);
    m_downloadButton->setEnabled(false);

    setWindowTitle(tr("csjbot update tool"));
    m_serialPortComboBox->setFocus();
    m_lineEdit->setEnabled(false);

    connect(m_connectButton, &QPushButton::clicked, this, &MainWindow::slot_connectBtnClicked);
    connect(m_disconnecButton, &QPushButton::clicked, this, &MainWindow::slot_disconnectBtnClicked);
    connect(m_chooseFileButton, &QPushButton::clicked, this, &MainWindow::slot_chooseFileBtnClicked);

    connect(m_downloadButton, &QPushButton::clicked, this, &MainWindow::slot_startDownload);
    connect(m_clearButton, &QPushButton::clicked, this, &MainWindow::slot_clear);

    connect(m_serial, &SerialThraed::sig_read, this, &MainWindow::slot_read);
    connect(m_serial, &SerialThraed::sig_stateChange, this, &MainWindow::slot_connectStateChange);
    connect(m_serial, &SerialThraed::sig_error, this, &MainWindow::slot_serial_error);
    connect(m_serial, &SerialThraed::sig_alreadySend, this, &MainWindow::slot_showSend);
    connect(this, &MainWindow::sig_send,m_serial, &SerialThraed::slot_send);

}

MainWindow::~MainWindow()
{
}

void MainWindow::slot_connectBtnClicked()
{
    if(m_serialPortComboBox->currentText().isEmpty()){
        slot_error("please select a serial port first!");
        return ;
    }
    QString portname = m_serialPortComboBox->currentText();
    int buadrate = m_serialPortBuadComboBox->currentText().toInt();
    m_serial->startSerial(portname,buadrate);
}

void MainWindow::slot_disconnectBtnClicked()
{
    m_serial->closeSerial();
}

void MainWindow::slot_chooseFileBtnClicked()
{
    auto fileName = QFileDialog::getOpenFileName(this,
                                                 tr("open hex file"), "", tr("hexfiles(*.bin *.hex)"));

    m_lineEdit->setText(fileName);
}

void MainWindow::slot_error(QString info)
{
    QMessageBox::warning(this, tr("error"),info);
}


void MainWindow::slot_serial_error(const QString &e)
{
    qDebug()<<"error "<<e;
}

void MainWindow::slot_read(QByteArray qba)
{
    m_textEdit->append("read:"+qba.toHex(' '));
}

void MainWindow::slot_showSend(const QByteArray &qba)
{
    m_textEdit->append("send:"+qba.toHex(' '));
}

void MainWindow::slot_processPercent(int percent)
{
    m_processBar->setValue(percent);
    QString str_per = QString("%1").arg(percent);
    if(str_per.length()==1){
        str_per = "  "+str_per+"%";
    }
    else if(str_per.length()==2){
        str_per = " "+str_per+"%";
    }
    else if(str_per.length()==3){
        str_per+="%";
    }
    m_downloadPercentLabel->setText(str_per);
}

void MainWindow::slot_connectStateChange(bool isOpen)
{
    if(isOpen){
        m_connectButton->setEnabled(false);
        m_disconnecButton->setEnabled(true);
        m_chooseFileButton->setEnabled(true);
        m_downloadButton->setEnabled(true);
    }else{
        m_connectButton->setEnabled(true);
        m_disconnecButton->setEnabled(false);
        m_chooseFileButton->setEnabled(false);
        m_downloadButton->setEnabled(false);
    }
}
void MainWindow::writeData(QByteArray qba)
{
    unsigned char LENGTH = 0x00;

    unsigned char head_0_unsigned[] = {0x10,0x02,0x55,0x4b};
    unsigned char head_1_unsigned[] = {0x10,0x02,0x55,0x4b};
    unsigned char tail_unsigned[] = {0x10,0x02,0x55,0x5a};
    unsigned char short_unsigned[] = {0x10,LENGTH,0x55,0x53, };
    unsigned char long_unsigned[] = {0x50,LENGTH,LENGTH,0x55,0x53, };

    QByteArray head_data_0((char *)head_0_unsigned,sizeof(head_0_unsigned));
    head_data_0.append(getCheckSum(head_data_0));

    QByteArray head_data_1((char *)head_1_unsigned,sizeof (head_1_unsigned));
    head_data_1.append(getCheckSum(head_data_1));

    QByteArray tail_data((char *)tail_unsigned,sizeof (tail_unsigned));
    tail_data.append(getCheckSum(tail_data));

    QByteArray short_data((char *)short_unsigned,sizeof (short_unsigned));
    QByteArray long_data((char *)long_unsigned,sizeof (long_unsigned));

    emit sig_send(head_data_0);
    slot_processPercent(1);
    QyhSleep(500);

    emit sig_send(head_data_1);
    slot_processPercent(2);
    QyhSleep(500);

    int send_index = 0;
    while(true){
        if(send_index>=qba.length()){
            break;
        }
        if(qba.length()-send_index>=2000){
            int length = 2000;
            QByteArray send_data = long_data + (qba.mid(send_index,length));
            send_data[2] = (((length+2)>>8)&0xff);
            send_data[1] = ((length+2)&0xff);
            send_data.append(getCheckSum(send_data));
            send_index+=length;
            emit sig_send(send_data);
        }else if(qba.length()-send_index>255){
            int length = qba.length()-send_index;
            QByteArray send_data = long_data + (qba.mid(send_index,length));
            send_data[2] = (((length+2)>>8)&0xff);
            send_data[1] = ((length+2)&0xff);
            send_data.append(getCheckSum(send_data));
            send_index+=length;
            emit sig_send(send_data);
        }else{
            int length = qba.length()-send_index;
            QByteArray send_data = short_data+(qba.mid(send_index,length));
            send_data[1] = ((length+2)&0xff);
            send_data.append(getCheckSum(send_data));
            send_index+=length;
            emit sig_send(send_data);
        }
        int p = (int)(97*(send_index*1.0/qba.length()));
        slot_processPercent(2+ p);
        QyhSleep(500);
    }

    emit sig_send(tail_data);
    slot_processPercent(100);

}

void MainWindow::slot_startDownload()
{
    auto fileName = m_lineEdit->text();
    if(fileName.isEmpty()){
        slot_error("please choose a file");
        return ;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)){
        slot_error("cannot open file "+fileName);
        return;
    }

    QByteArray qba = file.readAll();
    writeData(qba);
}

void MainWindow::slot_clear()
{
    m_textEdit->clear();
}

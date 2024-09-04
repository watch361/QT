#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void SerialPortInit();
    void Dashboard_Init();
    QSerialPort *serial; // 定义全局的串口对象（第三步）

private slots:
    void on_BtnOpenSerial_clicked();

    void on_AFG_start_clicked();

    void on_btnSend_clicked();

    void on_chkTimSend_stateChanged(int arg1);

    void timeSend_Init();

    void on_BtnDataSave_clicked();

    void on_BtnOpenSerial_2_clicked();

private:
    Ui::MainWindow *ui;
    QCustomPlot *customPlot;
    void SerialMonitor(const QByteArray& data, bool isSent);
    QTimer *timSend;  //数据发送定时器
    void UartReceiveMessage();
    void QCustomPlot_Init();
    QVector<double> arrX;
    QVector<double> arrY;
};
#endif // MAINWINDOW_H

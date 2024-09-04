#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QMessageBox>


#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    SerialPortInit();
    Dashboard_Init();

    timeSend_Init();

     QCustomPlot_Init();


}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::SerialPortInit()
{
    QStringList portNameList;

    serial = new QSerialPort; //申请内存,并设置父对象
    // 参数配置
    ui->BaudBox->addItem("9600");
    ui->BaudBox->addItem("57600");
    ui->BaudBox->addItem("115200");
    ui->BaudBox->addItem("230400");
    ui->BaudBox->addItem("460800");

    // 获取用户选择的波特率字符串
    QString selectedBaudRate = ui->BaudBox->currentText();

    // 检查用户选择的波特率，并根据不同的波特率设置串口的波特率
    // 检查用户选择的波特率，并根据不同的波特率设置串口的波特率
    if (selectedBaudRate == "9600")
    {
        serial->setBaudRate(QSerialPort::Baud9600);
    }
    if (selectedBaudRate == "57600")
    {
        serial->setBaudRate(QSerialPort::Baud57600);
    }
    if (selectedBaudRate == "115200")
    {
        serial->setBaudRate(QSerialPort::Baud115200);
    }
    // if (selectedBaudRate == "230400")
    // {
    //     serial->setBaudRate(QSerialPort::Baud230400);
    // }
    // if (selectedBaudRate == "460800")
    // {
    //     serial->setBaudRate(QSerialPort::Baud460800);
    // }
    // 校验，校验默认选择无
    ui->ParityBox->addItem("无");
    serial->setParity(QSerialPort::NoParity);

    // 数据位，数据位默认选择8位
    ui->BitBox->addItem("8");
    serial->setDataBits(QSerialPort::Data8);

    // 停止位，停止位默认选择1位
    ui->StopBox->addItem("1");
    serial->setStopBits(QSerialPort::OneStop);


    // 刷新串口
    //  RefreshSerialPort(0);
    ui->PortBox->clear();                                        //关闭串口号
    ui->PortBox->addItem("刷新");                                //添加刷新
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts()) //添加新串口
    {
        portNameList.append(info.portName());
    }

    ui->PortBox->addItems(portNameList);
    ui->PortBox->setCurrentIndex(1);                             // 当前串口号为COM1
    serial->setPortName(ui->PortBox->currentText());             //设置串口号

      connect(serial,&QSerialPort::readyRead,this,&MainWindow::UartReceiveMessage);


}
void MainWindow::QCustomPlot_Init()
{
    //添加一条曲线
    //向绘图区域QCustomPlot(从widget提升来的)添加一条曲线
    ui->myCustomPlot->addGraph();
    //设置画笔
    ui->myCustomPlot->graph(0)->setPen(QPen(Qt::blue));
    //设置画刷,曲线和X轴围成面积的颜色
    ui->myCustomPlot->graph(0)->setBrush(QBrush(QColor(255,255,0)));
    //设置右上角图形标注名称
    ui->myCustomPlot->graph(0)->setName("曲线");
    //设置坐标轴标签名称
    ui->myCustomPlot->xAxis->setLabel("x");
    ui->myCustomPlot->yAxis->setLabel("y");

    //设置坐标轴显示范围,否则我们只能看到默认的范围
    ui->myCustomPlot->xAxis->setRange(0,60);
    ui->myCustomPlot->yAxis->setRange(0,1000);


}

void MainWindow::UartReceiveMessage()
{

    uint16_t strReceiveData;
    // 解析帧数据
    QByteArray data_receive = serial->readAll();          // 读取数据

    SerialMonitor(data_receive,false);


    if (!data_receive.isEmpty())
    {
      uint8_t firstByte = static_cast<uint8_t>(data_receive.at(0));
        strReceiveData = static_cast<uint16_t>(firstByte);
    }



    arrX.append(arrX.count());
    arrY.append(strReceiveData);

    //传入数据，setData的两个参数类型为double
    ui->myCustomPlot->graph(0)->setData(arrX,arrY);
    //曲线重绘
    ui->myCustomPlot->replot();


}
void MainWindow::on_BtnOpenSerial_clicked()
{
    if(serial->isOpen())                                        // 如果串口打开了，先给他关闭
    {
        serial->clear();
        serial->close();
        // 关闭状态，按钮显示“打开串口”
        ui->BtnOpenSerial->setText("打开串口");
        // 关闭状态，允许用户操作
        ui->BaudBox->setDisabled(false);
        ui->ParityBox->setDisabled(false);
        ui->BitBox->setDisabled(false);
        ui->StopBox->setDisabled(false);
        // 禁止操作“发送字符操作”
        // 关闭状态，颜色为绿色
        ui->BtnOpenSerial->setStyleSheet("color: green;");
        // 关闭，显示灯为红色
        // 清空数据
    }
    else                                                        // 如果串口关闭了，先给他打开
    {
        //当前选择的串口名字
        serial->setPortName(ui->PortBox->currentText());
        //用ReadWrite 的模式尝试打开串口，无法收发数据时，发出警告
        if(!serial->open(QIODevice::ReadWrite))
        {
            QMessageBox::warning(this,tr("提示"),tr("串口打开失败!"),QMessageBox::Ok);
            return;
        }
        // 打开状态，按钮显示“关闭串口”
        ui->BtnOpenSerial->setText("关闭串口");
        // 打开状态，禁止用户操作
        ui->BaudBox->setDisabled(true);
        ui->ParityBox->setDisabled(true);
        ui->BitBox->setDisabled(true);
        ui->StopBox->setDisabled(true);
        // 允许操作“发送字符操作”
        // 打开状态，颜色为红色
        ui->BtnOpenSerial->setStyleSheet("color: red;");
        // 打开，显示灯为绿色
    }
}


void MainWindow::on_AFG_start_clicked()
{
    uint AFG_test=0;

    int AFG_position_Command;
    int AFG_speed_Command;
    int AFG_torque_Command;
    char AFG_flag=0;
    uint AFG_duration;
    double  AFG_Gain;
    double displacement_increment;
    double AFG_INC;
    int  AFG_value;
    int  AFG_value_last;
    uint AFG_amplitude;
    uint AFG_frequency;
    uint AFG_number;
    bool AFG_FB_flag=0;
    uint AFG_count=0;
    uint AFG_ct;

    float AFG_T;

    // uint AFG_amplitude;

    AFG_amplitude=ui->AFG_a->value();
    AFG_frequency=ui->AFG_f->value();
    AFG_number=   ui->AFG_num->value();
    AFG_duration= ui->AFG_t->value();
    AFG_ct=ui->AFG_ct->value();
    AFG_INC=0;
    double x1 = AFG_amplitude;

    QCustomPlot *customPlot = ui->AFG_PLOT;//创建customPlot
    customPlot->clearGraphs();
    customPlot->legend->setVisible(false);//将图例设置为不可见

    //每条曲线都会独占一个graph()
    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(Qt::blue)); // 曲线的颜色
    customPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // 曲线与X轴包围区的颜色

    //      customPlot->addGraph();//添加graph等价于添加一条新曲线
    //      customPlot->graph(1)->setPen(QPen(Qt::red)); // 曲线的颜色
    //      customPlot->addGraph();//添加graph等价于添加一条新曲线
    //      customPlot->graph(2)->setPen(QPen(Qt::yellow)); // 曲线的颜色
    //      customPlot->addGraph();//添加graph等价于添加一条新曲线
    //      customPlot->graph(3)->setPen(QPen(Qt::green)); // 曲线的颜色
    // 生成模拟数据点 (x-y0 第一条曲线, x-y1为第2条曲线):
    QVector<double> x(AFG_number*101), y0(AFG_number*101), y1(AFG_number*101),y2(AFG_number*101), y3(AFG_number*101);



    if(ui->comboBox_AFG->currentText()=="1.斜坡")//上位机模式判断
    {
        AFG_flag=1;
    }
    else if(ui->comboBox_AFG->currentText()=="2.方波")//上位机模式判断
    {
        AFG_flag=2;
    }
    else if(ui->comboBox_AFG->currentText()=="3.正弦波")//上位机模式判断
    {
        AFG_flag=3;
    }
    else if(ui->comboBox_AFG->currentText()=="4.阶跃")//上位机模式判断
    {
        AFG_flag=4;
    }
    else
    {
        AFG_flag=0;
    }
    /////////////////////////////////////////////////////
    if(AFG_flag==1)
    {
        customPlot->graph(0)->setName("斜坡");
    }
    else if(AFG_flag==2)
    {
        customPlot->graph(0)->setName("方波");
    }
    else if(AFG_flag==3)
    {
        customPlot->graph(0)->setName("正弦波");
    }
    else if(AFG_flag==4)
    {
        customPlot->graph(0)->setName("阶跃");
    }
    customPlot->legend->setVisible(true);
    displacement_increment=(double)AFG_amplitude/AFG_duration;

    AFG_T=1000/AFG_frequency;

    for (int i=0; i<(AFG_duration); i++)
    {
        x[i] = i;

        if(AFG_flag==1)
        {
            AFG_INC+=displacement_increment;
            if(AFG_INC>AFG_amplitude)
            {
                y0[i] = AFG_amplitude; // 第一条曲线：斜坡
            }
            else
            {

                y0[i] = AFG_INC; // 第一条曲线：斜坡
            }
        }
        if(AFG_flag==2)
        {


            if(AFG_count==AFG_T)
            {
                AFG_count=0;
                AFG_FB_flag = !AFG_FB_flag; //方波
            }
            AFG_count++;
            if(AFG_FB_flag==true)
            {
                y1[i] = AFG_amplitude;
            }
            if(AFG_FB_flag==false)
            {
                y1[i] = 0;
            }

        }
        if(AFG_flag==3)
        {
            y2[i]= AFG_amplitude/2*qSin(i*2*3.1415*AFG_frequency)+AFG_amplitude/2;
        }

        if(AFG_flag==4)
        {
            if(i>=AFG_ct)
            {
                y3[i]= AFG_amplitude;

            }
        }

    }
    // 边框右侧和上侧均显示刻度线，但不显示刻度值:
    customPlot->xAxis2->setVisible(true);
    customPlot->xAxis2->setTickLabels(false);
    customPlot->yAxis2->setVisible(true);
    customPlot->yAxis2->setTickLabels(false);
    // 使上下两个X轴的范围总是相等，使左右两个Y轴的范围总是相等
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
    // 把已存在的数据填充进graph的数据区
    if(AFG_flag==1)
    {
        customPlot->graph(0)->setData(x, y0);
    }
    if(AFG_flag==2)
    {
        customPlot->graph(0)->setData(x, y1);
    }
    if(AFG_flag==3)
    {
        customPlot->graph(0)->setData(x, y2);
    }
    if(AFG_flag==4)
    {

        customPlot->graph(0)->setData(x, y3);
    }


    customPlot->xAxis->setRange(0,AFG_duration+100);
    customPlot->yAxis->setRange(0,AFG_amplitude+100);


    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    // 立即刷新图像


    QCPItemTracer *tracer;




    tracer = new QCPItemTracer(customPlot);
    //tracer->setInterpolating(false);
    tracer->setStyle(QCPItemTracer::tsCircle);
    tracer->setPen(QPen(Qt::red));
    tracer->setBrush(Qt::red);
    tracer->setSize(6);

    //tracer->setInterpolating(true);
    tracer->setGraph(customPlot->graph(0)); //将游标和该曲线图层想连接



    // 让时间输入框不可选，避免误操作（输入功能不可用，控件背景为灰色）

    //AFG_test=AFG_test++;
    if(AFG_flag==1)//斜波
    {
        customPlot->graph(0)->setData(x, y0);
    }
    if(AFG_flag==2)//方波
    {
        customPlot->graph(0)->setData(x, y1);
    }


    if(AFG_flag==3)//正弦波
    {

        customPlot->graph(0)->setData(x, y2);
    }
    if(AFG_flag==4)//阶跃
    {

        customPlot->graph(0)->setData(x, y3);
    }




    // TIM_AFG=new QTimer;
    // TIM_AFG->setInterval(100);//设置默认值1ms 100 100ms  1000  1s
    // connect(TIM_AFG, &QTimer::timeout, this, [=](){
    //     on_AFG_QCustomPlot();
    // });


    ui->AFG_PLOT->replot();



}


void MainWindow::Dashboard_Init()
{
    //仪表盘显示
   // connect(this->ui->horizontalSlider,&QSlider::valueChanged,this,&MainWindow::horizontalSlider_3);
    this->ui->mydial->settitle("速度");
    this->ui->mydial->setunits("r/min");
    this->ui->mydial->setmaxValue(4000);
    this->ui->mydial->setminValue(-4000);
    //this->ui->dialPlate->setValue(value);
    this->ui->mydial->update();//手动调用控件的OnPaint方法
   // connect(this->ui->horizontalSlider_2,&QSlider::valueChanged,this,&MainWindow::horizontalSlider_4);
    this->ui->mydial1->settitle("角度");
    this->ui->mydial1->setunits("°");
    this->ui->mydial1->setmaxValue(360);
    this->ui->mydial1->setminValue(0);
    //this->ui->dialPlate->setValue(value);
    this->ui->mydial1->update();//手动调用控件的OnPaint方法



}

void MainWindow::SerialMonitor(const QByteArray& data, bool isSent)
{

    QString currentTime = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    QString hexString;
    // 将 QByteArray 转换为十六进制字符串
    if(isSent)//发送数据
    {
       if(ui->chkSend->checkState() == false)
      {
          hexString = QString::fromUtf8(data);
       }
       else
       {
            hexString = data.toHex(' ').toUpper(); // 使用空格分隔，大写形式显示


       }
    }
    else  //接收数据
    {
        if(ui->chkRec->checkState() == false)
        {
            hexString = QString::fromUtf8(data);
        }
        else
        {
            hexString = data.toHex(' ').toUpper(); // 使用空格分隔，大写形式显示


        }

    }
    if (isSent) {

        ui->DataReceived_monitor->insertPlainText("[" + currentTime + "] 发送：" + hexString + "\n");
        //QString str = QString::fromUtf8(data);
        //ui->DataReceived_monitor->insertPlainText("[" + currentTime + "] 发送：" + str + "\n");
        // 处理发送数据
    } else {
        ui->DataReceived_monitor->insertPlainText("[" + currentTime + "] 接收：" + hexString + "\n");
        // 处理接收数据
    }
    // 移动光标到下一行的开头
    ui->DataReceived_monitor->moveCursor(QTextCursor::End);
    ui->DataReceived_monitor->insertPlainText("\n");

    // 滚动到底部
    QScrollBar* scrollbar = ui->DataReceived_monitor->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());

    //const qint64 MAX_SIZE_BYTES = 2 * 1024 * 1024; // 2MB
     const qint64 MAX_SIZE_BYTES =   1024 *1024; // 1MB自动存储
    if (ui->DataReceived_monitor->toPlainText().size() > MAX_SIZE_BYTES) // 检查文本长度
    {
        on_BtnDataSave_clicked();
         ui->DataReceived_monitor->clear();
    }
}

void MainWindow::on_btnSend_clicked()
{
    QByteArray sendData;
    // 判断是否为16进制发送，将发送区全部的asc2转换为16进制字符串显示，发送的时候转换为16进制发送
    if(ui->chkSend->checkState() == false){
        // 字符串形式发送，GB2312编码用以兼容大多数单片机
        // sendData = ui->txtSend->toPlainText().toLocal8Bit();// GB2312编码输出
        sendData = ui->txtSend->toPlainText().toUtf8();// Unicode编码输出
        //sendData = ui->txtSend->toPlainText().toLatin1();
    }else{
        // 16进制发送，不要用.data()，.data()返回的是字符数组，0x00在ASC2中的意义为NUL，也就是'\0'结束符，所以遇到0x00就会终止
        sendData = QByteArray::fromHex(ui->txtSend->toPlainText().toUtf8());// Unicode编码输出 ///GB2312不能识别00
        // sendData = QByteArray::fromHex(ui->txtSend->toPlainText().toLocal8Bit());// GB2312编码输出
    }

    // 如发送成功，会返回发送的字节长度。失败，返回-1。

    int a = serial->write(sendData);
     SerialMonitor(sendData,true);
}

void MainWindow::timeSend_Init()
{
    timSend = new QTimer;
    timSend->setInterval(1000);// 设置默认定时时长1000ms
    connect(timSend, &QTimer::timeout, this, [=](){
        on_btnSend_clicked();
    });


}
void MainWindow::on_chkTimSend_stateChanged(int arg1)
{
    if(arg1 == 0){
        timSend->stop();
        // 时间输入框恢复可选
        ui->txtSendMs->setEnabled(true);
    }else{
        // 对输入的值做限幅，小于10ms会弹出对话框提示
        if(ui->txtSendMs->text().toInt() >= 10){
            timSend->start(ui->txtSendMs->text().toInt());// 设置定时时长，重新计数
            // 让时间输入框不可选，避免误操作（输入功能不可用，控件背景为灰色）
            ui->txtSendMs->setEnabled(false);
        }else{
            ui->chkTimSend->setCheckState(Qt::Unchecked);
            QMessageBox::critical(this, "错误提示", "定时发送的最小间隔为 10ms\r\n请确保输入的值 >=10");
        }
    }
}


void MainWindow::on_BtnDataSave_clicked()
{
    QString fileName = "D:/output.csv";
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Failed to open file for writing.");
        qDebug() << "File open failed.";
        return;
    }

    QTextStream out(&file);

    // Get the content of QTextEdit
    QString text = ui->DataReceived_monitor->toPlainText();

    // Convert text to CSV format
    QStringList rows = text.split('\n');
    for (const QString &row : rows) {
        QStringList fields = row.split('\t'); // Assuming tab-separated input
        out << fields.join(',') << "\n"; // Convert tab-separated fields to CSV format
    }

    file.close();
}


void MainWindow::on_BtnOpenSerial_2_clicked()
{
    //生成模拟数据
    arrX.append(arrX.count());
    arrY.append(rand()%4096);

    //传入数据，setData的两个参数类型为double
    ui->myCustomPlot->graph(0)->setData(arrX,arrY);
    //曲线重绘
    ui->myCustomPlot->replot();
}


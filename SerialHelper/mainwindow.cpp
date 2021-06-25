#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextCodec>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    chart(new QChart),
    isStopping(false)
{
    ui->setupUi(this);
    ui->radioButtonsendformat->setEnabled(false);
    ui->comboBoxsendformat->setEnabled(false);
//    ui->radioButtonreadauto->setEnabled(false);
//    ui->radioButtonreadformat->setEnabled(false);
    ui->comboBoxreadformat->setEnabled(false);
    ui->radioButtonsendauto->setEnabled(false);
    ui->spinBox->setProperty("noinput",true);
    ui->textBrowser->setProperty("noinput",true);
    QString strtext="    1,虚线为CWM,实线为MI. 2,红黑蓝依次为 Roll Pitch Yaw";
    ui->label_8->setText(strtext);
    // 设置串口状态
    b=false;
    port=new QSerialPort;
    connect(port,SIGNAL(readyRead()),this,SLOT(readread()));
    // 刷新串口
    MainWindow::on_pushButtonrefresh_clicked();
    // 可编码模式
    QList<QByteArray>list=QTextCodec::availableCodecs();
    for(int i=0;i<list.count();++i)
    {
        ui->comboBoxsendformat->addItem(list.at(i));
        ui->comboBoxreadformat->addItem(list.at(i));
    }
    ui->labelstatu->setStyleSheet("border-image: url(:/new/img/red.png);");
    // 绘制相关初始化
    series_cwm_roll = new QLineSeries;
    series_cwm_pitch = new QLineSeries;
    series_cwm_yaw = new QLineSeries;
    // 设置cwm为虚线
    series_cwm_roll->setPen(QPen(QBrush(Qt::red), 1, Qt::DashLine));
    series_cwm_pitch->setPen(QPen(QBrush(Qt::black), 1, Qt::DashLine));
    series_cwm_yaw->setPen(QPen(QBrush(Qt::blue), 1, Qt::DashLine));

    series_mi_roll = new QLineSeries;
    series_mi_pitch = new QLineSeries;
    series_mi_yaw = new QLineSeries;
    // 设置mi为实线
    series_mi_roll->setPen(QPen(QBrush(Qt::red), 1, Qt::SolidLine));
    series_mi_pitch->setPen(QPen(QBrush(Qt::black), 1, Qt::SolidLine));
    series_mi_yaw->setPen(QPen(QBrush(Qt::blue), 1, Qt::SolidLine));

    chart->addSeries(series_cwm_roll);
    chart->addSeries(series_cwm_pitch);
    chart->addSeries(series_cwm_yaw);
    chart->addSeries(series_mi_roll);
    chart->addSeries(series_mi_pitch);
    chart->addSeries(series_mi_yaw);

    chart->createDefaultAxes();
    chart->axisY()->setRange(-180, 180);
    chart->axisX()->setRange(0, 100);

    chart->axisX()->setTitleFont(QFont("Microsoft YaHei", 10, QFont::Normal, true));
    chart->axisY()->setTitleFont(QFont("Microsoft YaHei", 10, QFont::Normal, true));
    chart->axisX()->setTitleText("时间");
    chart->axisY()->setTitleText("dgree");

    chart->axisX()->setGridLineVisible(false);
    chart->axisY()->setGridLineVisible(false);
    chart->legend()->hide();

    chartView = new ChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    ui->drawLayout->addWidget(chartView);

}
MainWindow::~MainWindow()
{
    delete ui;
}
// 显示串口接收的字符串
void MainWindow::readread()
{
    //QByteArray arr= port->readLine();
    QByteArray arr= port->readAll();
    // 处理数据------------------------------------------------------
#define PROCESS
#ifdef PROCESS
    QString inputOriginal = arr;
    QString yaw_temp;
    QString pitch_temp;
    QString roll_temp;
    int i = inputOriginal.indexOf(QString(("[cyweeTrace]")));
    if(i >= 0)
    {
        QString input = inputOriginal.mid(i);
        int index1 =   input.indexOf(QString(("yaw=")));
        int index2 =   input.indexOf(QString((", pitch=")));
        int index3 =   input.indexOf(QString((", row=")));
        int index4 =   input.indexOf(QString(("\n")));
        yaw_temp = input.mid(index1+4, index2-index1-4);
        pitch_temp = input.mid(index2+8, index3-index2-8);
        roll_temp = input.mid(index3+6, index4-index3-6);
        ui->lineedit_cywee_yaw->setText(yaw_temp);
        ui->lineedit_cywee_pitch->setText(pitch_temp);
        ui->lineedit_cywee_roll->setText(roll_temp);
        //需要单独处理以下cwm的yaw
        qreal yaw = yaw_temp.toDouble()/1000;
        if(yaw >= 180)
            yaw -= 360;
        updateData(series_cwm_yaw, -yaw);
        updateData(series_cwm_pitch, -pitch_temp.toDouble()/1000);
        updateData(series_cwm_roll, -roll_temp.toDouble()/1000);
    }
    i = inputOriginal.indexOf(QString(("[HeadTrace]:: realstamp")));
    if(i >= 0)
    {
        QString input = inputOriginal.mid(i);
        int index1 =   input.indexOf(QString(("yaw:")));
        int index2 =   input.indexOf(QString((" pitch:")));
        int index3 =   input.indexOf(QString((" roll:")));
        int index4 =   input.indexOf(QString(("\n")));
        yaw_temp = input.mid(index1+4, index2-index1-4);
        pitch_temp = input.mid(index2+7, index3-index2-7);
        roll_temp = input.mid(index3+6, index4-index3-6);
        ui->lineedit_mi_yaw->setText(yaw_temp);
        ui->lineedit_mi_pitch->setText(pitch_temp);
        ui->lineedit_mi_roll->setText(roll_temp);
        updateData(series_mi_yaw, yaw_temp.toDouble()/1000);
        updateData(series_mi_pitch, pitch_temp.toDouble()/1000);
        updateData(series_mi_roll, roll_temp.toDouble()/1000);
    }
#endif
    // 处理数据结束--------------------------------------------------

    // 如果以文本显示，直接调用showformat函数
    if(ui->checkBoxtextformat->isChecked())
    {
        showformat(arr);
    }
    else
    {
        QString str;
        for(int i=0;i<arr.length();i++)
        {
            str+=QString("%1").arg((uchar)arr.at(i),2,16,QLatin1Char('0')).toUpper()+" ";
        }
        ui->textBrowser->append(str);
    }
}
void MainWindow::showformat(const QByteArray &arr)
{
    if(ui->radioButtonsendauto->isChecked())
    {
        QTextCodec *cod= QTextCodec::codecForName(ui->comboBoxsendformat->currentText().toLatin1());
        QString str=cod->toUnicode(arr);
        ui->textBrowser->append(str);
    }
    else
    {
        QString str(arr);
        ui->textBrowser->append(str);
    }
}
void MainWindow::on_pushButtonsend_clicked()
{
    if(ui->checkBoxsendautoclear->isChecked())
    {
        ui->lineEdit->clear();
    }
    if(!port->isOpen())
    {
        return;
    }
    if(ui->checkBoxsendformat->isChecked())
    {
        sendformat(ui->lineEdit->text());
    }
    else
    {
        QString str=ui->lineEdit->text();
        QStringList strlist=str.trimmed().split(" ");
        QByteArray arr;
        for(int i=0;i<strlist.count();++i)
        {
            bool bl=false;
            QString ch=strlist.at(i);
            uchar byte=(uchar)ch.toInt(&bl,16);
            if(!bl)
            {
                return;
            }
            arr.append(byte);
        }
        port->write(arr);
    }

}
void MainWindow::sendformat(const QString &strdata)
{
    if(ui->radioButtonreadauto->isChecked())
    {
        QTextCodec *cod= QTextCodec::codecForName(ui->comboBoxreadformat->currentText().toLatin1());
        QByteArray arr=cod->fromUnicode(strdata);
        port->write(arr);
    }
    else
    {
        QString str=ui->lineEdit->text();
        QByteArray arr(str.toLatin1());
        port->write(arr);
    }
}
void MainWindow::on_pushButtonopen_clicked()
{
    if(b)
    {
        port->close();
        ui->pushButtonopen->setText("打开");
        ui->labelstatu->setStyleSheet("border-image: url(:/new/img/red.png);");
        b=false;
        ui->labelstate->setText("串口已关闭！");
        ui->pushButtonrefresh->setEnabled(true);
    }
    else
    {
#ifdef Q_OS_WIN
        port->setPortName(ui->comboBoxserial->currentText());
#else
        port->setPortName("/dev/"+ui->comboBoxserial->currentText());
#endif
        //port->setBaudRate(ui->comboBoxbute->currentText().toInt());
        port->setBaudRate(ui->lineBaudRate->text().toInt());
        port->setDataBits(getbit(ui->spinBox->value()));
        if(port->open(QIODevice::ReadWrite))
        {
            ui->pushButtonopen->setText("关闭");
            ui->labelstatu->setStyleSheet("border-image: url(:/new/img/lv.png);");
            b=true;
            ui->labelstate->setText("串口已打开！");
            ui->pushButtonrefresh->setEnabled(false);
        }
        else
        {
            b=false;
            ui->labelstate->setText("打开失败！");
            ui->labelstatu->setStyleSheet("border-image: url(:/new/img/red.png);");
        }
    }
}
void MainWindow::on_checkBoxtextformat_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    if(ui->checkBoxtextformat->isChecked())
    {
        ui->radioButtonreadformat->setEnabled(true);
        ui->radioButtonreadauto->setEnabled(true);
    }
    else
    {
        ui->radioButtonreadformat->setEnabled(false);
        ui->radioButtonreadauto->setEnabled(false);
    }
}
void MainWindow::on_checkBoxsendformat_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    if(ui->checkBoxsendformat->isChecked())
    {
        ui->radioButtonsendformat->setEnabled(true);
        ui->radioButtonsendauto->setEnabled(true);
    }
    else
    {
        ui->radioButtonsendformat->setEnabled(false);
        ui->radioButtonsendauto->setEnabled(false);
    }
}
void MainWindow::on_pushButtonclear_clicked()
{
    ui->textBrowser->clear();
}
void MainWindow::on_pushButtoncleasend_clicked()
{
    ui->lineEdit->clear();
}
void MainWindow::on_pushButtonhelp_clicked()
{
    QString strtext="帮助信息：\n"
                    "1.默认情况下接收和发送均是16进制，用英文空格' '将数据隔开发送数据格式：\n"
                    "01 02 03 04 05 06 07 08 09 0A 0B 0C 0E 0D 0F 10 11 12\n"
                    "或者：\n 1 2 3 4 5 6 7 8 9 A B C D E F 10 11 12\n"
                    "同样，收到的数据会按照16进制以空格分隔的方式显示;\n"
                    "2.当选择文本格式发送或着接收时，可以选择编码后发送或显示，具体编码格式根据下拉框选择;\n"
                    "3.当发送汉字的时候需要选择UTF-8编码或者GBK编码,默认采用系统默认;"
                    "4.嵌入式环境下，点击输入框会有软件盘用于输入，关闭软件盘需要点击空白处;";
    ui->textBrowser->append(strtext);
}
void MainWindow::on_radioButtonsendauto_clicked()
{
    if(ui->radioButtonsendauto->isChecked())
    {
        ui->comboBoxsendformat->setEnabled(true);
    }
    else
    {
        ui->comboBoxsendformat->setEnabled(false);
    }
}
void MainWindow::on_radioButtonreadauto_clicked()
{
    if(ui->radioButtonreadauto->isChecked())
    {
        ui->comboBoxreadformat->setEnabled(true);
    }
    else
    {
        ui->comboBoxreadformat->setEnabled(false);
    }
}
void MainWindow::on_radioButtonsendformat_clicked()
{
    if(ui->radioButtonsendauto->isChecked())
    {
        ui->comboBoxsendformat->setEnabled(true);
    }
    else
    {
        ui->comboBoxsendformat->setEnabled(false);
    }
}
void MainWindow::on_radioButtonreadformat_clicked()
{
    if(ui->radioButtonreadauto->isChecked())
    {
        ui->comboBoxreadformat->setEnabled(true);
    }
    else
    {
        ui->comboBoxreadformat->setEnabled(false);
    }
}
void MainWindow::on_comboBoxserial_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    if(this->port->isOpen())
    {
        this->port->close();
        b=false;
        ui->labelstatu->setStyleSheet("border-image: url(:/new/img/red.png);");
    }
    if(ui->pushButtonopen->text()=="关闭")
    {
        ui->pushButtonopen->setText("打开");
    }
    ui->labelstate->setText("选择串口："+ui->comboBoxserial->currentText());
}
// 按钮：刷新串口
void MainWindow::on_pushButtonrefresh_clicked()
{
    ui->comboBoxserial->clear();
    QList<QSerialPortInfo> strlist=QSerialPortInfo::availablePorts();
    QList<QSerialPortInfo>::const_iterator iter;
    for(iter=strlist.constBegin();iter!=strlist.constEnd();++iter)
    {
        ui->comboBoxserial->addItem((*iter).portName());
    }
}
// 更新数据
void MainWindow::updateData(QLineSeries* s, qreal num)
{
    int i;
    QVector<QPointF> oldData = s->pointsVector();
    QVector<QPointF> data;
    if(oldData.size() < 100){
        data = oldData;
    }else
    {
        /* 添加之前老的数据到新的vector中，不复制最前的数据，即每次替换前面的数据
         * 由于这里每次只添加1个数据，所以为1，使用时根据实际情况修改
         */
        for (i = 1; i < oldData.size(); ++i) {
            data.append(QPointF(i - 1 , oldData.at(i).y()));
        }
    }
    data.append(QPointF(data.size(), num));
    s->replace(data);
}

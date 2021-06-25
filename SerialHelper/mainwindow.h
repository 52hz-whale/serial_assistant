#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>

#include <QWidget>
#include <QChart>
#include <QLineSeries>
#include <QVector>
#include <QTimer>

#include "chartview.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    //串口相关
    QSerialPort *port;
    Ui::MainWindow *ui;
    bool b; // 串口是否打开的flag
    void showformat(const QByteArray &arr);
    void sendformat(const QString &strdata);
    inline QSerialPort::DataBits getbit(int bit)        //
    {
        switch (bit)
        {
        case 5:
            return QSerialPort::Data5;
        case 6:
            return QSerialPort::Data6;
        case 7:
            return QSerialPort::Data7;
        default:
            return QSerialPort::Data8;
        }
    }
    // 绘制相关
    ChartView *chartView;
    QChart *chart;
    // 不同的曲线
    QLineSeries *series_cwm_roll;
    QLineSeries *series_cwm_pitch;
    QLineSeries *series_cwm_yaw;
    QLineSeries *series_mi_roll;
    QLineSeries *series_mi_pitch;
    QLineSeries *series_mi_yaw;

    bool isStopping;
    void updateData(QLineSeries* s, qreal num);

private slots:
    // 串口相关槽
    void readread();
    void on_pushButtonopen_clicked();
    void on_checkBoxtextformat_stateChanged(int arg1);
    void on_pushButtonsend_clicked();
    void on_pushButtonclear_clicked();
    void on_pushButtonhelp_clicked();
    void on_checkBoxsendformat_stateChanged(int arg1);
    void on_pushButtoncleasend_clicked();
    void on_radioButtonsendauto_clicked();
    void on_radioButtonreadauto_clicked();
    void on_radioButtonsendformat_clicked();
    void on_radioButtonreadformat_clicked();
    void on_comboBoxserial_currentIndexChanged(int index);
    void on_pushButtonrefresh_clicked();
};

#endif // MAINWINDOW_H

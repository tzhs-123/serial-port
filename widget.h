#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QtSerialPort/QSerialPort>
#include<QtSerialPort/QSerialPortInfo>
#include<QString>
#include<QList>
#include<QMessageBox>
#include <QTextCodec>
#include<QFileDialog>
#include<QFile>
#include<QtCharts/QAbstractAxis>
#include<QtCharts/QChart>
#include<QValueAxis>
#include<QChartView>
#include<QStackedBarSeries>
#include<QSplineSeries>
#include<QLineSeries>
QT_CHARTS_USE_NAMESPACE

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

public:
    bool getserialportconfig();//配置串口
    void openaction();//串口打开时的动作
    void closeaction();//串口关闭时的动作

private slots:
    void on_openbut_clicked();

    void on_closebut_clicked();

    void on_sendbut_clicked();

    void on_totextbut_clicked();

    void on_SerialPort_readyread();//接收槽函数
    void on_drawbut_clicked();
    void on_clearrecv_clicked();

    void on_clearsend_clicked();

private:
    Ui::Widget *ui;
public:
    QSerialPort mserialport;
    QString mportname;//端口号
    QString mbaudrate;//波特率
    QString mparity;//校验位
    QString mdatabit;//数据位
    QString mstopbit;//停止位
    bool mIsopen;
};

#endif // WIDGET_H

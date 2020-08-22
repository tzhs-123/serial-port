#include "widget.h"
#include "ui_widget.h"

//配置串口信息 配置成功返回true
bool Widget::getserialportconfig()
{
    //获取串口配置
    mportname = ui->setcom->currentText();//获取当前端口号
    mbaudrate = ui->setbaudrate->currentText();//获取波特率
    mparity = ui->setparitybit->currentText();//获取校验位
    mdatabit = ui->setdatabit->currentText();//获取数据位
    mstopbit = ui->setstopbit->currentText();//获取停止位

    //设置串口配置
    //串口号
    mserialport.setPortName(mportname);
    //波特率
    if("9600" == mbaudrate)
    {
        mserialport.setBaudRate(QSerialPort::Baud9600);
    }
    else if("2400" == mbaudrate)
    {
        mserialport.setBaudRate(QSerialPort::Baud2400);
    }
    else
    {
        mserialport.setBaudRate(QSerialPort::Baud115200);
    }
    //校验位
    if("NONE"==mparity)
    {
        mserialport.setParity(QSerialPort::NoParity);
    }
    else if("ODD"==mparity)
    {
        mserialport.setParity(QSerialPort::OddParity);
    }
    else
    {
        mserialport.setParity(QSerialPort::EvenParity);
    }
    //数据位
    if("5"==mdatabit)
    {
        mserialport.setDataBits(QSerialPort::Data5);
    }
    else if("6"==mdatabit)
    {
        mserialport.setDataBits(QSerialPort::Data6);
    }
    else if("7"==mdatabit)
    {
        mserialport.setDataBits(QSerialPort::Data7);
    }
    else
    {
        mserialport.setDataBits(QSerialPort::Data8);
    }
    //停止位
    if("1.5"==mstopbit)
    {
        mserialport.setStopBits(QSerialPort::OneAndHalfStop);
    }
    else if("2"==mstopbit)
    {
        mserialport.setStopBits(QSerialPort::TwoStop);
    }
    else
    {
        mserialport.setStopBits(QSerialPort::OneStop);
    }

    return mserialport.open(QSerialPort::ReadWrite);//配置完成后 以读写方式打开串口，并返回打开成功
}

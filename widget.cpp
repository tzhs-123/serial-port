#include "widget.h"
#include "ui_widget.h"
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    mIsopen = false;

    this->setWindowTitle("串口助手");
    //设置初始界面状态
    //默认接收和发送选择文本模式
    ui->sendtxtmode->setChecked(true);
    ui->recvtxtmode->setChecked(true);
    //串口未打开时，关闭按钮和发送按钮都为灰色无法按下
    ui->closebut->setEnabled(false);
    ui->sendbut->setEnabled(false);

    //找到用户可以打开的串口,并添加到串口号的下拉框中
    QList<QSerialPortInfo> serialPortinfo = QSerialPortInfo::availablePorts();
    int count = serialPortinfo.count();
    for(int i = 0;i<count;i++)
    {
        ui->setcom->addItem(serialPortinfo.at(i).portName());
    }
    connect(&mserialport,SIGNAL(readyRead()),this,SLOT(on_SerialPort_readyread()));//接收信号和槽
}
void Widget::openaction()//开启串口后，所有下拉框关闭不可选，打开按钮不可选，关闭和发送按钮变为可选
{
    mIsopen = true;
    ui->openbut->setEnabled(false);
    ui->closebut->setEnabled(true);
    ui->sendbut->setEnabled(true);
    ui->setcom->setEnabled(false);
    ui->setbaudrate->setEnabled(false);
    ui->setparitybit->setEnabled(false);
    ui->setdatabit->setEnabled(false);
    ui->setstopbit->setEnabled(false);
}
void Widget::closeaction()//关闭串口后，所有下拉框变为可选，关闭和发送按钮变为不可选，打开按钮可选
{
    mIsopen = false;
    mserialport.close();//关闭串口（open 操作已在config.cpp中完成）
    ui->openbut->setEnabled(true);
    ui->closebut->setEnabled(false);
    ui->sendbut->setEnabled(false);
    ui->setcom->setEnabled(true);
    ui->setbaudrate->setEnabled(true);
    ui->setparitybit->setEnabled(true);
    ui->setdatabit->setEnabled(true);
    ui->setstopbit->setEnabled(true);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_openbut_clicked()//开始按钮的槽函数
{
    if(getserialportconfig())
    {
        openaction();//若配置完成且打开，则进行打开后动作
    }
    else
    {
        QMessageBox::warning(this, "打开串口失败", QString("原因:") + mserialport.errorString());//若打开失败，弹出信息框提示
    }
}


void Widget::on_closebut_clicked()
{
    closeaction();//关闭按钮按下时，关闭串口，并进行关闭后操作
}

void Widget::on_sendbut_clicked()
{
    if(this->mIsopen)
    {
        QString contentString = this->ui->sendtext->toPlainText();
        if(!contentString.isEmpty())
        {
            QByteArray send_arr;
            if(this->ui->sendtxtmode->isChecked())
            {
                QTextCodec* gbk = QTextCodec::codecForName("gbk");
                send_arr = gbk->fromUnicode(contentString.toLocal8Bit().data());
                mserialport.write(send_arr);
            }
            else if(this->ui->sendHEXmode->isChecked())
            {
                int content_length = contentString.length();
                char hexchar, hexchar_next;
                for(int i = 0; i < content_length; i++)
                {
                    hexchar = contentString[i].toLower().toLatin1();
                    switch(hexchar)
                    {
                        case '\r':
                        case '\n':
                        case ' ':
                            break;
                        default:
                            if(hexchar >= '0' && hexchar <= '9')
                            {
                                if(i + 1 < content_length)
                                {
                                    hexchar_next = contentString[i + 1].toLower().toLatin1();
                                    if(hexchar_next >= '0' && hexchar_next <= '9')
                                    {
                                        send_arr.append((char)(((hexchar - 0x30) << 4) | (hexchar_next - 0x30)));
                                        i++;
                                    }
                                    else if(hexchar_next >= 'a' && hexchar_next <= 'f')
                                    {
                                        send_arr.append((char)(((hexchar - 0x30) << 4) | (hexchar_next - 'a' + 10)));
                                        i++;
                                    }
                                    else if(hexchar_next == '\n' || hexchar_next == '\r' || hexchar_next == ' ')
                                    {
                                        send_arr.append((char)(hexchar - 0x30));
                                        i++;
                                    }
                                    else
                                    {
                                        send_arr.append((char)(hexchar - 0x30));
                                        i = content_length;
                                    }
                                }
                                else
                                {
                                    send_arr.append((char)(hexchar - 0x30));
                                }
                            }
                            else if(hexchar >= 'a' && hexchar <= 'z')
                            {
                                if(i + 1 < content_length)
                                {
                                    hexchar_next = contentString[i + 1].toLower().toLatin1();
                                    if(hexchar_next >= '0' && hexchar_next <= '9')
                                    {
                                        send_arr.append((char)(((hexchar - 'a' + 10) << 4) | (hexchar_next - 0x30)));
                                        i++;
                                    }
                                    else if(hexchar_next >= 'a' && hexchar_next <= 'z')
                                    {
                                        send_arr.append((char)(((hexchar - 'a' + 10) << 4) | (hexchar_next - 'a' + 10)));
                                        i++;
                                    }
                                    else if(hexchar_next == '\n' || hexchar_next == '\r' || hexchar_next == ' ')
                                    {
                                        send_arr.append((char)(hexchar - 'a' + 10));
                                        i++;
                                    }
                                    else
                                    {
                                        send_arr.append((char)(hexchar - 'a' + 10));
                                        i = content_length;
                                    }
                                }
                                else
                                {
                                    send_arr.append((char)(hexchar - 'a' + 10));
                                }
                            }
                            else
                            {
                                i = content_length;
                            }
                            break;
                    }
                }
//                qDebug()<<send_arr;
                mserialport.write(send_arr);
            }
        }
    }
}
void Widget::on_SerialPort_readyread()
{
    QByteArray buf;
    buf = mserialport.readAll();
//    ui->recvtext->moveCursor(QTextCursor::EndOfLine);
    if(!buf.isEmpty())
   {
//        qDebug()<<tr(buf);
        if(ui->recvtxtmode->isChecked() == true)   //文本模式
        {
            ui->recvtext->insertPlainText(buf);
        }
        else if(ui->recvHEXmode->isChecked() == true)
        {
            QString str = ui->recvtext->toPlainText();
            QByteArray str1 = buf.toHex().toUpper();
            if (str1.length()>2)
            {
                QByteArray str2 = str1.left(2);
                QByteArray str3 = str1.mid(2);
                while(true)
                {
                    QByteArray str_temp;
                    str_temp.append("0x"+str2.append(" "));
                    str2 = str3.left(2);
                    if(str3.length()<=2)
                    {
                        str_temp.append("0x"+str3.append(" "));
                        str+=tr(str_temp);
                        break;
                    }
                    else
                    {
                        str3 = str3.mid(2);
                    }
                    str+=tr(str_temp);
                }
            }
            else
                str+=tr("0x"+str1);
            str += "  ";
            ui->recvtext->clear();
            ui->recvtext->insertPlainText(str);

        }
        static int cut;
        cut ++;
        ui->recvcount->setText(QString::number(cut)+"次");
        buf.clear();
    }
}

void Widget::on_totextbut_clicked()
{
    QByteArray buf;
    buf = ui->recvtext->toPlainText().toStdString().c_str();

    QFile file;
    file.setFileName("text.txt");
    file.open(QIODevice::ReadWrite);
    file.write(buf);
//    qDebug()<<buf;

    file.close();
}

void Widget::on_drawbut_clicked()
{
//    QPainter painter(ui->widget);
//    QPen pen;
//    pen.setWidth(2);
//    painter.setPen(pen);
//    for(int a = 1;a<Point_array.size();a++)
//    {
//        QPoint p1 = Point_array[a-1];
//        QPoint p2 = Point_array[a];
//        painter.drawLine(p1,p2);
//    }

    QChart* chart = new QChart;
    QSplineSeries* series = new QSplineSeries;
    QSplineSeries*splineseries = new QSplineSeries;
       int i = ui->recvcount->text().toInt();
       QByteArray buf = mserialport.readAll();

    series->setName("波形");
    series->setPen(QPen(Qt::black,2,Qt::SolidLine));
    splineseries->setPen(QPen(Qt::black,2,Qt::SolidLine));
    chart->addSeries(series);
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, 500);
    axisY->setLabelFormat("%d"); //设置刻度的格式
    axisY->setGridLineVisible(true);
    axisY->setTickCount(16);     //设置多少格
    axisY->setMinorTickCount(5); //设置每格小刻度线的数目
    splineseries->append(5,100);
    splineseries->append(10,100);
    splineseries->append(10,10);
    splineseries->append(5,100);
    splineseries->append(4,30);
        //QChart加载轴,并把轴附加到series上
    chart->setAxisY(axisY, series);  //为chart设置X轴,并把轴附加到series线上;
    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0,10);
    axisX->setLabelFormat("%d");
    axisX->setGridLineVisible(true);
    axisX->setTickCount(10);
    axisX->setMinorTickCount(0);
    chart->setAxisX(axisX,series);
    chart->addSeries(splineseries);
    QChartView *chartView=new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->resize(800,600);
    chartView->show();

}

void Widget::on_clearrecv_clicked()
{
    ui->recvtext->clear();
    ui->recvcount->setText("0次");
}

void Widget::on_clearsend_clicked()
{
    ui->sendtext->clear();
}


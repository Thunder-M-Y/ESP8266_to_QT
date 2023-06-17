#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostAddress>
#include<QMessageBox>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);




    //创建客户端Socket
    server = new QTcpSocket(this);
    //连接服务器
    server->connectToHost(QHostAddress("192.168.4.1"), 8086);

    //接收服务器数据
    connect(server, &QTcpSocket::readyRead,this, &MainWindow::slotReadyRead);
    //发送按钮
    connect(ui->pushButton, &QPushButton::clicked,this, &MainWindow::slotSendMsg);

    series=new QLineSeries();
        series->setPen(QPen(Qt::blue,1,Qt::SolidLine));
        series->setColor(QColor(0,0,255));
        QChart *chart=new QChart();
        chart->addSeries(series);
        chart->setTitle("湿度");
        chart->createDefaultAxes();
        chart->axisX()->setRange(0, 20);
        chart->axisY()->setRange(0, 100);

        ui->graphicsView->setChart(chart);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotReadyRead()
{
    // 接收数据
    QByteArray array = server->readAll();
    QString message = QString::fromLocal8Bit(array);
    //QMessageBox::information(this, "Server Message", message);


        // 解析接收到的数据
        if (message.startsWith("humidity:"))
        {
            // 提取湿度数值部分
            QString humidityValue = message.mid(9); // 从索引为9的位置开始提取

            // 提取湿度值
            bool ok;
            qreal humidity = humidityValue.toDouble(&ok);
            if (ok)
            {
                QPointF dataPoint(my_x++, humidity);
                series->append(dataPoint); // 添加最新的数据点


                if (series->count() > 20)
                             {   series->remove(0);
                   ui->graphicsView->chart()->axisX()->setRange(my_x-20, my_x-1);}



                }




        }



}



void MainWindow::slotSendMsg()
{
    QString text = ui->textEdit->toPlainText();
     //发送数据
    server->write(text.toUtf8());
    ui->textEdit->clear();
}


void MainWindow::on_pushButton_clicked()
{

}

void MainWindow::on_pushButton_2_clicked()
{
   QString text1 = "ON";
  server->write(text1.toUtf8());






}

void MainWindow::on_pushButton_3_clicked()
{
   QString text2 = "OFF";
       server->write(text2.toUtf8());

}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QByteArray>
#include <QValueAxis>
#include <QtCharts>
QT_CHARTS_USE_NAMESPACE


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void slotReadyRead();
    void slotSendMsg();
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    QTcpSocket* server;

    Ui::MainWindow *ui;
    QLineSeries *series;
    QList <QPointF> qPoint;
    int my_x=0,my_y=0;
    QChart *chart = new QChart();


};

#endif // MAINWINDOW_H

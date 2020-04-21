#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTimer>

//声明为 C 库
extern "C"
{
    #include <stdio.h>
    #include <fcntl.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <termios.h>
    #include <unistd.h>
    #include <string.h>
    #include "yuyv.h"
}


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void connected_ok();   //链接服务器成功

    void disconnect_ok(); //与服务器断开链接

    void show_stream();  //显示视频流

    void readdata();    //读服务器发过来的数据

    void read_car();   //读卡


private:
    Ui::MainWindow *ui;

    QString in_out_time;  //入场时间

    QTcpSocket *clien;   //tcp通信

    QUdpSocket  *my_udp;    //udp通信

    QTimer *my_time;        //定时器（上传视频流）

    QTimer *my_time1;        //定时器（读卡）

    struct jpg_data jpg_buf;   //一帧图像信息

};
#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTcpServer>
//数据库头文件
#include <QSqlDatabase>
#include <QListWidgetItem>
#include <QStringList>
//#include <QThread>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    friend void *bf1(void *);
    friend void *bf2(void *);

public slots:
    void read_insert();//创建一个新的通信对象
    void read_data();//把进场的车辆信息放到数据库中
    void read_del();//把出场的车辆信息从数据库中删除
    //void bf1();
    //void bf2();

private slots:
    void read_all();//显示进场摄像头传输过来的视频
    void on_pushButton_clicked();
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_tableWidget_cellDoubleClicked(int row, int column);

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;

    QSqlDatabase pc_sql;//设置数据库的类型
    QUdpSocket *pc_udp;
    QTcpServer *pc_tcp;
    QTcpSocket *new_clien[2];
    //QTimer *pc_time;
    //QTimer *pc_time1;
    //定义一个线程
    //my_thread1 *tid1;
    //my_thread2 *tid2;

    int i=0;//链接进来的客户端数
    QString data;    //该车进场时间 
    QString VIP;
    int col;//有多少列
    QStringList  head;//设置表头
    int x; //选中的坐标
    int y; //选中的坐标
    QString table_name;//当前操作的表名字
    int j=0;//多少行
    int chewei=1000;//定义停车场的车位总数

};

#endif // MAINWINDOW_H

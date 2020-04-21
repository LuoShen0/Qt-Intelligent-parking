#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include <QDateTime>
#include <QHostAddress>
#include <QMessageBox>
#include <QElapsedTimer>
#include <QMovie>

#include "iostream"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>

unsigned  int  car_id;    //入场车辆的卡号
QElapsedTimer t;          //读卡时暂停5秒用的
int fd;                  //串口
char buf[7]={0};        //请求 ID卡
QMovie *movie;        //运行gif动图


int set_serial_uart(int ser_fd)  //设置串口信息
{
    struct termios new_cfg,old_cfg;

    /*保存并测试现有串口参数设置，在这里如果串口号等出错，会有相关的出错信息*/
    if	(tcgetattr(ser_fd, &old_cfg) != 0)
    {
        perror("tcgetattr");
        return -1;
    }

    bzero( &new_cfg, sizeof(new_cfg));
    /*原始模式*/
    /* 设置字符大小*/
    new_cfg = old_cfg;
    cfmakeraw(&new_cfg); /* 配置为原始模式 */

    /*波特率为115200*/
    cfsetispeed(&new_cfg, B9600);
    cfsetospeed(&new_cfg, B9600);

    new_cfg.c_cflag |= CLOCAL | CREAD;
    new_cfg.c_cflag &= ~CSIZE;

    /*8位数据位*/
    new_cfg.c_cflag |= CS8;

    /*无奇偶校验位*/
    new_cfg.c_cflag &= ~PARENB;

    /*1位停止位*/
    new_cfg.c_cflag &= ~CSTOPB;
    /*清除串口缓冲区*/
    tcflush( ser_fd,TCIOFLUSH);
    new_cfg.c_cc[VTIME] = 0;
    new_cfg.c_cc[VMIN] = 1;
    tcflush ( ser_fd, TCIOFLUSH);
    /*串口设置使能*/
    tcsetattr( ser_fd ,TCSANOW,&new_cfg);
}

int save_pic(char *pic,struct jpg_data *jpg_buf)   //保存抓拍时的图片
{
     //创建文件
    int fd=open(pic,O_RDWR|O_CREAT|O_TRUNC,0777);
        if(fd < 0)
        {
            perror("");
            return 0;
        }

    //把数据写入到文件夹中
     write(fd,jpg_buf->jpg_data,jpg_buf->jpg_size);

    close(fd);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

     ui->label->setText("欢迎光临！请刷卡入场！");   //提示信息

     ui->label->setScaledContents(true);
            movie = new QMovie("/play/cunche.gif"); //新建一个播放器，传入动图的路径
            ui->label_10->setMovie(movie);
            movie->start();


    //打开串口1
    fd = open("/dev/ttySAC1" , O_RDWR|O_NOCTTY);
    if(fd < 0)
    {
        perror ("open failed 1");

    }

    //初始化串口属性
    set_serial_uart(fd);

    //请求 ID卡
    buf[0] = 0x07;
    buf[1] = 0x02;
    buf[2] = 0x41;
    buf[3] = 0x01;
    buf[4] = 0x52;

    //配置校验和
    char BCC = 0;
    int i=0;
    for(i=0; i<(buf[0]-2); i++)
    {
         BCC ^= buf[i];
    }

    buf[5] = ~BCC;
    buf[6] = 0x03;

    clien = new QTcpSocket();           //为tcp客户端对象分配空间
    QHostAddress ip("192.168.95.8");    //tcp服务器的ip
    unsigned short port = 8849;         //tcp服务器的端口
    clien->connectToHost(ip,port);      //连接tcp服务器

    //关联信号与槽（接收服务器发过来的信息）
    connect(clien,SIGNAL(readyRead()),this,SLOT(readdata()));

    //关联信号与槽（连接成功）
    connect(clien,SIGNAL(connected()),this,SLOT(connected_ok()));

    //关联信号与槽（断开连接成功）
    connect(clien,SIGNAL(disconnected()),this,SLOT(disconnect_ok()));

    my_udp  = new QUdpSocket(this);        //为udp发送端对象分配空间
    my_udp->bind(QHostAddress::Any,6666); //绑定本地端口

    linux_v4l2_yuyv_init("/dev/video7"); //初始化摄像头设备
    linux_v4l2_start_yuyv_capturing();//开启摄像头的捕捉画面功能

    my_time = new QTimer(this);
    my_time1 = new QTimer(this);

    connect(my_time,SIGNAL(timeout()),this,SLOT(show_stream()));//循环获取并且上传图像
    my_time->start(10);

    connect(my_time1,SIGNAL(timeout()),this,SLOT(read_car()));//循环读卡
    my_time1->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::show_stream()    //把摄像头图像流上传给服务器
{

    //显示视频流抓取一针图像
    bzero(&jpg_buf,sizeof(jpg_buf));
    linux_v4l2_get_yuyv_data(&jpg_buf);

    /*
    QPixmap pic;
    //初始化图像数据
    pic.loadFromData(jpg_buf.jpg_data,jpg_buf.jpg_size);

    pic = pic.scaled(150,250);

    //显示到标签中
    ui->label_10->setPixmap(pic);
    */

    //获取到一帧就发送一帧
    long long size = my_udp->writeDatagram((const char *)jpg_buf.jpg_data,jpg_buf.jpg_size,QHostAddress("192.168.95.8"),8850);
}

void MainWindow::readdata()  //接收服务器发过来的剩余车位
{
    //读取所有数据
    QString chewei  = clien->readAll();

    if(chewei != nullptr)
    {
        //显示实时车位
         ui->label_7->setText(chewei);
    }

}

void MainWindow::read_car()  //不停的读卡，当用户刷卡时会暂停5秒给用户拿走卡
{
    //发送请求帧
    tcflush(fd,TCIOFLUSH);
    write(fd,buf,7);

    //读取从机(RFID模块)应答的数据
    char r_buf[8];
    read(fd,r_buf,8);

    //判断从机返回的状态
    if(r_buf[2] == 0x00)
    {
        printf("从机应答成功\n");

        //进行防碰撞 获取卡号
        char buf1[8]={0};

        buf1[0] = 0x08;
        buf1[1] = 0x02;
        buf1[2] = 0x42;
        buf1[3] = 0x02;
        buf1[4] = 0x93;
        buf1[5] = 0x00;
        //配置校验和
        char BCC = 0;
        int i=0;
        for(i=0; i<(buf1[0]-2); i++)
        {
            BCC ^= buf1[i];
        }

        buf1[6] = ~BCC;
        buf1[7] = 0x03;

        //发送防碰撞请求
        tcflush(fd,TCIOFLUSH);
        write(fd,buf1,8);

        //获取 最大的卡号
        char r_buf1[10]={0};
        read(fd,r_buf1,10);
        if(r_buf1[2] == 0)
        {
            printf("获取卡号成功\n");
            printf("%x %x %x %x\n",r_buf1[4],r_buf1[5],r_buf1[6],r_buf1[7]);
            car_id = r_buf1[7] << 24 | r_buf1[6] << 16 |  r_buf1[5]<<8 | r_buf1[4];
            printf("car_id=%x\n",car_id);

           // save_pic("jin.jpg",&jpg_buf);

            QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
            in_out_time = time.toString("yyyy-MM-dd hh:mm:ss"); //设置显示格式
            ui->label_2->setText(in_out_time);//显示时间

            ui->label_5->setText(QString::number(car_id));    //显示卡号

            QString msg = in_out_time + "/" + QString::number(car_id) +"/"+"in"; //拼接时间和卡号
            clien->write(msg.toUtf8());          //把时间、卡号以及进出标记发送给服务器

            //暂停5秒，让用户拿走卡
            t.start();
             while(t.elapsed() < 5000)
             {
                 QCoreApplication::processEvents();
             }

        }
        else
        {
            printf("获取卡号失败\n");
        }
    }
    else
    {
        printf("从机应答失败\n");
    }
}

void MainWindow::on_pushButton_clicked()   //该按键已废弃
{

}

void MainWindow::on_pushButton_3_clicked() //该按键已废弃
{

}

void MainWindow::on_pushButton_2_clicked() //该按键已废弃
{

}
void MainWindow::connected_ok()    //链接服务器成功/
{
    qDebug()<<"tcp connect ok"<<endl;
}

void MainWindow::disconnect_ok()    //与服务器断开链接
{
    qDebug()<<"tcp disconnect ok"<<endl;
}


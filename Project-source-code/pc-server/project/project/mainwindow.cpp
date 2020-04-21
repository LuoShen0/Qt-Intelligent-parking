#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QFileDialog>
#include <QHostAddress>
#include <QTcpSocket>

//百度识别用到
#include "iostream"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>

//添加第三方头文件
#include "ocr.h"
//声明为 C 库
extern "C"
{
    #include <unistd.h>
    #include <stdio.h>
    #include <fcntl.h>
    //#include <syspes.h>
    #include <sys/stat.h>
    #include <termios.h>
    #include <string.h>
    #include <pthread.h>
}

QString kahao="0";
QString msg2;
double sf4=0;//停车收费多少
QByteArray shiping;   //开发板传过来的总视频
QByteArray shiping1;  //入口视频
QByteArray shiping2;  //出口视频
int ii;
QString  text1;
QString  text2;

int p=0;
int p1=0;


void *bf1(void *arg)    //播报入口车辆信息
{
    while(1)
    {
        if(p == 1)
        {
               MainWindow *tmp1 = (MainWindow *)arg;
               QString str = QString("欢迎车牌为%1的车主进入停车场，请减速慢行").arg(text1);

               //把要播报的内容写进wav文件
               QString str1 = QString("./tts_offline_sample %1").arg(str);
               chdir("/home/gec/text_to_talk/bin/");
               system(str1.toUtf8());

               //播报
               system("aplay 1.wav");
               chdir("/home/gec/project/project/");
               p = 0;
               break;
         }
    }
}

void *bf2(void *arg)  //播报出口车辆信息以及消费情况
{
    while(1)
    {
        if(p1 == 1)
        {
                MainWindow *tmp2 = (MainWindow *)arg;

                QString str = QString("车牌为%1的车主准备离开停车场，本次停车消费为%2元").arg(text2).arg(sf4);

                //把要播报的内容写进wav文件
                QString str1 = QString("./tts_offline_sample %1").arg(str);
                chdir("/home/gec/text_to_talk/bin/");
                system(str1.toUtf8());

                //播报
                system("aplay 1.wav");
                chdir("/home/gec/project/project/");
                p1 = 0;
        }
    }
}

int seva_pic(char *pic,QByteArray *shiping)   //拍照
{   
    //创建文件
   int fd=open(pic,O_RDWR|O_CREAT|O_TRUNC,0777);
       if(fd < 0)
       {
           perror("");
           return 0;
       }

     //把数据写入到文件夹中
     write(fd,shiping->data(),shiping->size());

     close(fd);

}
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    pc_sql =  QSqlDatabase::addDatabase("QSQLITE");//设置数据库的类型

    pc_udp = new QUdpSocket(this);  //为udp分配空间

    pc_tcp = new QTcpServer(this);  //为tcp分配空间

    pthread_t  pid1;
    pthread_create(&pid1,NULL,bf1,(void *)this);

    pthread_t  pid2;
    pthread_create(&pid2,NULL,bf2,(void *)this);

    //pc_time = new QTimer(this);
    //pc_time1 = new QTimer(this);

    //绑定本地地址
    pc_udp->bind(QHostAddress::Any,8850);

    //设置服务器为监听模式
    if(pc_tcp->listen(QHostAddress("192.168.95.8"),8849))
    {
        qDebug() << "设置监听模式成功"  << endl;
    }
    else
    {
        qDebug() << "设置监听模式失败"  << endl;
    }

    //关联可读信号
    connect(pc_tcp,SIGNAL(newConnection()),this,SLOT(read_insert()));
    connect(pc_udp,SIGNAL(readyRead()),this,SLOT(read_all()));

    //connect(pc_time,SIGNAL(timeout()),this,SLOT(bf1()));  //进场播报
    //connect(pc_time1,SIGNAL(timeout()),this,SLOT(bf2()));   //出场播报

    //pc_time->start(1000);
    //pc_time1->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::read_all() //显示进场摄像头传输过来的视频
{
    QHostAddress addr;//ip
    quint16 port;//端口号

    while (pc_udp->hasPendingDatagrams())
    {
        //获取一帧图像
        shiping.resize(pc_udp->pendingDatagramSize());
        pc_udp->readDatagram(shiping.data(), shiping.size(), &addr, &port);

        QPixmap pic;
        pic.loadFromData(shiping);

        //分割地址
        QString addr_msg = addr.toString();
        addr_msg.remove(0, addr_msg.lastIndexOf(":")+1);

        if(addr_msg == "192.168.95.4")  //入口
        {
            shiping1 = shiping;
            pic.scaled(ui->label->width(), ui->label->height());
            ui->label->setPixmap(pic);
        }
        else  //出口
        {
            shiping2 = shiping;
            pic.scaled(ui->label_3->width(), ui->label_3->height());
            ui->label_3->setPixmap(pic);
        }
    }
}


void MainWindow::read_insert() //两个tcp通信对象，接收入口和出口发过来的卡号等数据
{
    //注意服务器只 负责接收连接请求 不负责通信，产生新的通信对象
    new_clien[i]  = pc_tcp->nextPendingConnection();

    if(i==0)
    {
        //关联车库入口可读信号
        connect(new_clien[0],SIGNAL(readyRead()),this,SLOT(read_data()));
        i++;
    }
    else
    {
        //关联车库出口可读信号
        connect(new_clien[1],SIGNAL(readyRead()),this,SLOT(read_del()));
    }

}

void MainWindow::read_data() //处理车位、抓拍、识别车牌以及把入口车辆信息添加到数据库
{
     chewei = chewei-1;   //进来一辆车，车位-1

     QString  msg  =  new_clien[0]->readAll(); //读取车库入口发过来的数据

     new_clien[0]->write(QString::number(chewei).toUtf8()); //给入口开发板发送剩余车位信息

     //保存图像
     char pic[100];
     sprintf(pic,"/home/gec/project/project/%d.jpg",ii++);
     seva_pic(pic,&shiping1);  //车辆一进场，就抓拍

     QPixmap pic1(pic);

     pic1 = pic1.scaled(ui->label_8->width(),ui->label_8->height());
     ui->label_8->setPixmap(pic1);

     //提交抓拍到的图片给百度api 识别车牌
     QString fileName = pic; //获得文件的名字

     // 设置APPID/AK/SK
     std::string app_id = "18168648";
     std::string api_key = "akwdEZH17wavAHe1Ml8IlTPa";
     std::string secret_key = "uuj2jKLRLG1a0CAT9FxrRXaABKzKnw4C";
     aip::Ocr client(app_id, api_key, secret_key);


     Json::Value result; //初始化需要识别的图像

     std::string image;
     aip::get_file_content(fileName.toUtf8().data(), &image);

      // 调用通用文字识别（高精度版）
      result = client.accurate_basic(image, aip::null);

      QString  msggg = result.toStyledString().data();

      QJsonParseError err;
      QJsonDocument    json=  QJsonDocument::fromJson(msggg.toUtf8(),&err);
                 if(err.error != QJsonParseError::NoError)
                 {
                     return ;
                 }

      QJsonObject obj    = json.object();

      QJsonArray  arry   = obj.take("words_result").toArray();
      qDebug()  << arry.size() << endl;

        for (int i=0;i<arry.size();i++)
        {

           text1  = arry.at(i).toObject().take("words").toString(); //识别到的车牌

            ui->label_2->setText(text1);
            qDebug() << text1 << endl;

        }

        p = 1;

     QStringList list_msg =  msg.split("/");//把入口发过来的数据进行切割

     QString msg1 = list_msg.at(0);   //进场时间
     kahao = list_msg.at(1);         //进场卡号
     QString msg3 = list_msg.at(2); //进场状态标记(in)
     QString vip = "无";             //会员情况（月卡or年卡）

     QString find = QString("select * from %1 where 卡号='%2';").arg(table_name).arg(kahao);

     qDebug() << "查找：" << find << endl;

     //定义一个数据库管理器
     QSqlQuery  sql_cmd1;

     if(sql_cmd1.exec(find))
     {
          qDebug() << " 执行查找数据库命令成功（入口）"  << endl;
          while (sql_cmd1.next())
          {
              if(sql_cmd1.value(1).toString() == kahao)
              {
                  //delete from  car1 where  卡号='2910033223';
                  QString cmd2 = QString("delete from %1 where 卡号='%2';").arg(table_name).arg(kahao);

                  qDebug() << "删除:" << cmd2 << endl;

                  //定义一个数据库管理器
                  QSqlQuery  sql_cmd2;

                  if(sql_cmd2.exec(cmd2))
                  {
                      qDebug() << " 删除数据成功（入口）"  << endl;
                      break;
                  }

                  else
                  {
                      qDebug() << " 删除数据失败（入口）"  << endl;
                      break;
                  }

              }

          }

          ui->tableWidget->insertRow(j++);//插入一行数据

          //根据车辆入场的车牌号，插入一行数据
          //insert into  表名  values('','','','','');
          QString cmd = QString("insert into %1 values('%2','%3','%4','%5');").arg(table_name).arg(msg1).arg(kahao).arg(msg3).arg(vip);
          //定义一个数据库管理器
          QSqlQuery  sql_cmd;

          if(sql_cmd.exec(cmd))
          {
              qDebug() << " 插入数据成功（入口）"  << endl;
          }

          else
          {
              qDebug() << " 插入数据失败（入口）"  << endl;
          }

     }
     else
     {
         qDebug() << " 执行查找数据库命令失败（入口）"  << endl;
     }
}

void MainWindow::read_del() //处理车位、抓拍、识别车牌、把出口车辆信息添加到数据库以及计算消费情况
{
    chewei = chewei+1;  //车辆出库时，车位+1

    QString  msg  =  new_clien[1]->readAll();//读取出口车辆的数据

    new_clien[0]->write(QString::number(chewei).toUtf8());//给入口发送当前的车位数

    //保存图像
    char pic[100];
    sprintf(pic,"/home/gec/project/project/%d.jpg",ii++);
    seva_pic(pic,&shiping2);  //车辆一出场，就抓拍

    QPixmap pic1(pic);

    pic1 = pic1.scaled(ui->label_9->width(),ui->label_9->height());
    ui->label_9->setPixmap(pic1);

    //提交抓拍到的图片给百度api 识别
    QString fileName =pic;

    // 设置APPID/AK/SK
      std::string app_id = "18168648";
      std::string api_key = "akwdEZH17wavAHe1Ml8IlTPa";
      std::string secret_key = "uuj2jKLRLG1a0CAT9FxrRXaABKzKnw4C";
      aip::Ocr client(app_id, api_key, secret_key);


      //初始化需要识别的图像
      Json::Value result;

      std::string image;
      aip::get_file_content(fileName.toUtf8().data(), &image);


      // 调用通用文字识别（高精度版）
      result = client.accurate_basic(image, aip::null);

           QString  msggg = result.toStyledString().data();

           QJsonParseError err;
           QJsonDocument    json=  QJsonDocument::fromJson(msggg.toUtf8(),&err);
                        if(err.error != QJsonParseError::NoError)
                        {
                            return ;
                        }

           QJsonObject obj    = json.object();

           QJsonArray  arry   = obj.take("words_result").toArray();


           qDebug()  << arry.size() << endl;

           for (int i=0;i<arry.size();i++)
           {

               text2  = arry.at(i).toObject().take("words").toString();  //识别到的车牌

               ui->label_6->setText(text2);
                qDebug()  <<text2 << endl;

           }

           p1 = 1;

    QStringList list_msg =  msg.split("/");//把从出口读到的数据进行切割

    QString msg1 = list_msg.at(0);   //出场时间 2037-07-09 02:33:08
    msg2 = list_msg.at(1);          //出场卡号
    QString msg3 = list_msg.at(2); //出场状态 out

    //把年月日切割出来
    QStringList list_msg1 =  msg1.split(" ");
    QString str = list_msg1.at(0);  //获取到出场的年月日 2037-07-09

    //切割年、月、日
    QStringList list_msg2 =  str.split("-");
    QString year3 = list_msg2.at(0);   //获取到出场的年 2037
    QString moon3 = list_msg2.at(1);  //获取到出场的月 07
    QString day3 = list_msg2.at(2);  //获取到出场的日 09

    //把时分秒切割出来
    QString str1 = list_msg1.at(1);//获取到出场的时分秒 02:33:08

    //切割时、分、秒
    QStringList list_msg3 =  str1.split(":");
    QString hour3 = list_msg3.at(0);    //获取到出场的时 02
    QString min3 = list_msg3.at(1);    //获取到出场的分 33
    QString sec3 = list_msg3.at(2);   //获取到出场的秒 08

    //对获取到的年月日，时分秒进行类型转换（转int型）
    int year4 = year3.toInt();
    int moon4 = moon3.toInt();
    int day4 = day3.toInt();
    int hour4 = hour3.toInt();
    int min4 = min3.toInt();
    int sec4 = sec3.toInt();

    //ui->label_6->setText(msg2);  //显示出场的卡号

    //根据车辆出场的车牌号，添加一行数据 状态为out
    QString cmd = QString("insert into %1 values('%2','%3','%4','0');").arg(table_name).arg(msg1).arg(msg2).arg(msg3);

    //定义一个数据库管理器
    QSqlQuery  sql_cmd;

    if(sql_cmd.exec(cmd))
    {
         qDebug() << " 执行命令失败（出口）"  << endl;

        //根据车辆出场的卡号，查找该车入场时的信息
        QString cmd1 = QString("select * from %1 where 卡号=%2;").arg(table_name).arg(msg2);

        if(sql_cmd.exec(cmd1))
        {
            qDebug() << " 查找成功"  << endl;
            while (sql_cmd.next())
            {
                data = sql_cmd.value(0).toString(); //获取该车进场的时间 2037-07-09 02:33:08

                VIP = sql_cmd.value(3).toString();  //判断是月卡还是年卡

                qDebug() << data  << endl;
                qDebug() << VIP  << endl;

                break;
            }
        }
        else
        {
            qDebug() << " 查找失败失败"  << endl;
        }
    }
    else
    {
        qDebug() << " 执行语句失败（出口）"  << endl;
    }

    QStringList msg_list =  data.split(" "); //对车辆进场的时间进行切割 2037-07-09 02:33:08
    QString data1 = msg_list.at(0); //获取车辆进场的年月日 2037-07-09

    QStringList msg_list1 =  data1.split("-");
    QString year = msg_list1.at(0);//获取车辆进场的年 2037
    QString moon = msg_list1.at(1);//获取车辆进场的月 07
    QString day = msg_list1.at(2);//获取车辆进场的日 09

    QString data2 = msg_list.at(1);//获取车辆进场的时分秒 02:33:08
    QStringList msg_list2 =  data2.split(":");
    QString hour = msg_list2.at(0);//获取车辆进场的时 02
    QString min = msg_list2.at(1);//获取车辆进场的分 33
    QString sec = msg_list2.at(2);//获取车辆进场的秒 08

    //对进场车辆的年月日，时分秒进行类型转换（转int）
    int year1 = year.toInt();
    int moon1 = moon.toInt();
    int day1 = day.toInt();
    int hour1 = hour.toInt();
    int min1 = min.toInt();
    int sec1 = sec.toInt();

    qDebug() << "年：" << year1 << endl;
    qDebug() << "年4：" << year4 << endl;
    qDebug() << "月：" << moon1 << endl;
    qDebug() << "月4：" << moon4 << endl;
    qDebug() << "日：" << day1 << endl;
    qDebug() << "日4：" << day4 << endl;
    qDebug() << "时：" << hour1 << endl;
    qDebug() << "时4：" << hour4 << endl;

    int sf = ((year4 - year1)+(moon4 - moon1)+(day4 - day1))*24;  //计算该车停了几天

    int sf1 = hour4 - hour1;  //计算该车停了几个小时（按小时收费）

    if(VIP == "无")   //如果不是VIP
    {
        if(sf1 < 0)  //停车不够一天
        {
            int sf3 = sf - sf1;//获取总小时数
            sf4 = sf3*3; //计算收费 （每小时3元）
            qDebug() <<"收取的费用为：" << sf4 << endl;
            new_clien[1]->write(QString::number(sf4).toUtf8());//给出口发送应该收取的费用
        }
        if(sf1 == 0) //停车刚好够一天
        {
            sf4 = sf*3;
            qDebug() <<"收取的费用为：" << sf4   << endl;
            new_clien[1]->write(QString::number(sf4).toUtf8());//给出口发送应该收取的费用
        }
        if(sf1 > 0)
        {
            int sf3 = sf + sf1;//获取总小时数
            sf4 = sf3*3;//计算收费 （每小时3元）
            qDebug() <<"收取的费用为：" << sf4 << endl;
            new_clien[1]->write(QString::number(sf4).toUtf8());//给客户端发送应该收取的费用
        }
    }
    if(VIP == "月卡")//如果VIP是月卡0.9
    {
        if(sf1 < 0)
        {
            int sf3 = sf - sf1;//获取总小时数
            sf4 = sf3*3*0.9;//计算收费打九折
            qDebug() <<"收取的费用为：" << sf4 << endl;
            new_clien[1]->write(QString::number(sf4).toUtf8());//给客户端发送应该收取的费用
        }
        if(sf1 == 0)
        {
            sf4 = sf*3*0.9;
            qDebug() <<"收取的费用为：" << sf4   << endl;
            new_clien[1]->write(QString::number(sf4).toUtf8());//给客户端发送应该收取的费用
        }
        if(sf1 > 0)
        {
            int sf3 = sf + sf1;//获取总小时数
            sf4 = sf3*3*0.9;//计算收费
            qDebug() <<"收取的费用为：" << sf4 << endl;
            new_clien[1]->write(QString::number(sf4).toUtf8());//给客户端发送应该收取的费用
        }
    }
    if(VIP == "年卡")  //如果VIP是年卡0.8
    {
        if(sf1 < 0)
        {
            int sf3 = sf - sf1;//获取总小时数
            sf4 = sf3*3*0.8;//计算收费 （每小时3元）
            qDebug() <<"收取的费用为：" << sf4 << endl;
            new_clien[1]->write(QString::number(sf4).toUtf8());//给客户端发送应该收取的费用
        }
        if(sf1 == 0)
        {
            sf4 = sf*3*0.8;
            qDebug() <<"收取的费用为：" << sf4   << endl;
            new_clien[1]->write(QString::number(sf4).toUtf8());//给客户端发送应该收取的费用
        }
        if(sf1 > 0)
        {
            int sf3 = sf + sf1;//获取总小时数
            sf4 = sf3*3*0.8;//计算收费
            qDebug() <<"收取的费用为：" << sf4 << endl;
            new_clien[1]->write(QString::number(sf4).toUtf8());//给客户端发送应该收取的费用
        }
    }
}

void MainWindow::on_pushButton_clicked() //获取本地数据库文件
{
    QString fileName = QFileDialog::getOpenFileName(this,
              tr("Open Image"), "/home/gec", tr("Image Files (*.db)"));

    //设置路径
    pc_sql.setDatabaseName(fileName);

    //2.打开数据库
    if(pc_sql.open())
    {
        QMessageBox::warning(this, tr("温馨提示"),
                                   tr("打开数据库成功"),
                                   QMessageBox::Ok);

        //获取表格
        QStringList table_list  = pc_sql.tables();

        ui->listWidget->clear();
        ui->listWidget->addItems(table_list);

    }
    else
    {
        QMessageBox::warning(this, tr("温馨提示"),
                                   tr("打开数据库失败"),
                                   QMessageBox::Ok);
    }
}

//显示数据库里面的内容，并把数据库的内容添加进表格中
void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    j = 0;
    col = 0;
    head.clear();
    //获取表格信息设置表格
    QSqlQuery  sql_cmd;
    QString cmd;
    //pragma table_info(表名)；
    table_name = item->text();
    cmd = QString("pragma table_info(%1);").arg(item->text());
    if(sql_cmd.exec(cmd))
    {
        //获取表格的字段数
        while(sql_cmd.next())
        {
            col++;
            head<< sql_cmd.value(1).toString();
        }
    }
    else
    {
        QMessageBox::warning(this, tr("温馨提示"),
                                   tr("执行命令失败"),
                                   QMessageBox::Ok);
    }
    //设置数据库的表头
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(col);
    ui->tableWidget->setHorizontalHeaderLabels(head);
    //查询数据库中的数据
    cmd = QString("select * from %1;").arg(item->text());
    if(sql_cmd.exec(cmd))
    {
        //获取表格的字段数
        while(sql_cmd.next())
        {
            //插入一行数据
            ui->tableWidget->insertRow(j);

            //设置数据
            for(int i=0;i<col;i++)
            {
                QString data  = sql_cmd.value(i).toString();

                //设置到表格中
                QTableWidgetItem *it  = new QTableWidgetItem(data);
                ui->tableWidget->setItem(j,i,it);
            }

            j++;
        }
    }
    else
    {
        QMessageBox::warning(this, tr("温馨提示"),
                                   tr("执行命令失败"),
                                   QMessageBox::Ok);
    }
}


void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column)  //获取表格的行号
{
    //取行与列
    qDebug() << row << endl;
    qDebug() << column << endl;

    x = row; //行
    y = column;//列

    //取出原来的数据
    QTableWidgetItem  *it   = ui->tableWidget->takeItem(row,column);
    if(it == NULL)
    {
        qDebug()  << " IS  nullptr" << endl;
        return ;
    }
    ui->label_7->setText(it->text());
}


void MainWindow::on_pushButton_2_clicked() //管理员为车辆办理月卡或者年卡
{
    QSqlQuery  sql_cmd;
    QString cmd;
    QString  s =  head.at(y);
    //update  表名   set id=    where  id=

    cmd = QString("update %1 set %2='%3' where %4='%5';").arg(table_name).arg(s).arg(ui->lineEdit->text()).arg(s).arg(ui->label_7->text());
    qDebug() << cmd << endl;
    //命令如何设计
    if(sql_cmd.exec(cmd))
    {
        qDebug() << " 执行语句成功"  << endl;

    }
    else {
        qDebug() << " 执行语句失败"  << endl;
    }
}

































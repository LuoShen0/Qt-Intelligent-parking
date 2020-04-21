/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_5;
    QLabel *label_4;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *label_6;
    QLabel *label_10;
    QLabel *label_3;
    QLabel *label_11;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(800, 480);
        MainWindow->setStyleSheet(QStringLiteral("background-color: rgb(65, 65, 65);"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        label = new QLabel(centralwidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 90, 431, 31));
        QFont font;
        font.setPointSize(15);
        label->setFont(font);
        label->setStyleSheet(QStringLiteral("color: rgb(255, 0, 0);"));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 260, 231, 31));
        label_2->setStyleSheet(QStringLiteral("color: rgb(255, 0, 0);"));
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(90, 310, 161, 31));
        label_5->setStyleSheet(QStringLiteral("color: rgb(255, 0, 0);"));
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(10, 360, 141, 31));
        label_4->setStyleSheet(QStringLiteral("color: rgb(255, 0, 0);"));
        label_7 = new QLabel(centralwidget);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(40, 410, 201, 51));
        QFont font1;
        font1.setPointSize(25);
        label_7->setFont(font1);
        label_7->setStyleSheet(QStringLiteral("color: rgb(85, 255, 0);"));
        label_8 = new QLabel(centralwidget);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(280, 0, 361, 71));
        label_8->setFont(font1);
        label_8->setStyleSheet(QStringLiteral("color: rgb(255, 0, 0);"));
        label_9 = new QLabel(centralwidget);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(10, 210, 231, 31));
        label_9->setStyleSheet(QStringLiteral("color: rgb(255, 0, 0);"));
        label_6 = new QLabel(centralwidget);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(10, 310, 61, 31));
        label_6->setStyleSheet(QStringLiteral("color: rgb(255, 0, 0);"));
        label_10 = new QLabel(centralwidget);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(320, 200, 480, 300));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(650, -20, 140, 220));
        label_3->setStyleSheet(QStringLiteral("border-image: url(:/pp.jpg);"));
        label_11 = new QLabel(centralwidget);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(10, 150, 431, 31));
        label_11->setFont(font);
        label_11->setStyleSheet(QStringLiteral("color: rgb(255, 0, 0);"));
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        label->setText(QString());
        label_2->setText(QString());
        label_5->setText(QApplication::translate("MainWindow", "0000", 0));
        label_4->setText(QApplication::translate("MainWindow", "\346\234\254\346\254\241\345\201\234\350\275\246\346\266\210\350\264\271\357\274\232", 0));
        label_7->setText(QApplication::translate("MainWindow", "0", 0));
        label_8->setText(QApplication::translate("MainWindow", "\345\244\251\346\262\263\345\234\260\344\270\213\345\201\234\350\275\246\345\234\272", 0));
        label_9->setText(QApplication::translate("MainWindow", "\345\207\272\345\234\272\346\227\266\351\227\264\357\274\232", 0));
        label_6->setText(QApplication::translate("MainWindow", "\345\215\241\345\217\267\357\274\232", 0));
        label_10->setText(QString());
        label_3->setText(QString());
        label_11->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

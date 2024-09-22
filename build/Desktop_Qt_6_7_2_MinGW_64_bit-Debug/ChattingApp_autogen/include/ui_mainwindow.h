/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QTextEdit *OutWindow;
    QTextEdit *sendEdit;
    QPushButton *SendButton;
    QListWidget *clientsListWidget;
    QLineEdit *IPEdit;
    QLineEdit *PortEdit;
    QPushButton *ConnectButton;
    QLineEdit *nameEdit;
    QFrame *frame;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1295, 677);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        OutWindow = new QTextEdit(centralwidget);
        OutWindow->setObjectName("OutWindow");
        OutWindow->setGeometry(QRect(10, 220, 1271, 301));
        OutWindow->setReadOnly(true);
        sendEdit = new QTextEdit(centralwidget);
        sendEdit->setObjectName("sendEdit");
        sendEdit->setGeometry(QRect(10, 520, 631, 111));
        SendButton = new QPushButton(centralwidget);
        SendButton->setObjectName("SendButton");
        SendButton->setGeometry(QRect(650, 540, 80, 61));
        clientsListWidget = new QListWidget(centralwidget);
        clientsListWidget->setObjectName("clientsListWidget");
        clientsListWidget->setGeometry(QRect(10, 10, 161, 201));
        IPEdit = new QLineEdit(centralwidget);
        IPEdit->setObjectName("IPEdit");
        IPEdit->setGeometry(QRect(180, 160, 113, 24));
        PortEdit = new QLineEdit(centralwidget);
        PortEdit->setObjectName("PortEdit");
        PortEdit->setGeometry(QRect(180, 190, 113, 24));
        ConnectButton = new QPushButton(centralwidget);
        ConnectButton->setObjectName("ConnectButton");
        ConnectButton->setGeometry(QRect(300, 180, 80, 24));
        nameEdit = new QLineEdit(centralwidget);
        nameEdit->setObjectName("nameEdit");
        nameEdit->setGeometry(QRect(180, 130, 113, 24));
        frame = new QFrame(centralwidget);
        frame->setObjectName("frame");
        frame->setGeometry(QRect(170, 80, 231, 141));
        frame->setFrameShape(QFrame::Shape::StyledPanel);
        frame->setFrameShadow(QFrame::Shadow::Raised);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1295, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        SendButton->setText(QCoreApplication::translate("MainWindow", "Send", nullptr));
        IPEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "IP Address", nullptr));
        PortEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "Port Number", nullptr));
        ConnectButton->setText(QCoreApplication::translate("MainWindow", "Connect", nullptr));
        nameEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "Your name", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

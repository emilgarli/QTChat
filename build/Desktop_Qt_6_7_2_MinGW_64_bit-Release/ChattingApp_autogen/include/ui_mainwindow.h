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
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
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
    QPushButton *VoIPButton;
    QLabel *ActiveVoice;
    QPushButton *bAddFile;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1295, 677);
        QIcon icon(QIcon::fromTheme(QIcon::ThemeIcon::InsertLink));
        MainWindow->setWindowIcon(icon);
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
        VoIPButton = new QPushButton(centralwidget);
        VoIPButton->setObjectName("VoIPButton");
        VoIPButton->setGeometry(QRect(1170, 520, 111, 111));
        QIcon icon1(QIcon::fromTheme(QIcon::ThemeIcon::CallStart));
        VoIPButton->setIcon(icon1);
        ActiveVoice = new QLabel(centralwidget);
        ActiveVoice->setObjectName("ActiveVoice");
        ActiveVoice->setGeometry(QRect(1110, 520, 61, 21));
        bAddFile = new QPushButton(centralwidget);
        bAddFile->setObjectName("bAddFile");
        bAddFile->setGeometry(QRect(650, 600, 31, 31));
        QIcon icon2(QIcon::fromTheme(QIcon::ThemeIcon::MailAttachment));
        bAddFile->setIcon(icon2);
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
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Encrypted chat", nullptr));
        SendButton->setText(QCoreApplication::translate("MainWindow", "Send", nullptr));
        IPEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "IP Address", nullptr));
        PortEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "Port Number", nullptr));
        ConnectButton->setText(QCoreApplication::translate("MainWindow", "Connect", nullptr));
        nameEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "Your name", nullptr));
#if QT_CONFIG(tooltip)
        VoIPButton->setToolTip(QCoreApplication::translate("MainWindow", "Start a call", nullptr));
#endif // QT_CONFIG(tooltip)
        VoIPButton->setText(QString());
        ActiveVoice->setText(QString());
        bAddFile->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

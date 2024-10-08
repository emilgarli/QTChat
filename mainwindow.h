#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <qlistwidget.h>
#pragma once
#include <QMainWindow>
#include "ConnectionHandler.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void handleUpdateUI(const QString &message);
    void handleShowImage(const QByteArray &imageData);
    void handleUpdateClientList(const QString &clientName);
    void handleRemoveFromClientList(const QString &clientName);
    bool voiceActive = false;
private slots:
    void on_SendButton_clicked();

    void on_sendEdit_textChanged();

    void on_clientsListWidget_itemClicked(QListWidgetItem *item);

    void on_IPEdit_editingFinished();

    void on_PortEdit_textEdited(const QString &arg1);

    void on_ConnectButton_clicked();

    void on_nameEdit_textChanged(const QString &arg1);

    void on_VoIPButton_clicked();

    void on_bAddFile_clicked();

private:
    Ui::MainWindow *ui;
    ConnectionHandler* handler;
    std::string currentClient = "";
    std::string myName = "";
    std::string IPAddress = "10.0.0.24";
    int portNumber = 17590;

    bool newMesIsImage = false;
    QString imgTag = "";
};
#endif // MAINWINDOW_H

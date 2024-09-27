#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QThread>
#include "map"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , handler(new ConnectionHandler)
{
    ui->setupUi(this);
    //This thread will handle all incomming connections
    std::thread lisThread(&ConnectionHandler::listenThread, handler);
    lisThread.detach();
    // Create a separate thread for the connection handler
    QThread *thread = new QThread(this);
    handler->moveToThread(thread);

    // Connect signal from ConnectionHandler to MainWindow slot
    connect(handler, &ConnectionHandler::updateUI, this, &MainWindow::handleUpdateUI);
    connect(handler, &ConnectionHandler::updateClientList, this, &MainWindow::handleUpdateClientList);
    // Start the thread
    thread->start();
}
//handleUpdateClientList("asd");
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleUpdateUI(const QString &message) {
    // Safely update the UI from the signal
    ui->OutWindow->append(message);
}

void MainWindow::handleUpdateClientList(const QString &message) {
    ui->clientsListWidget->addItem(message);
}

void MainWindow::on_SendButton_clicked()
{
    // Get the current client’s ActiveConnection from the handler
    std::map<std::string, ActiveConnection*> conMap = handler->getConMap();

    // Ensure currentClient is valid and exists in the map
    if (conMap.find(currentClient) != conMap.end()) {
        if(ui->sendEdit->toPlainText()!=""){
            QString message = ui->sendEdit->toPlainText();

            conMap[currentClient]->writeHandler(message.toStdString().c_str(), message.length());
            ui->OutWindow->append("[YOU]: " + message);
            ui->sendEdit->clear();
        }

    } else {
        // Handle case where currentClient is not found in conMap
        ui->OutWindow->append("No active connection for the selected client.");
    }
}



void MainWindow::on_sendEdit_textChanged()
{
}


void MainWindow::on_clientsListWidget_itemClicked(QListWidgetItem *item)
{
    currentClient = item->text().toStdString();
}


void MainWindow::on_IPEdit_editingFinished()
{
    IPAddress = ui->IPEdit->text().toStdString();
}


void MainWindow::on_PortEdit_textEdited(const QString &arg1)
{
    portNumber = ui->PortEdit->text().toInt();
}


void MainWindow::on_ConnectButton_clicked()
{
    if(IPAddress != "" && portNumber >= 0){
        std::thread connectionThread(&ConnectionHandler::connectToPeer, handler, IPAddress, portNumber, 0);
        connectionThread.detach();
    }
    else
        ui->OutWindow->append("You need to set IP and port number of desired peer.");
}


void MainWindow::on_nameEdit_textChanged(const QString &arg1)
{
    myName = ui->nameEdit->text().toStdString();
    handler->setUsername(myName);
}


void MainWindow::on_VoIPButton_clicked()
{
    if(!voiceActive){
        ui->OutWindow->append("Starting voice call...");
        //Pass 1 as third argument, as this means a voice connection
        std::thread connectionThread(&ConnectionHandler::connectToPeer, handler, IPAddress, portNumber, 1);
        connectionThread.detach();
    }
    else{
        ui->OutWindow->append("Closing voice chat.");
        voiceActive = false;
    }

}


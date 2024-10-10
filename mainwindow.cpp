#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QThread>
#include <QFileDialog>
#include <QBuffer>
#include "map"
#include <string>

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
    connect(handler, &ConnectionHandler::removeFromClientList, this, &MainWindow::handleRemoveFromClientList);
    connect(handler, &ConnectionHandler::showImage, this, &MainWindow::handleShowImage);
    // Start the thread
    thread->start();
}
//handleUpdateClientList("asd");
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleRemoveFromClientList(const QString &clientName) {
    // Find the item with the text matching clientName
    QList<QListWidgetItem *> items = ui->clientsListWidget->findItems(clientName, Qt::MatchExactly);

    // If any item is found, remove it
    if (!items.isEmpty()) {
        delete ui->clientsListWidget->takeItem(ui->clientsListWidget->row(items[0]));
    }
}


void MainWindow::handleUpdateClientList(const QString &message) {
    ui->clientsListWidget->addItem(message);
}

void MainWindow::handleUpdateUI(const QString &message) {
    // Safely update the UI from the signal
    ui->OutWindow->append(message);
}

void MainWindow::handleShowImage(const QByteArray &imageBuffer) {

    if (!imageBuffer.isEmpty()) {
        // Decode the image from base64
        QByteArray decodedImage = QByteArray::fromBase64(imageBuffer);

        // Load the image from the decoded QByteArray
        QPixmap image;
        if (image.loadFromData(decodedImage)) {
            // Convert the QPixmap to base64
            QByteArray byteArray;
            QBuffer buffer(&byteArray);
            buffer.open(QIODevice::WriteOnly);
            image.save(&buffer, "PNG");

            // Encode the image in base64 format for HTML insertion
            QString base64Image = QString::fromLatin1(byteArray.toBase64().data());

            // Create the HTML image tag using the base64 encoded image
            QString imgTag = QString("<img src=\"data:image/png;base64,%1\" width=\"100\" height=\"100\" />").arg(base64Image);

            // Insert the image tag into the QTextEdit (sendEdit)
            ui->sendEdit->insertHtml(imgTag);
            ui->sendEdit->insertPlainText("\n");

            // Mark that the message is an image
            newMesIsImage = true;
        } else {
            ui->sendEdit->append("Failed to load image from QByteArray");
        }
    }
}



void MainWindow::on_SendButton_clicked()
{
    // Get the current client’s ActiveConnection from the handler
    std::map<std::string, ActiveConnection*> conMap = handler->getConMap();

    // Ensure currentClient is valid and exists in the map
    if (conMap.find(currentClient) != conMap.end()) {
        if(ui->sendEdit->toPlainText()!=""){
            //If the message is not an image
            if(!newMesIsImage){
                QString message = ui->sendEdit->toPlainText();

                conMap[currentClient]->writeHandler(message.toStdString().c_str(), message.length());
                ui->OutWindow->append("[YOU]: " + message);
                ui->sendEdit->clear();
            }
            //If the message is an image
            else{
                handler->connectToPeer(IPAddress, portNumber, 2);
                std::map<std::string, ActiveConnection*> fileConMap = handler->getFileConMap();
                ui->OutWindow->insertHtml(imgTag);
                ui->OutWindow->insertPlainText("\n");

                ui->sendEdit->clear();
                handleShowImage(rawImageData);

                std::string metaData = std::to_string(rawImageData.size());

                metaData += ",imgName";
                fileConMap[currentClient]->writeHandler(metaData.c_str(), metaData.length());
                fileConMap[currentClient]->writeFile(rawImageData.data(), rawImageData.size());
            }
            newMesIsImage = false;
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

//Connects to the entered IP and Port Number
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

//Attempts to start a voice chat witht he selected user
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

//Opens an explorer and lets the user select an image to send to a peer.
//TODO:
//Add support for multiple file types. not only images
void MainWindow::on_bAddFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Open Image"), "C:/Users", tr("Image Files (*.png *.jpg *.bmp)"));

    if (!fileName.isEmpty()) {
        // Load the image as a QPixmap to check if it is valid
        QPixmap image(fileName);

        // Check if the image is valid
        if (!image.isNull()) {
            imgTag = QString("<img src=\"%1\" width=\"100\" height=\"100\" />").arg(fileName);
            ui->sendEdit->insertHtml(imgTag);
            ui->sendEdit->insertPlainText("\n");
            //Read the actual file and get the raw image data
            //We will access it in the send button handler, and pass it along to the socket
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly)) {
                rawImageData = file.readAll().toBase64();
                file.close();
            }
            // Here we tell the send button that the message is an image, so that the log window OutWindow will
            // know to display the image.
            newMesIsImage = true;
        } else {
            ui->sendEdit->append("Failed to load image");
        }
    }
}



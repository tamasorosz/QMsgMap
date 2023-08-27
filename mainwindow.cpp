//#include "mainwindow.h"
//#include "ui_mainwindow.h"

//#include <QQmlContext>
//#include <QTimer>
//#include <cmath>
//#include <QMessageBox>
//#include <QDebug>
//#include <QThread>
//#include "markermodel.h"

//const int connection_check = 3000; //ms
//const int remove_markers = 1000;   //ms

//MainWindow::MainWindow(QWidget *parent) :
//    QMainWindow(parent),
//    ui(new Ui::MainWindow)
//{
//    Receiver *receiver = new Receiver();

//    ui->setupUi(this);

//    ui->quickWidget->rootContext()->setContextProperty("marker_model", &marker_model);
//    ui->quickWidget->setSource(QUrl("qrc:/mapview.qml"));

//    QTimer *itemlifeTimer = new QTimer();
//    connect(itemlifeTimer, &QTimer::timeout, this, &MainWindow::removeOldItems);
//    itemlifeTimer->start(remove_markers);

//    // checks the connection with rabbitMq
//    QTimer *timer = new QTimer();
//    connect(timer, &QTimer::timeout, this, &MainWindow::onTimeout);
//    timer->start(connection_check);
//    connect(receiver, &Receiver::messageReceived, this, &MainWindow::handle_received_message,  Qt::QueuedConnection);
//}

//MainWindow::~MainWindow()
//{
//    delete ui;
//}

//void MainWindow::handle_received_message(MarkerItem item)
//{
//    marker_model.addMarker(new MarkerItem(item));
//}


//void MainWindow::onTimeout()
//{
//    // checking the connection
//    if (receiver->isConnected()) {

//        ui->label_connection_status->setText("ok");
//        reconnect_counter = 1; // changing back

//    } else
//    {
//        qDebug() << "Receiver is not connected to RabbitMQ server.";
//        if (reconnect_counter<4){
//            ui->label_connection_status->setText("reconnecting");
//            reconnect_counter++;}
//        else{
//            ui->label_connection_status->setText("lost..");
//            reconnect_counter++;}
//    }
//}

//void MainWindow::removeOldItems()
//{
//    marker_model.removeOldItems();
//}

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QQmlContext>
#include <QTimer>
#include <QMessageBox>
#include <QDebug>

const int connection_check = 3000; // ms
const int remove_markers = 1000*180;   // ms

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    receiver(new Receiver),
    itemlifeTimer(new QTimer),
    connectionTimer(new QTimer)
{
    ui->setupUi(this);

    ui->quickWidget->rootContext()->setContextProperty("marker_model", &marker_model);
    ui->quickWidget->setSource(QUrl("qrc:/mapview.qml"));

    connect(itemlifeTimer, &QTimer::timeout, this, &MainWindow::removeOldItems);
    itemlifeTimer->start(remove_markers);

    connect(connectionTimer, &QTimer::timeout, this, &MainWindow::checkConnection);
    connectionTimer->start(connection_check);

    connect(receiver, &Receiver::messageReceived, this, &MainWindow::handleReceivedMessage, Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleReceivedMessage(MarkerItem item)
{
    marker_model.addMarker(new MarkerItem(item));
}

void MainWindow::checkConnection()
{
    // checking the connection
    if (receiver->isConnected()) {
        ui->label_connection_status->setText("ok");
        reconnect_counter = 1; // changing back
    } else {
        qDebug() << "Receiver is not connected to RabbitMQ server.";

        if (reconnect_counter < 5) {
            ui->label_connection_status->setText("reconnecting");
        } else {
            ui->label_connection_status->setText("lost..");
        }
        reconnect_counter *= 2;
        delay(reconnect_counter);
    }
}

void MainWindow::removeOldItems()
{
    marker_model.removeOldItems();
}

void MainWindow::delay(int threshold)
{
    QTime dieTime= QTime::currentTime().addSecs(threshold);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

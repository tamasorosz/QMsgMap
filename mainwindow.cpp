#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QQmlContext>
#include <QTimer>
#include <cmath>
#include <QMessageBox>
#include <QDebug>
#include <QThread>
#include "markermodel.h"

const int refresh_time = 3000;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    Receiver *receiver = new Receiver();
    receiver->start();

    ui->setupUi(this);

    ui->quickWidget->rootContext()->setContextProperty("marker_model", &marker_model);
    ui->quickWidget->setSource(QUrl("qrc:/mapview.qml"));


    QTimer *timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &MainWindow::onTimeout);
    timer->start(refresh_time);

    connect(receiver, &Receiver::messageReceived, this, &MainWindow::handle_received_message,  Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handle_received_message(MarkerItem item)
{
    marker_model.addMarker(new MarkerItem(item));
}


void MainWindow::onTimeout()
{
//    // checking the connection
//    if (receiver.isConnected()) {
//        qDebug() << "Receiver is connected to RabbitMQ server. ";
//        ui->label_connection_status->setText("ok");
//        tries_to_reconnect = 1; // changing back
//    } else
//    {
//        qDebug() << "Receiver is not connected to RabbitMQ server.";
//        ui->label_connection_status->setText("reconnecting");
//        bool status = receiver.attemptReconnect();

//        if(!status){
//        tries_to_reconnect++;
//        ui->label_connection_status->setText("connection lost...");
//        if (tries_to_reconnect>4){
//            QMessageBox::warning(nullptr, "Message Received", QString::fromStdString("Connection lost, I will try to reconnect within 30secs!"));
//        }
//        else{
//        timer = new QTimer(this);
//        timer->setInterval(refresh_time*pow(tries_to_reconnect, 2));
//        }}
//    }
}

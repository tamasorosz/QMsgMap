#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QQmlContext>
#include <QTimer>
#include <cmath>
#include <QMessageBox>
#include <QDebug>
#include <QThread>
#include "markermodel.h"

const double pi = std::acos(-1);
const int refresh_time = 300;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    receiver.start();

    ui->setupUi(this);

    ui->quickWidget->rootContext()->setContextProperty("marker_model", &marker_model);
    ui->quickWidget->setSource(QUrl("qrc:/mapview.qml"));


    QTimer *timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &MainWindow::onTimeout);
    timer->start(refresh_time);
    //marker_model.setMaxMarkers(105);
    //marker_model.moveMarker(NamedCoordinate("Besenyő",QGeoCoordinate(48.15, 20.79)));
}

MainWindow::~MainWindow()
{
    delete ui;
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

    double x = 48.15 + 0.01*std::sin(i*pi/24)+0.1*i;
    double y = 20.79 + 0.01*std::cos(i*pi/24)-0.1*i;

    MarkerItem* marker = new MarkerItem(QPointF(x, y), MarkerItem::marker_observation, QDateTime::currentDateTime(), QString("Point %1").arg(i));
    qDebug()<< marker->position();

    //NamedCoordinate next = NamedCoordinate("Besenyő++",QGeoCoordinate(x, y));

    //marker_model.moveMarker(next);
    marker_model.addMarker(marker);
    i++;
}

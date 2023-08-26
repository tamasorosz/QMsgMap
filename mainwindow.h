#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "markermodel.h"
#include "receiver.h"

#include <QGeoCoordinate>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public Q_SLOTS:
    void handle_received_message(MarkerItem item);
private Q_SLOTS:
    void onTimeout();
private:
    Ui::MainWindow *ui;
    MarkerModel marker_model;
    int i = 0;
    int tries_to_reconnect = 1;
    Receiver *receiver;
    QTimer *timer;
};

#endif // MAINWINDOW_H

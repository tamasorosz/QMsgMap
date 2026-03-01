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

private Q_SLOTS:
    void removeOldItems();
    void handleReceivedMessage(MarkerItem item);
    void checkConnection();

private:
    Ui::MainWindow *ui;
    MarkerModel marker_model;
    int reconnect_counter = 1;
    Receiver *receiver;
    QTimer *itemlifeTimer;
    QTimer *connectionTimer;
    void delay(int);
};

#endif // MAINWINDOW_H

//#ifndef RECEIVER_H
//#define RECEIVER_H

//#include <QObject>
//#include <SimpleAmqpClient/SimpleAmqpClient.h>
//#include <QThread>
//#include <QDebug>
//#include "markermodel.h"
//#include <QTimer>
//#include <QMutex>

//class Receiver : public QObject
//{
//    Q_OBJECT

//public:
//    explicit Receiver(QObject *parent = nullptr);
//    ~Receiver();
//    bool isConnected() const; // Method to check connection status
//    bool attemptReconnect();
//    int getMessageCount(const std::string& queueName);
//    void initConnection();

//public slots:
//    void start();
//    //void stop();
//    void cleanupOnExit();

//signals:
//    void messageReceived(MarkerItem item);

//private:
//    AmqpClient::Channel::ptr_t m_channel; // RabbitMQ channel
//    QThread m_consumerThread; // Thread for message consumption
//    QTimer *m_checkTimer;
//    bool m_isCleaningUp;
//    QMutex m_channelMutex;
//    bool m_stopConsuming;
//    bool declareQueue();

//    //bool deleteQueue();
//    bool queueExists();
//    MarkerItem parseMarkerItem(const QString &jsonString);

//private slots:
//    void consumeMessages();

//};

//#endif // RECEIVER_H

#ifndef RECEIVER_H
#define RECEIVER_H

#include <QObject>
#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <QThread>
#include <QDebug>
#include "markermodel.h"
#include <QTimer>
#include <QMutex>
#include <QWaitCondition>

class Receiver : public QObject
{
    Q_OBJECT

public:
    explicit Receiver(QObject *parent = nullptr);
    ~Receiver();
    bool isConnected() const;
    bool attemptReconnect();
    int getMessageCount(const std::string& queueName);
    void initConnection();

public slots:
    void start();
    void cleanupOnExit();

signals:
    void messageReceived(MarkerItem item);

private:
    AmqpClient::Channel::ptr_t* m_channel;
    QThread *m_consumerThread;
    QTimer *m_checkTimer;
    bool m_isCleaningUp;
    QMutex m_flagMutex;
    QWaitCondition m_reconnectCondition;
    bool m_stopConsuming;
    bool declareQueue();
    MarkerItem parseMarkerItem(const QString &jsonString);

private slots:
    void consumeMessages();

};

#endif // RECEIVER_H

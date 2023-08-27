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

public slots:
    void start();
    void cleanupOnExit();

signals:
    void messageReceived(MarkerItem item);

private:

    // Connect to RabbitMq server
    const std::string hostname = "localhost";
    const int port = 5672;
    const std::string username = "guest";
    const std::string password = "guest";

    const  std::string exchangeName = "positions";
    const  std::string queueName = "receiver_queue"; // Use the same queue name as in your C++ code
    const  std::string routingKey = exchangeName;

    AmqpClient::Channel::ptr_t* m_channel;
    QThread *m_consumerThread;
    bool m_isCleaningUp;
    bool m_disConnected;
    bool declareQueue();
    MarkerItem parseMarkerItem(const QString &jsonString);
    int reconnectionCounter=1;

private slots:
    void consumeMessages();

};

#endif // RECEIVER_H

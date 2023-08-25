#ifndef RECEIVER_H
#define RECEIVER_H

#include <QObject>
#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <QThread>
#include <QDebug>
#include "markermodel.h"

class Receiver : public QObject
{
    Q_OBJECT

public:
    explicit Receiver(QObject *parent = nullptr);
    ~Receiver();
    bool isConnected() const; // Method to check connection status
    bool attemptReconnect();
    NamedCoordinate parseJsonMessage(const std::string &message);


public slots:
    void start();
    void cleanupOnExit();
private:
    AmqpClient::Channel::ptr_t m_channel; // RabbitMQ channel
    QThread* m_consumerThread; // Thread for message consumption
    bool m_isCleaningUp;
    bool declareQueue();
    void consumeMessages();
    bool deleteQueue();
    bool queueExists();
};

#endif // RECEIVER_H


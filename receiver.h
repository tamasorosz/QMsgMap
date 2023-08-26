#ifndef RECEIVER_H
#define RECEIVER_H

#include <QObject>
#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <QThread>
#include <QDebug>
#include "markermodel.h"
#include <QTimer>

class Receiver : public QObject
{
    Q_OBJECT

public:
    explicit Receiver(QObject *parent = nullptr);
    ~Receiver();
    bool isConnected() const; // Method to check connection status
    bool attemptReconnect();
    int getMessageCount(const std::string& queueName);

public slots:
    void start();
    void stop();
    void cleanupOnExit();

signals:
    void messageReceived(MarkerItem item);

private:
    AmqpClient::Channel::ptr_t m_channel; // RabbitMQ channel
    QThread* m_consumerThread; // Thread for message consumption
    QTimer *m_checkTimer;
    bool m_isCleaningUp;
    bool m_stopConsuming;
    bool declareQueue();

    bool deleteQueue();
    bool queueExists();
    MarkerItem parseMarkerItem(const QString &jsonString);

private slots:
    void consumeMessages();

};

#endif // RECEIVER_H


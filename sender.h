#ifndef SENDER_H
#define SENDER_H

#include <QObject>
#include <SimpleAmqpClient/SimpleAmqpClient.h>

class Sender : public QObject
{
    Q_OBJECT

public:
    explicit Sender(QObject *parent = nullptr);

public slots:
    void start();

private:
    AmqpClient::Channel::ptr_t m_channel;
};

#endif // SENDER_H

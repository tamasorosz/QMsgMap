#include "sender.h"

Sender::Sender(QObject *parent) : QObject(parent)
{
    m_channel = AmqpClient::Channel::Create("localhost");
}

void Sender::start()
{
    std::string queue_name = "homework_queue";
    m_channel->DeclareQueue(queue_name);

    std::string message = "Hello, RabbitMQ!";
    AmqpClient::BasicMessage::ptr_t msg = AmqpClient::BasicMessage::Create(message);

    m_channel->BasicPublish("", queue_name, msg);
}

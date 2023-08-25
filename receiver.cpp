#include "receiver.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>


Receiver::Receiver(QObject *parent) : QObject(parent)
{
    m_channel = AmqpClient::Channel::Create();
    connect(this, &QObject::destroyed, this, &Receiver::cleanupOnExit);
}

Receiver::~Receiver()
{
    cleanupOnExit();
    if (m_consumerThread) {
        m_consumerThread->quit();
        m_consumerThread->wait();

        delete m_consumerThread;
        m_consumerThread = nullptr;
    }
}

bool Receiver::isConnected() const
{
    try {
        AmqpClient::Channel::ptr_t tempChannel = AmqpClient::Channel::Create();
        // Declare a temporary queue
        tempChannel->DeclareQueue("temp_queue", false, false, false, false);
        tempChannel->DeleteQueue("temp_queue"); // Delete the temporary queue
        return true; // If no exception, consider connected
    } catch (...) {
        return false; // Exception indicates not connected
    }
}

//GeoCoordinateInfo Receiver::parseJsonMessage(const std::string &message)
//{
//    GeoCoordinateInfo namedCoordinate;

//    QJsonDocument jsonDoc = QJsonDocument::fromJson(QString::fromStdString(message).toUtf8());
//    if (!jsonDoc.isNull() && jsonDoc.isObject()) {
//        QJsonObject jsonObj = jsonDoc.object();

//        namedCoordinate.name = jsonObj.value("name").toString();
//        double longitude = jsonObj.value("long").toDouble();
//        double latitude = jsonObj.value("lat").toDouble();
//        namedCoordinate.position = GeoCoordinateInfo(latitude, longitude);
//    }

//    return namedCoordinate;
//}


bool Receiver::declareQueue()
{
    try {
        if (m_channel) {
            qDebug() << "Deklarálom";
            m_channel->DeclareQueue("homework", false, true, false, false);
            return true;
        } else {
            qDebug() << "Channel is not valid.";
            return false;
        }
    } catch (const std::exception &e) {
        qDebug() << "Error creating queue: " << e.what();
        return false;
    }
}

void Receiver::consumeMessages()
{
    AmqpClient::Envelope::ptr_t envelope;
    while (true)
    {
        try {
            if (declareQueue()) {
                std::string consumer_tag = m_channel->BasicConsume("homework", "", true, false, false);
                envelope = m_channel->BasicConsumeMessage(consumer_tag);
                std::string message = envelope->Message()->Body();
                qDebug() << "Received message: " << QString::fromStdString(message);
                //NamedCoordinate namedCoordinate = parseJsonMessage(message);

                // Process namedCoordinate as needed
                //qDebug() << "Parsed named coordinate: " << namedCoordinate.name << " at " << namedCoordinate.position;

                m_channel->BasicAck(envelope);
            }
        } catch (const AmqpClient::ConsumerCancelledException &e) {
            qDebug() << "Consumer was cancelled: " << e.what();
            // Handle this case as needed, e.g., reconnect or exit the loop
        } catch (const std::exception &e) {
            qDebug() << "Error: " << e.what();
        }
    }
}

void Receiver::start()
{
    if (!m_consumerThread) {
        m_consumerThread = new QThread;
        moveToThread(m_consumerThread);
        connect(m_consumerThread, &QThread::started, this, &Receiver::consumeMessages);

        m_consumerThread->start();
    }
}

bool Receiver::attemptReconnect()
{
    // return: status of the connection
    if (!isConnected()) {
        qDebug() << "Attempting reconnection...";
        try {
            m_channel = AmqpClient::Channel::Create();
            start();
            qDebug() << "Reconnection successful!";
        } catch (const std::exception &e) {
            qDebug() << "Reconnection failed: " << e.what();
            return false;
        }
    }
    return true;
}

bool Receiver::deleteQueue()
{
    try {
        if (m_channel) {
            m_channel->DeleteQueue("homework");
            return true;
        } else {
            qDebug() << "Channel is not valid.";
            return false;
        }
    } catch (const std::exception &e) {
        qDebug() << "Error deleting queue: " << e.what();
        QMessageBox::critical(nullptr, "Error", QString("Error deleting queue: %1").arg(e.what()));
        return false;
    }
}

void Receiver::cleanupOnExit()
{
    if (!m_isCleaningUp) {
        m_isCleaningUp = true; // Set the flag to prevent recursive cleanup

        deleteQueue();
        // Delete the channel if it's not null
        if (m_channel) {
            m_channel.reset(); // Automatically closes the channel when the object is destroyed
            qDebug() << "Channel deleted.";
        }

        m_isCleaningUp = false; // Reset the flag
    }
}


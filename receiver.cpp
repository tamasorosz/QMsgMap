#include "receiver.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>

Receiver::Receiver(QObject *parent) : QObject(parent)
{
    //m_channel = AmqpClient::Channel::Create();
    //connect(this, &QObject::destroyed, this, &Receiver::cleanupOnExit);
    // Connect to RabbitMQ server
    std::string hostname = "localhost";
    int port = 5672;
    std::string username = "guest";
    std::string password = "guest";

    AmqpClient::Channel::ptr_t connectionChannel = AmqpClient::Channel::Create(hostname, port, username, password);
    m_channel = connectionChannel;

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

bool Receiver::declareQueue()
{
    try {
        if (m_channel) {
            qDebug() << "Declaring...";
            std::string exchangeName = "positions";
            // Declare a named queue
            std::string queueName = "homework";
            m_channel->DeclareQueue(queueName, false, true, false, false);

            // Bind the queue to the exchange
            m_channel->BindQueue(queueName, exchangeName, queueName);

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

MarkerItem Receiver::parseMarkerItem(const QString &jsonString)
{
    MarkerItem markerItem(QPointF(0, 0), MarkerItem::marker_observation, QDateTime::currentDateTime(), "");

    QJsonDocument jsonDoc;
    QJsonParseError parseError;
    jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "Error parsing JSON: " << parseError.errorString();
        return markerItem;
    }

    if (!jsonDoc.isObject()) {
        qDebug() << "Error parsing JSON: JSON is not an object";
        return markerItem;
    }

    QJsonObject jsonObject = jsonDoc.object();

    if (!jsonObject.contains("name") || !jsonObject.contains("lon") || !jsonObject.contains("lat")) {
        qDebug() << "Error parsing JSON: Missing required fields";
        return markerItem;
    }

    QJsonValue nameValue = jsonObject["name"];
    if (!nameValue.isString()) {
        qDebug() << "Error parsing JSON: 'name' field is not a string";
        return markerItem;
    }
    QString name = nameValue.toString();

    QJsonValue lonValue = jsonObject["lon"];
    QJsonValue latValue = jsonObject["lat"];
    if (!lonValue.isDouble() || !latValue.isDouble()) {
        qDebug() << "Error parsing JSON: 'lon' and 'lat' fields must be doubles";
        return markerItem;
    }
    double lon = lonValue.toDouble();
    double lat = latValue.toDouble();

    QPointF position(lon, lat);
    MarkerItem::marker_state state = MarkerItem::marker_observation; // Set the appropriate state
    QDateTime when = QDateTime::currentDateTime(); // Set the appropriate time
    QString label = name;

    markerItem = MarkerItem(position, state, when, label);

    return markerItem;
}



void Receiver::consumeMessages()
{
    AmqpClient::Envelope::ptr_t envelope;
    while (true)
    {
        try {
            if (declareQueue()) {
                std::string consumer_tag = m_channel->BasicConsume("", "", true, false, false);
                envelope = m_channel->BasicConsumeMessage(consumer_tag);
                std::string message = envelope->Message()->Body();
                qDebug() << "Received message: " << QString::fromStdString(message);

                MarkerItem markerItem = parseMarkerItem(QString::fromStdString(message));

                emit messageReceived(markerItem);

                qDebug() << "Parsed markerItem: " << markerItem.label() << " at " << markerItem.position();

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


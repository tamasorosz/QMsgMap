#include "receiver.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThreadPool>
#include <QMutex>

// Connect to RabbitMq server
const std::string hostname = "localhost";
const int port = 5672;
const std::string username = "guest";
const std::string password = "guest";

const  std::string exchangeName = "positions";
const  std::string queueName = "receiver_queue"; // Use the same queue name as in your C++ code
const  std::string routingKey = exchangeName;

Receiver::Receiver(QObject *parent) : QObject(parent), m_isCleaningUp(false), m_consumerThread(nullptr)
{
    m_channel = new AmqpClient::Channel::ptr_t(AmqpClient::Channel::Create(hostname, port, username, password));
    connect(this, &QObject::destroyed, this, &Receiver::cleanupOnExit);
    start();
}

Receiver::~Receiver()
{
    cleanupOnExit();
    m_consumerThread->quit();
    m_consumerThread->wait();
    delete m_channel;
}


bool Receiver::isConnected() const
{
    try {
        AmqpClient::Channel::ptr_t tempChannel = AmqpClient::Channel::Create();

        tempChannel->DeclareQueue("temp_queue", false, false, false, false);
        tempChannel->DeleteQueue("temp_queue"); // Delete the temporary queue
        return true;
    } catch (...) {
        return false;
    }
}

bool Receiver::declareQueue()
{
    try {
        if (*m_channel) {

            (*m_channel)->DeclareExchange(exchangeName, AmqpClient::Channel::EXCHANGE_TYPE_DIRECT, false, false, false);
            (*m_channel)->DeclareQueue(queueName, false, true, false, false);
            (*m_channel)->BindQueue(queueName, exchangeName, routingKey);
            return true;}

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

    QPointF position(lat, lon);
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
                        std::string consumer_tag = (*m_channel)->BasicConsume(queueName, "", true, false, false);
                        envelope = (*m_channel)->BasicConsumeMessage(consumer_tag);

                        std::string message = envelope->Message()->Body();

                        MarkerItem markerItem = parseMarkerItem(QString::fromStdString(message));
                        emit messageReceived(markerItem);

                        qDebug() << "Parsed markerItem: " << markerItem.label() << " at " << markerItem.position();
                        m_stopConsuming = false;
                        reconnectionCounter = 1;
                    }
            } catch (const AmqpClient::ConsumerCancelledException &e) {
                    qDebug() << "Consumer was cancelled: " << e.what();
                    m_stopConsuming = true;
                    // Handle this case as needed, e.g., reconnect or exit the loop
            } catch (const std::exception &e) {
                    qDebug() << "Error: " << e.what();
                    m_stopConsuming = true;
            }

            if (m_stopConsuming){

                attemptReconnect();
                reconnectionCounter *= 3; // 1, 3, 9, 27 secs for reconnecting
                QThread::sleep(reconnectionCounter);}
        }
}


void Receiver::start()
{


    m_stopConsuming = false; // Initialize the flag
    m_consumerThread = new QThread;
    moveToThread(m_consumerThread);
    connect(m_consumerThread, &QThread::started, this, &Receiver::consumeMessages);
    m_consumerThread->start();
}


bool Receiver::attemptReconnect()
{
    qDebug() << "Attempting reconnection...";
    try {
        m_channel = new AmqpClient::Channel::ptr_t(AmqpClient::Channel::Create(hostname, port, username, password));    
        return true;
    } catch (const std::exception &e) {
        qDebug() << "Reconnection failed: " << e.what();
        return false;
    }
}

void Receiver::cleanupOnExit()
{
    if (!m_isCleaningUp) {
        m_isCleaningUp = true; // Set the flag to prevent recursive cleanup
        delete m_channel;
        m_isCleaningUp = false; // Reset the flag
    }
}

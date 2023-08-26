#ifndef MESSAGEPROCESSOR_H
#define MESSAGEPROCESSOR_H

#include <QRunnable>
#include <QObject>
#include "markermodel.h"

class MessageProcessor : public QObject, public QRunnable {
    Q_OBJECT
protected:
    MessageProcessor(const QString& message, QObject* parent = nullptr) : QObject(parent), m_message(message) {}

public:
    virtual ~MessageProcessor() {}
    void run() override {
        MarkerItem markerItem = parseMarkerItem(m_message);
        messageProcessed(markerItem);
    }

private:
    QString m_message;
    virtual MarkerItem parseMarkerItem(const QString &jsonString) = 0;
    virtual void messageProcessed(const MarkerItem& markerItem) = 0;
};

class MyMessageProcessor : public MessageProcessor {
    Q_OBJECT
public:
    MyMessageProcessor(const QString& message, QObject* parent = nullptr): MessageProcessor(message, parent), m_message(message) {}

    void run() override {
        MarkerItem markerItem = parseMarkerItem(m_message);
        qDebug()<<"parsed item:"<<markerItem.label()<<markerItem.position();
        emit messageReceived(markerItem);
        //messageProcessed(markerItem);
    }

signals:
    void messageReceived(MarkerItem item);

private:
    QString m_message;
    MarkerItem parseMarkerItem(const QString &jsonString) override;
    void messageProcessed(const MarkerItem& markerItem) override {
        emit messageReceived(markerItem);
    }
};

#endif // MESSAGEPROCESSOR_H

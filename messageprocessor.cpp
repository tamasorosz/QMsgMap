#include "messageprocessor.h"
#include <QJsonDocument>
#include <QJsonObject>
#include "markermodel.h"


MarkerItem MyMessageProcessor::parseMarkerItem(const QString &jsonString) {
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

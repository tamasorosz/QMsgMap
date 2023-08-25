//#ifndef MARKERMODEL_H
//#define MARKERMODEL_H

//#include <QAbstractListModel>
//#include <QGeoCoordinate>
//#include <QDateTime>


//struct NamedCoordinate //:QGeoCoordinate
//{

//    QGeoCoordinate coord;
//    QString name;
//    qint64 creation_date;
//    const QPointF& position() const;
//    QGeoCoordinate coordinate() const;
//    const QString& label() const;

//    NamedCoordinate(const QPointF& pos, const QDateTime& when, const QString& label);


//    NamedCoordinate(QString name, QGeoCoordinate coord){
//        this->creation_date = QDateTime::currentMSecsSinceEpoch();
//        this->name =name;
//        this->coord = coord;
//    }

//    NamedCoordinate(const NamedCoordinate &other){
//        this->coord = other.coord;
//        this->name = other.name;
//    }

//    bool isOlderThan5Minutes() const {
//        qint64 currentTimestamp = QDateTime::currentMSecsSinceEpoch();
//        return (currentTimestamp - creation_date) > (5 * 60 * 1000); // 5 minutes in milliseconds
//    }

//    };

//};

//class MarkerModel : public QAbstractListModel
//{
//    Q_OBJECT
//    Q_PROPERTY(NamedCoordinate current READ current NOTIFY currentChanged)
//public:
//    enum MarkerRoles{
//        PositionRole = Qt::UserRole + 1000,
//        NameRole
//    };
//    explicit MarkerModel(QObject *parent = nullptr);
//    void moveMarker(const NamedCoordinate & coordinate);
//    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
//    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
//    QHash<int, QByteArray> roleNames() const override;

//    int maxMarkers() const;
//    void setMaxMarkers(int maxMarkers=0);
//    NamedCoordinate current() const;
//signals:
//    void currentChanged();
//private:
//    void insert(int row, const NamedCoordinate & coordinate);
//    void removeLastMarker();
//    QList<NamedCoordinate> m_markers;
//    NamedCoordinate m_current;
//    int m_maxMarkers;
//};

//#endif // MARKERMODEL_H

#ifndef MARKERMODEL_H
#define MARKERMODEL_H

#include <QList>
#include <QPointF>
#include <QMetaType>
#include <QDateTime>
#include <QGeoRoute>
#include <QGeoCoordinate>
#include <QAbstractListModel>

struct MarkerItem{
    enum marker_state{
        marker_observation = 0,
        marker_important,
        marker_redundant,
        marker_deleted
    };
    MarkerItem(const QPointF& pos, marker_state state, const QDateTime& when, const QString& label);

    const QPointF& position() const;
    QGeoCoordinate coordinate() const;
    const QString& label() const;
    marker_state state() const;

    void change_state(marker_state state);

private:
    QPointF      _position;
    marker_state _state;
    QString      _label;
    QDateTime    _when;
};

class MarkerModel : public QAbstractListModel{
    Q_OBJECT
    Q_PROPERTY(QGeoRoute* route READ route NOTIFY routeChanged)
public:
    enum MarkerRoles {
        PositionRole = Qt::UserRole + 1,
        StateRole,
        LabelRole
    };
    explicit MarkerModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
public:
    QHash<int, QByteArray> roleNames() const;
private:
    QList<MarkerItem*> _markers;
public:
    void addMarker(MarkerItem* marker);
public:
    QGeoRoute* route() const;
signals:
    void routeChanged();
};

#endif // MARKERMODEL_H
